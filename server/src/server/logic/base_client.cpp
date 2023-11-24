#include "server/logic/base_client.hpp"

#include <boost/uuid/uuid_io.hpp>
#include <deque>
#include <messages/messages_eq.hpp>
#include <messages/messages_io.hpp>
#include <spdlog/spdlog.h>

namespace chess::server::logic {

namespace {
enum class received_serial_num_error_t
{
  prev,
  wrong,
  ok
};

struct msg_to_send_t
{
  msg::some_datagram_t message;
  bool extra;
};

} // namespace

struct base_client_t::impl_t
{
  impl_t(io_service_t& io_serv, const endpoint_t& addr);
  void message_received(const std::string& message);
  bool default_preprocess_and_is_futher_processing_needed(const msg::incoming_datagram_t& m);

  received_serial_num_error_t check_ser_num(const msg::incoming_datagram_t& num) const;
  void check_message_received();
  void begin_wait_receive(const msg::some_datagram_t& message);
  void start_connection_timer();
  void start_response_timer();
  void set_connection_status(bool online);

  template<typename T>
  void add_for_send(T&& m, bool extra_message = false)
  {
    SPDLOG_TRACE("add message={}", m);
    msg::some_datagram_t d;
    if constexpr (!std::is_same_v<std::decay_t<T>, msg::some_datagram_t>) d = msg::to_some_datagram(std::forward<T>(m));
    else d = std::forward<T>(m);

    auto msg = msg_to_send_t{std::move(d), extra_message};
    if (extra_message) messages_for_send_.push_front(std::move(msg));
    else messages_for_send_.push_back(std::move(msg));
  }

  deadline_timer_t response_timer_;
  deadline_timer_t connection_timer_;

  std::deque<msg_to_send_t> messages_for_send_;
  std::deque<msg::some_datagram_t> messages_for_logic_;
  msg::some_datagram_t last_send_message_;

  uint64_t received_serial_num_;
  uint64_t send_serial_num_;
  bool prev_message_received_;

  endpoint_t address_;
  bool online_;
  base_client_t::connection_status_signal_t connection_status_signal_;
};

base_client_t::base_client_t(io_service_t& io_serv, const endpoint_t& addr)
  : impl_(std::make_unique<impl_t>(io_serv, addr))
{}

base_client_t::~base_client_t() = default;

bool base_client_t::default_preprocess_and_is_futher_processing_needed(const msg::incoming_datagram_t& m)
{
  return impl_->default_preprocess_and_is_futher_processing_needed(m);
}

void base_client_t::add_for_send(const msg::some_datagram_t& message, bool extra_message)
{
  impl_->add_for_send(message, extra_message);
}

void base_client_t::add_for_logic(msg::some_datagram_t msg)
{
  impl_->messages_for_logic_.push_back(std::move(msg));
}

bool base_client_t::message_for_send_append() const
{
  const auto& msg = impl_->messages_for_send_;
  return !msg.empty() && (impl_->prev_message_received_ || (msg.front().extra && msg.front().message == impl_->last_send_message_));
}

bool base_client_t::message_for_logic_append() const
{
  return !impl_->messages_for_logic_.empty();
}

std::string base_client_t::pull_for_send()
{
  auto msg = std::move(impl_->messages_for_send_.front());
  impl_->messages_for_send_.pop_front();
  if (msg.message.type != msg::id_v<msg::message_received_t>) impl_->begin_wait_receive(msg.message);

  return msg::prepare_for_send(msg::incoming_datagram_t{std::move(msg.message), msg.extra ? impl_->send_serial_num_ : ++impl_->send_serial_num_, impl_->received_serial_num_ + 1});
}

msg::some_datagram_t base_client_t::pull_for_logic()
{
  SPDLOG_DEBUG("pull_for_logic");
  const msg::some_datagram_t m = impl_->messages_for_logic_.front();
  impl_->messages_for_logic_.pop_front();
  return m;
}

const endpoint_t& base_client_t::address() const
{
  return impl_->address_;
}

void base_client_t::set_address(const endpoint_t& addr)
{
  impl_->address_ = addr;
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
  , received_serial_num_(0)
  , send_serial_num_(0)
  , prev_message_received_(true)
  , address_(addr)
  , online_(false)
{}

bool base_client_t::impl_t::default_preprocess_and_is_futher_processing_needed(const msg::incoming_datagram_t& datagram)
try
{
  bool futher_processing_needed = false;
  switch (check_ser_num(datagram))
  {
    case received_serial_num_error_t::prev: add_for_send(msg::message_received_t{}, true); break;
    case received_serial_num_error_t::wrong: break;
    default:
    {
      received_serial_num_ = datagram.ser_num;
      if (datagram.data.type == msg::id_v<msg::message_received_t>) prev_message_received_ = true;
      else
      {
        add_for_send(msg::message_received_t{});
        futher_processing_needed = true;
      }
    }
  }

  start_connection_timer();
  set_connection_status(true); // we should update status only after response as after this signal server logic could send messages
  return futher_processing_needed;
}
catch (const std::exception& ex)
{
  SPDLOG_CRITICAL("Failed to process datagrm={}; reason={}!!!", datagram, ex.what());
  throw;
}

received_serial_num_error_t base_client_t::impl_t::check_ser_num(const msg::incoming_datagram_t& datagram) const
{
  if (datagram.ser_num == received_serial_num_ && datagram.data.type != msg::id_v<msg::message_received_t>)
  {
    return received_serial_num_error_t::prev;
  }

  if (datagram.ser_num != received_serial_num_ + 1)
  {
    SPDLOG_WARN("Warning! Wrong sequence number! received={}; ex={}", datagram.ser_num, received_serial_num_);
    return received_serial_num_error_t::wrong;
  }

  return received_serial_num_error_t::ok;
}

void base_client_t::impl_t::begin_wait_receive(const msg::some_datagram_t& message)
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

  add_for_send(last_send_message_, true);

  if (msg::id_v<msg::is_client_lost_t> == last_send_message_.type && ++num_of_restarts == 3)
  {
    SPDLOG_INFO("Client on address={} went offline!", address_);
    set_connection_status(false);
    return;
  }

  start_response_timer();
}

void base_client_t::impl_t::start_connection_timer()
{
  connection_timer_.cancel();
  connection_timer_.expires_from_now(boost::posix_time::milliseconds(7000));
  connection_timer_.async_wait([&](const boost::system::error_code& error) {
    if (!error)
    {
      add_for_send(msg::is_client_lost_t{});
      return;
    }
    if (error != boost::system::errc::operation_canceled) SPDLOG_ERROR("connection timer error={}; client address={}", error.message(), address_);
  });
}

void base_client_t::impl_t::start_response_timer()
{
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

} // namespace chess::server::logic
