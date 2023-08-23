#include "server/client.hpp"

#include "messages/messages.hpp"

#include <boost/uuid/uuid_io.hpp>
#include <spdlog/spdlog.h>
#include <vector>

namespace {
const char FREE_SPASE = ' ';
}

namespace server {

struct client_t::impl_t
{
  impl_t(io_service_t& io_serv, const endpoint_t& addr, const uuid_t& uuid);
  void message_received(const std::string& message);
  bool is_message_for_server_append() const;
  bool is_message_for_logic_append() const;
  std::string pull_for_server();
  std::string pull_for_logic();

  bool check_ser_num(const msg::incoming_datagramm_t& num);
  void add_for_server(const std::string& message, bool is_extra_message = false);
  void is_message_received();
  void connection_timer_timeout();
  void begin_wait_receive(const std::string& message);
  void start_connection_timer();
  void start_response_timer();

  struct server_mess_t
  {
    server_mess_t(const std::string& m, bool is_extra);
    server_mess_t() = default;
    std::string message;
    bool is_extra;
  };

  deadline_timer_t response_timer_;
  deadline_timer_t connection_timer_;

  std::vector<server_mess_t> messages_for_server_;
  std::vector<std::string> messages_for_logic_;
  std::string last_send_message_;

  credentials_t creds_;
  int elo_;
  bool playing_white_;
  const uuid_t uuid_;

  int received_serial_num_;
  int send_serial_num_;
  bool prev_message_received_;
  endpoint_t address_;
};

client_t::client_t(io_service_t& io_serv, const endpoint_t& addr, const uuid_t& uuid)
  : impl_(std::make_unique<impl_t>(io_serv, addr, uuid))
{}

client_t::~client_t() = default;

void client_t::message_received(const std::string& message)
{
  impl_->message_received(message);
}

void client_t::push_for_send(const std::string& m)
{
  impl_->messages_for_server_.push_back(server_mess_t(m, false));
}

bool client_t::is_message_for_server_append() const
{
  return impl_->is_message_for_server_append();
}

bool client_t::is_message_for_logic_append() const
{
  return impl_->is_message_for_logic_append();
}

std::string client_t::pull_for_server()
{
  return impl_->pull_for_server();
}

std::string client_t::pull_for_logic()
{
  return impl_->pull_for_logic();
}

const endpoint_t& client_t::address() const
{
  return impl_->address_;
}

const client_t::uuid_t& client_t::uuid() const
{
  return impl_->uuid_;
}

void client_t::set_credentials(const credentials_t& creds)
{
  impl_->creds_ = creds;
}

const credentials_t& client_t::credentials() const
{
  return impl_->creds_;
}

void client_t::set_rating(const int rating)
{
  impl_->elo_ = rating;
}

int client_t::rating() const
{
  return impl_->elo_;
}

bool client_t::playing_white() const
{
  return impl_->playing_white_;
}

void client_t::set_playing_white(bool playing_white)
{
  impl_->playing_white_ = playing_white;
}

client_t::impl_t::impl_t(io_service_t& io_serv, const endpoint_t& addr, const uuid_t& uuid)
  : response_timer_(io_serv)
  , connection_timer_(io_serv)
  , elo_{1200}
  , playing_white_{true}
  , received_serial_num_(1)
  , send_serial_num_(0)
  , prev_message_received_(true)
  , address_(addr)
  , uuid_{uuid}
{}

void client_t::impl_t::message_received(const std::string& m)
{
  SPDLOG_INFO("message={}", m);

  const auto datagramm = msg::init<msg::incoming_datagramm_t>(m);

  if (!check_ser_num(datagramm))
  {
    return;
  }

  ++received_serial_num_;
  start_connection_timer();
  const auto t = msg::init<msg::some_datagramm_t>(datagramm.data).type;

  if (t != msg::id_v<msg::message_received_t>)
  {
    add_for_server(msg::prepare_for_send(msg::message_received_t()));
  }

  switch (msg::init<msg::some_datagramm_t>(datagramm.data).type)
  {
    case msg::id_v<msg::message_received_t>:
      SPDLOG_DEBUG("msg::message_received_t");
      prev_message_received_ = true;
      break;
    case msg::id_v<msg::is_server_lost_t>: SPDLOG_DEBUG("msg::is_server_lost_t"); break;
    case msg::id_v<msg::hello_server_t>:
      SPDLOG_DEBUG("msg::hello_server_t");
      add_for_server(msg::prepare_for_send(msg::get_login_t()));
      break;
    default: SPDLOG_ERROR("default"); messages_for_logic_.push_back(datagramm.data);
  }
}

bool client_t::impl_t::is_message_for_server_append() const
{
  return (!messages_for_server_.empty() &&
          (prev_message_received_ || (messages_for_server_.front().is_extra && messages_for_server_.front().message == last_send_message_)));
}

bool client_t::impl_t::is_message_for_logic_append() const
{
  return !messages_for_logic_.empty();
}

std::string client_t::impl_t::pull_for_server()
{
  SPDLOG_DEBUG("pull_for_server");

  const auto _1 = messages_for_server_.front();

  if (!msg::is_equal_types<msg::message_received_t>(_1.message))
  {
    begin_wait_receive(_1.message);
  }

  messages_for_server_.erase(messages_for_server_.begin());

  return msg::prepare_for_send(msg::incoming_datagramm_t(_1.message, _1.is_extra ? send_serial_num_ : ++send_serial_num_));
}

std::string client_t::impl_t::pull_for_logic()
{
  SPDLOG_DEBUG("pull_for_logic");

  const std::string m = messages_for_logic_.front();
  messages_for_logic_.erase(messages_for_logic_.begin());

  return m;
}

bool client_t::impl_t::check_ser_num(const msg::incoming_datagramm_t& _1)
{
  if (_1.ser_num == received_serial_num_ - 1 && !msg::is_equal_types<msg::message_received_t>(_1.data))
  {
    start_connection_timer();
    add_for_server(msg::prepare_for_send(msg::message_received_t()), true);
    return false;
  }

  if (_1.ser_num != received_serial_num_)
  {
    SPDLOG_WARN("Warning! Wrong serial number!");
    return false;
  }

  return true;
}

void client_t::impl_t::add_for_server(const std::string& m, bool is_extra_message)
{
  SPDLOG_DEBUG("add message={}", m);
  auto _1 = server_mess_t(m, is_extra_message);
  if (is_extra_message)
  {
    messages_for_server_.insert(messages_for_server_.begin(), _1);
  }
  else
  {
    messages_for_server_.push_back(_1);
  }
}

void client_t::impl_t::begin_wait_receive(const std::string& message)
{
  prev_message_received_ = false;
  last_send_message_ = message;
  start_response_timer();
}

void client_t::impl_t::is_message_received()
{
  SPDLOG_TRACE("is message received");
  static int num_of_restarts = 0;
  if (prev_message_received_)
  {
    response_timer_.cancel();
    num_of_restarts = 0;
  }
  else
  {
    add_for_server(last_send_message_, true);

    if (msg::is_equal_types<msg::is_client_lost_t>(last_send_message_) && num_of_restarts == 3)
    {
      messages_for_logic_.push_back(msg::prepare_for_send(msg::client_lost_t()));
    }

    ++num_of_restarts;
    start_response_timer();
  }
}

void client_t::impl_t::connection_timer_timeout()
{
  add_for_server(msg::prepare_for_send(msg::is_client_lost_t()));
}

void client_t::impl_t::start_connection_timer()
{
  SPDLOG_TRACE("start_connection_timer();");
  connection_timer_.cancel();
  connection_timer_.expires_from_now(boost::posix_time::milliseconds(7000));
  connection_timer_.async_wait([&](const boost::system::error_code& error) {
    if (!error)
    {
      connection_timer_timeout();
    }
    SPDLOG_DEBUG(">>>>connection_timer<<<< {}", error.message());
  });
  //  connection_timer_.async_wait([&](auto /*e*/){log(">>>>connection_timer<<<<");});
}

void client_t::impl_t::start_response_timer()
{
  SPDLOG_TRACE("start_response_timer();");
  response_timer_.cancel();
  response_timer_.expires_from_now(boost::posix_time::milliseconds(1500));
  response_timer_.async_wait([&](const boost::system::error_code& error) {
    SPDLOG_DEBUG(">>>>response_timer<<<< {}", error.message());
    if (!error)
    {
      is_message_received();
    }
  });
  // response_timer_.async_wait([&](auto /*e*/){log(">>>>response_timer<<<<");});
}

client_t::impl_t::server_mess_t::server_mess_t(const std::string& m, const bool is_extra_message)
  : message(m)
  , is_extra(is_extra_message)
{}

std::ostream& operator<<(std::ostream& os, const client_t& c)
{
  return os << "Client{ uuid=" << boost::uuids::to_string(c.uuid()) << "; creds=" << c.credentials() << "; address" << c.address() << " }";
}

bool operator==(const client_t& lhs, const client_t& rhs)
{
  return lhs.uuid() == rhs.uuid();
}

std::ostream& operator<<(std::ostream& os, const credentials_t& c)
{
  return os << "Credentials{ login=" << c.login << "; pwd=" << c.pwd << " }";
}

bool operator==(const credentials_t& lhs, const credentials_t& rhs)
{
  return lhs.login == rhs.login && lhs.pwd == rhs.pwd;
}

bool operator!=(const credentials_t& lhs, const credentials_t& rhs)
{
  return !(lhs == rhs);
}

} // namespace server