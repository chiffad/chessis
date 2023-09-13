#include "server/base_client.hpp"

#include <boost/uuid/uuid_io.hpp>
#include <spdlog/spdlog.h>
#include <vector>

namespace server {

struct base_client_t::impl_t
{
  impl_t(io_service_t& io_serv, const endpoint_t& addr);
  void message_received(const std::string& message);
  std::string pull_for_server();
  std::optional<msg::some_datagramm_t> preprocess_message(const std::string& m);

  bool check_ser_num(const msg::incoming_datagramm_t& num);
  void add_for_server(const std::string& message, bool extra_message = false);
  void check_message_received();
  void connection_timer_timeout();
  void begin_wait_receive(const std::string& message);
  void start_connection_timer();
  void start_response_timer();
  void set_connection_status(bool online);

  struct server_mess_t
  {
    std::string message;
    bool extra;
  };

  deadline_timer_t response_timer_;
  deadline_timer_t connection_timer_;

  // TODO: use deque?
  std::vector<server_mess_t> messages_for_server_;
  std::vector<msg::some_datagramm_t> messages_for_logic_;
  std::string last_send_message_;

  int received_serial_num_;
  int send_serial_num_;
  bool prev_message_received_;

  endpoint_t address_;
  bool online_;
  base_client_t::connection_status_signal_t connection_status_signal_;
};

base_client_t::base_client_t(io_service_t& io_serv, const endpoint_t& addr)
  : impl_(std::make_unique<impl_t>(io_serv, addr))
{}

base_client_t::~base_client_t() = default;

std::optional<msg::some_datagramm_t> base_client_t::preprocess_message(const std::string& m)
{
  return impl_->preprocess_message(m);
}

void base_client_t::add_for_server(const std::string& message, bool extra_message)
{
  impl_->add_for_server(message, extra_message);
}

void base_client_t::add_for_logic(msg::some_datagramm_t msg)
{
  impl_->messages_for_logic_.push_back(std::move(msg));
}

void base_client_t::push_for_send(const std::string& m)
{
  impl_->messages_for_server_.push_back(base_client_t::impl_t::server_mess_t{m, false});
}

bool base_client_t::message_for_server_append() const
{
  const auto& msg = impl_->messages_for_server_;
  return !msg.empty() && (impl_->prev_message_received_ || (msg.front().extra && msg.front().message == impl_->last_send_message_));
}

bool base_client_t::message_for_logic_append() const
{
  return !impl_->messages_for_logic_.empty();
}

std::string base_client_t::pull_for_server()
{
  return impl_->pull_for_server();
}

msg::some_datagramm_t base_client_t::pull_for_logic()
{
  SPDLOG_DEBUG("pull_for_logic");
  const msg::some_datagramm_t m = impl_->messages_for_logic_.front();
  impl_->messages_for_logic_.erase(impl_->messages_for_logic_.begin());
  return m;
}

const endpoint_t& base_client_t::address() const
{
  return impl_->address_;
}

bool base_client_t::online() const
{
  return impl_->online_;
}

boost::signals2::connection base_client_t::connect_connection_status_changed(const connection_status_signal_t::slot_type& subscriber)
{
  return impl_->connection_status_signal_.connect(subscriber);
}

base_client_t::impl_t::impl_t(io_service_t& io_serv, const endpoint_t& addr)
  : response_timer_(io_serv)
  , connection_timer_(io_serv)
  , received_serial_num_(1)
  , send_serial_num_(0)
  , prev_message_received_(true)
  , address_(addr)
  , online_(false)
{}

std::optional<msg::some_datagramm_t> base_client_t::impl_t::preprocess_message(const std::string& m)
{
  const auto datagramm = msg::init<msg::incoming_datagramm_t>(m);
  if (!check_ser_num(datagramm))
  {
    return std::nullopt;
  }

  set_connection_status(true);
  ++received_serial_num_;
  start_connection_timer();

  const auto some_datagram = msg::init<msg::some_datagramm_t>(datagramm.data);
  if (some_datagram.type == msg::id_v<msg::message_received_t>)
  {
    prev_message_received_ = true;
    return std::nullopt;
  }

  add_for_server(msg::prepare_for_send(msg::message_received_t()));
  return some_datagram;
}

std::string base_client_t::impl_t::pull_for_server()
{
  const auto msg = messages_for_server_.front();
  if (!msg::is_equal_types<msg::message_received_t>(msg.message))
  {
    begin_wait_receive(msg.message);
  }

  messages_for_server_.erase(messages_for_server_.begin());
  return msg::prepare_for_send(msg::incoming_datagramm_t(msg.message, msg.extra ? send_serial_num_ : ++send_serial_num_));
}

bool base_client_t::impl_t::check_ser_num(const msg::incoming_datagramm_t& datagram)
{
  if (datagram.ser_num == received_serial_num_ - 1 && !msg::is_equal_types<msg::message_received_t>(datagram.data))
  {
    start_connection_timer();
    add_for_server(msg::prepare_for_send(msg::message_received_t()), true);
    return false;
  }

  if (datagram.ser_num != received_serial_num_)
  {
    SPDLOG_WARN("Warning! Wrong serial number!");
    return false;
  }

  return true;
}

void base_client_t::impl_t::add_for_server(const std::string& m, bool extra_message)
{
  SPDLOG_TRACE("add message={}", m);
  auto msg = server_mess_t{m, extra_message};
  if (extra_message)
  {
    messages_for_server_.insert(messages_for_server_.begin(), std::move(msg));
  }
  else
  {
    messages_for_server_.push_back(std::move(msg));
  }
}

void base_client_t::impl_t::begin_wait_receive(const std::string& message)
{
  prev_message_received_ = false;
  last_send_message_ = message;
  start_response_timer();
}

void base_client_t::impl_t::check_message_received()
{
  static int num_of_restarts = 0;
  if (prev_message_received_)
  {
    response_timer_.cancel();
    num_of_restarts = 0;
    return;
  }

  add_for_server(last_send_message_, true);

  if (msg::is_equal_types<msg::is_client_lost_t>(last_send_message_) && ++num_of_restarts == 3)
  {
    set_connection_status(false);
  }

  start_response_timer();
}

void base_client_t::impl_t::connection_timer_timeout()
{
  add_for_server(msg::prepare_for_send(msg::is_client_lost_t()));
}

void base_client_t::impl_t::start_connection_timer()
{
  // SPDLOG_TRACE("Start connection timer for client on address={}", address_);
  connection_timer_.cancel();
  connection_timer_.expires_from_now(boost::posix_time::milliseconds(7000));
  connection_timer_.async_wait([&](const boost::system::error_code& error) {
    if (!error)
    {
      connection_timer_timeout();
      return;
    }
    if (error != boost::system::errc::operation_canceled) SPDLOG_ERROR("connection timer error={}; client address={}", error.message(), address_);
  });
  //  connection_timer_.async_wait([&](auto /*e*/){log(">>>>connection_timer<<<<");});
}

void base_client_t::impl_t::start_response_timer()
{
  // SPDLOG_TRACE("Start response timer for client on address={}", address_);
  response_timer_.cancel();
  response_timer_.expires_from_now(boost::posix_time::milliseconds(1500));
  response_timer_.async_wait([&](const boost::system::error_code& error) {
    if (!error)
    {
      check_message_received();
      return;
    }
    if (error != boost::system::errc::operation_canceled) SPDLOG_ERROR("response timer error={}; client address={}", error.message(), address_);
  });
  // response_timer_.async_wait([&](auto /*e*/){log(">>>>response_timer<<<<");});
}

void base_client_t::impl_t::set_connection_status(const bool online)
{
  if (online == online_) return;

  online_ = online;
  connection_status_signal_(online_);
}

std::ostream& operator<<(std::ostream& os, const base_client_t& c)
{
  return os << "Client{ address" << c.address() << " }";
}

bool operator==(const base_client_t& lhs, const base_client_t& rhs)
{
  return lhs.address() == rhs.address();
}

} // namespace server
