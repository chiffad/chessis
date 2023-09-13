#include "client.h"
#include "client/socket.hpp"
#include "helper.h"
#include <messages/messages.hpp>

#include <QByteArray>
#include <QObject>
#include <QTimer>
#include <QUdpSocket>
#include <functional>
#include <spdlog/spdlog.h>
#include <vector>

namespace cl {

namespace {
const int RESPONSE_WAIT_TIME = 500;
const int CHECK_CONNECT_TIME = 5000;
const int RESEND_HELLO_SERVER_TIME = 40;

} // namespace

struct client_t::impl_t
{
  struct state_t : public std::enable_shared_from_this<state_t>
  {
    explicit state_t(client_t::impl_t& client);
    virtual ~state_t() = default;
    virtual void send(const std::string& data, bool prev_serial_needed) = 0;
    virtual void read() = 0;

    client_t::impl_t& client_;
  };

  struct connect_state_t : public state_t
  {
    connect_state_t(client_t::impl_t& client, std::unique_ptr<connection_strategy_t>& connection_strategy);
    void establish_connection(const bool prev_serial_needed);
    void send(const std::string& data, const bool prev_serial_needed) override;
    void read() override;

    QTimer resend_hello_server_timer_;
    std::unique_ptr<connection_strategy_t>& connection_strategy_;
  };

  struct connected_state_t : public state_t
  {
    explicit connected_state_t(client_t::impl_t& client);
    void send(const std::string& data, const bool prev_serial_needed) override;
    void read() override;
  };

  impl_t(const client_t::message_received_callback_t& callback, const client_t::server_status_changed_callback_t& server_status_changed,
         std::unique_ptr<connection_strategy_t> connection_strategy);
  void resend_prev_message();
  std::string add_serial_num(const std::string& data, bool prev_serial_needed = false);
  void begin_wait_receive(const std::string& data);
  bool validate_serial_num(const msg::incoming_datagramm_t& data);
  inline void send(const std::string& data, bool prev_serial_needed = false) { state_->send(data, prev_serial_needed); }
  inline void set_new_state(std::shared_ptr<state_t> s) { state_ = std::move(s); }
  void write_datagram(const std::string& data, bool prev_serial_needed);

  udp_socket_t socket_;
  QTimer response_checker_timer_;
  QTimer server_alive_timer_;

  std::string last_send_message_;
  std::vector<std::string> messages_to_send_;
  int received_serial_num_;
  int send_serial_num_;
  bool prev_message_sent_;
  endpoint_t endpoint_;
  client_t::message_received_callback_t message_received_callback_;
  client_t::server_status_changed_callback_t server_status_changed_callback_;
  std::unique_ptr<connection_strategy_t> connection_strategy_;
  std::shared_ptr<state_t> state_;
};

client_t::client_t(const message_received_callback_t& callback, const client_t::server_status_changed_callback_t& server_status_changed,
                   std::unique_ptr<connection_strategy_t> connection_strategy)
  : impl_(std::make_unique<impl_t>(callback, server_status_changed, std::move(connection_strategy)))
{}

client_t::~client_t() = default;

void client_t::send(const std::string& data)
{
  impl_->send(data);
}

client_t::impl_t::impl_t(const client_t::message_received_callback_t& callback, const client_t::server_status_changed_callback_t& server_status_changed,
                         std::unique_ptr<connection_strategy_t> connection_strategy)
  : socket_{[this]() { state_->read(); }}
  , received_serial_num_(0)
  , send_serial_num_(0)
  , prev_message_sent_(true)
  , endpoint_{}
  , message_received_callback_(callback)
  , server_status_changed_callback_(server_status_changed)
  , connection_strategy_{std::move(connection_strategy)}
{
  QObject::connect(&response_checker_timer_, &QTimer::timeout, [&]() { resend_prev_message(); });
  QObject::connect(&server_alive_timer_, &QTimer::timeout, [&]() { send(msg::prepare_for_send(msg::is_server_lost_t{})); });

  state_ = std::make_shared<connect_state_t>(*this, connection_strategy_);
}

void client_t::impl_t::begin_wait_receive(const std::string& data)
{
  prev_message_sent_ = false;
  last_send_message_ = data;
  response_checker_timer_.start(RESPONSE_WAIT_TIME);
}

void client_t::impl_t::resend_prev_message()
{
  static int num_of_restarts = 0;
  if (prev_message_sent_)
  {
    response_checker_timer_.stop();
    num_of_restarts = 0;
    return;
  }

  write_datagram(last_send_message_, true);

  if (++num_of_restarts == 5 || msg::id_v<msg::is_server_lost_t> == msg::init<msg::some_datagramm_t>(last_send_message_).type)
  {
    server_status_changed_callback_(false);
  }
}

std::string client_t::impl_t::add_serial_num(const std::string& data, const bool prev_serial_needed)
{
  return msg::prepare_for_send(msg::incoming_datagramm_t(data, prev_serial_needed ? send_serial_num_ : ++send_serial_num_));
}

bool client_t::impl_t::validate_serial_num(const msg::incoming_datagramm_t& datagramm)
{
  if (datagramm.ser_num == ++received_serial_num_) return true;

  SPDLOG_WARN("Wrong serial number! ex={}; received={}", received_serial_num_, datagramm.ser_num);
  --received_serial_num_;
  if (datagramm.ser_num == received_serial_num_ && !msg::is_equal_types<msg::message_received_t>(datagramm.data))
  {
    send(msg::prepare_for_send(msg::message_received_t()), true);
  }

  return false;
}

void client_t::impl_t::write_datagram(const std::string& data, const bool prev_serial_needed)
{
  // SPDLOG_TRACE("Send data={}; to {}", data, endpoint_);
  socket_.write({endpoint_, add_serial_num(data, prev_serial_needed)});
}

//============================ state_t ============================

client_t::impl_t::state_t::state_t(client_t::impl_t& client)
  : client_{client}
{}

//============================ connect_state_t ============================

client_t::impl_t::connect_state_t::connect_state_t(client_t::impl_t& client, std::unique_ptr<connection_strategy_t>& connection_strategy)
  : state_t{client}
  , connection_strategy_{connection_strategy}
{
  assert(connection_strategy_ != nullptr);

  QObject::connect(&resend_hello_server_timer_, &QTimer::timeout, [&]() { establish_connection(true); });

  client_.socket_.bind();
  SPDLOG_DEBUG("Trying to connect to the server");
  establish_connection(false);
  resend_hello_server_timer_.start(RESEND_HELLO_SERVER_TIME);
}

void client_t::impl_t::connect_state_t::establish_connection(const bool prev_serial_needed)
{
  // TODO: make strategy to be able to connect to logic server by provided endpoint and login server with search logic
  // when login_response_t received, client_t should be reinit with new strategy for logic server connection
  // login_response_t should not be processed in controllers logic

  connection_strategy_->exec(client_.endpoint_);
  SPDLOG_INFO("Try to establish connection on address={}", client_.endpoint_);
  client_.write_datagram(msg::prepare_for_send(msg::hello_server_t{}), prev_serial_needed);
}

void client_t::impl_t::connect_state_t::send(const std::string& data, const bool prev_serial_needed)
{
  client_.messages_to_send_.push_back(data);
}

void client_t::impl_t::connect_state_t::read()
{
  const auto data = client_.socket_.read();
  SPDLOG_TRACE("Read data={}", data);

  if (data.endpoint == client_.socket_.endpoint()) return;

  const auto datagramm = msg::init<msg::incoming_datagramm_t>(data.data);
  if (!client_.validate_serial_num(datagramm)) return;
  if (!msg::is_equal_types<msg::message_received_t>(datagramm.data)) return;

  SPDLOG_INFO("Server found on {}", data.endpoint);
  client_.endpoint_.port = data.endpoint.port;
  resend_hello_server_timer_.stop();

  client_.server_status_changed_callback_(true);

  auto self = shared_from_this(); // set_new_state will change current state on new one
  client_.set_new_state(std::make_shared<connected_state_t>(client_));
}

//============================ connected_state_t ============================

client_t::impl_t::connected_state_t::connected_state_t(client_t::impl_t& client)
  : state_t{client}
{}

void client_t::impl_t::connected_state_t::send(const std::string& data, const bool prev_serial_needed)
{
  if (!msg::is_equal_types<msg::message_received_t>(data))
  {
    if (!client_.prev_message_sent_)
    {
      client_.messages_to_send_.push_back(data);
      return;
    }
    client_.begin_wait_receive(data);
  }

  client_.write_datagram(data, prev_serial_needed);
}

void client_t::impl_t::connected_state_t::read()
{
  const auto data = client_.socket_.read();
  SPDLOG_TRACE("Read={}", data.data);

  if (data.endpoint.port == client_.socket_.endpoint().port) return;

  if (!(data.endpoint == client_.endpoint_))
  {
    SPDLOG_WARN("Wrong sender {}! Expected {}", data.endpoint, client_.endpoint_);
    return;
  }

  client_.server_alive_timer_.start(CHECK_CONNECT_TIME);

  const auto datagramm = msg::init<msg::incoming_datagramm_t>(data.data);
  if (!client_.validate_serial_num(datagramm)) return;

  if (msg::is_equal_types<msg::message_received_t>(datagramm.data))
  {
    client_.prev_message_sent_ = true;
    client_.server_status_changed_callback_(true);
    return;
  }

  client_.send(msg::prepare_for_send(msg::message_received_t()));
  if (!msg::is_equal_types<msg::is_client_lost_t>(datagramm.data))
  {
    client_.message_received_callback_(datagramm.data);
  }
}

} // namespace cl