#include "client.h"
#include "client/socket.hpp"
#include "helper.h"

#include <QByteArray>
#include <QObject>
#include <QTimer>
#include <QUdpSocket>
#include <functional>
#include <spdlog/spdlog.h>
#include <vector>

namespace chess::cl {

namespace {
const int RESPONSE_WAIT_TIME = 500;
const int CHECK_CONNECT_TIME = 5000;
const int RESEND_HELLO_SERVER_TIME = 40;

} // namespace

struct client_t::impl_t
{
  struct state_t : public std::enable_shared_from_this<state_t>
  {
    explicit state_t(impl_t& client);
    virtual ~state_t() = default;
    virtual void send(const data_to_send_t& data, bool prev_serial_needed) = 0;
    virtual void read() = 0;

    client_t::impl_t& client_;
  };

  struct connect_state_t : public state_t
  {
    connect_state_t(impl_t& client, std::unique_ptr<connection_strategy_t>& connection_strategy);
    void establish_connection(const bool prev_serial_needed);
    void send(const data_to_send_t& data, const bool prev_serial_needed) override;
    void read() override;

    QTimer resend_hello_server_timer_;
    std::unique_ptr<connection_strategy_t>& connection_strategy_;
  };

  struct connected_state_t : public state_t
  {
    explicit connected_state_t(impl_t& client);
    void send(const data_to_send_t& data, const bool prev_serial_needed) override;
    void read() override;
  };

  impl_t(const message_received_callback_t& callback, const server_status_changed_callback_t& server_status_changed, std::unique_ptr<connection_strategy_t> connection_strategy);
  void resend_prev_message();
  void begin_wait_receive(const data_to_send_t& data);
  bool validate_serial_num(const msg::incoming_datagram_t& data);
  inline void set_new_state(std::shared_ptr<state_t> s) { state_ = std::move(s); }
  void write_datagram(const std::string& data, bool prev_serial_needed);

  udp_socket_t socket_;
  QTimer server_alive_timer_;

  QTimer response_checker_timer_;
  data_to_send_t last_send_message_;
  std::vector<data_to_send_t> messages_to_send_; // TODO: fix: not used!
  uint64_t received_sequence_num_;
  uint64_t send_sequence_num_;
  bool prev_message_sent_;

  endpoint_t endpoint_;
  message_received_callback_t message_received_callback_;
  server_status_changed_callback_t server_status_changed_callback_;
  std::unique_ptr<connection_strategy_t> connection_strategy_;
  std::shared_ptr<state_t> state_;
};

client_t::client_t(const message_received_callback_t& callback, const client_t::server_status_changed_callback_t& server_status_changed,
                   std::unique_ptr<connection_strategy_t> connection_strategy)
  : impl_(std::make_unique<impl_t>(callback, server_status_changed, std::move(connection_strategy)))
{}

client_t::~client_t() = default;

void client_t::send(const data_to_send_t& data)
{
  impl_->state_->send(data, false);
}

client_t::impl_t::impl_t(const client_t::message_received_callback_t& callback, const client_t::server_status_changed_callback_t& server_status_changed,
                         std::unique_ptr<connection_strategy_t> connection_strategy)
  : socket_{[this]() { state_->read(); }}
  , received_sequence_num_(0)
  , send_sequence_num_(0)
  , prev_message_sent_(true)
  , endpoint_{}
  , message_received_callback_(callback)
  , server_status_changed_callback_(server_status_changed)
  , connection_strategy_{std::move(connection_strategy)}
{
  QObject::connect(&response_checker_timer_, &QTimer::timeout, [&]() { resend_prev_message(); });
  QObject::connect(&server_alive_timer_, &QTimer::timeout, [&]() { state_->send(msg::is_server_lost_t{}, false); });

  state_ = std::make_shared<connect_state_t>(*this, connection_strategy_);
}

void client_t::impl_t::begin_wait_receive(const data_to_send_t& data)
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

  write_datagram(last_send_message_.data(), true);

  if (++num_of_restarts == 5 || msg::id_v<msg::is_server_lost_t> == msg::init<msg::some_datagram_t>(last_send_message_.data()).type)
  {
    server_status_changed_callback_(false);
  }
}

bool client_t::impl_t::validate_serial_num(const msg::incoming_datagram_t& datagram)
{
  if (datagram.ser_num == ++received_sequence_num_) return true;

  SPDLOG_WARN("Wrong serial number! ex={}; received={}", received_sequence_num_, datagram.ser_num);
  --received_sequence_num_;
  if (datagram.ser_num == received_sequence_num_ && !msg::is_equal_types<msg::message_received_t>(datagram.data))
  {
    state_->send(msg::message_received_t{}, true);
  }

  return false;
}

void client_t::impl_t::write_datagram(const std::string& data, const bool prev_serial_needed)
{
  std::string to_send = msg::prepare_for_send(msg::incoming_datagram_t{data, prev_serial_needed ? send_sequence_num_ : ++send_sequence_num_, received_sequence_num_ + 1});

  // SPDLOG_TRACE("Send data={}; to {}", to_send, endpoint_);
  socket_.write({endpoint_, std::move(to_send)});
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
  connection_strategy_->exec(client_.endpoint_);
  SPDLOG_INFO("Try to establish connection on address={}", client_.endpoint_);
  client_.write_datagram(msg::prepare_for_send(msg::hello_server_t{}), prev_serial_needed);
}

void client_t::impl_t::connect_state_t::send(const data_to_send_t& data, const bool prev_serial_needed)
{
  client_.messages_to_send_.push_back(data);
}

void client_t::impl_t::connect_state_t::read()
{
  const auto data = client_.socket_.read();
  SPDLOG_TRACE("Read data={}", data);

  if (data.endpoint == client_.socket_.endpoint()) return;

  const auto datagram = msg::init<msg::incoming_datagram_t>(data.data);
  if (!client_.validate_serial_num(datagram)) return;
  if (!msg::is_equal_types<msg::message_received_t>(datagram.data)) return;

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

void client_t::impl_t::connected_state_t::send(const data_to_send_t& data, const bool prev_serial_needed)
{
  if (!msg::is_equal_types<msg::message_received_t>(data.data()))
  {
    if (!client_.prev_message_sent_)
    {
      client_.messages_to_send_.push_back(data);
      return;
    }
    client_.begin_wait_receive(data);
  }

  client_.write_datagram(data.data(), prev_serial_needed);
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

  const auto datagram = msg::init<msg::incoming_datagram_t>(data.data);
  if (!client_.validate_serial_num(datagram)) return;

  if (msg::is_equal_types<msg::message_received_t>(datagram.data))
  {
    client_.prev_message_sent_ = true;
    client_.server_status_changed_callback_(true);
    return;
  }

  send(msg::message_received_t{}, false);
  if (!msg::is_equal_types<msg::is_client_lost_t>(datagram.data))
  {
    client_.message_received_callback_(datagram.data);
  }
}

} // namespace chess::cl
