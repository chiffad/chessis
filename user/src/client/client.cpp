#include "client.h"

#include <QByteArray>
#include <QObject>
#include <QTimer>
#include <QUdpSocket>
#include <functional>
#include <vector>

#include "helper.h"
#include <messages/messages.hpp>
#include <spdlog/spdlog.h>

using namespace cl;

namespace {
const int RESPONSE_WAIT_TIME = 1000;
const int CHECK_CONNECT_TIME = 5000;
const int FIRST_PORT = 49152;
const int LAST_PORT = 49500;
const char FREE_SPASE = ' ';
} // namespace

struct client_t::impl_t
{
  explicit impl_t(const client_t::message_received_callback_t& callback);
  void send(const std::string& message, bool prev_serial_need = false);
  void read();
  bool is_message_received();
  bool is_message_append() const;
  std::string add_serial_num(const std::string& data, bool prev_serial_need = false);
  void begin_wait_receive(const std::string& message);

  QUdpSocket socket_;
  QTimer response_checker_;
  QTimer connection_checker_;

  std::string last_send_message_;
  std::vector<std::string> send_message_stack_;
  int received_serial_num_;
  int send_serial_num_;
  bool received_;

  quint16 my_port_;
  quint16 server_port_;
  const QHostAddress SERVER_IP;

  client_t::message_received_callback_t message_received_callback_;
};

client_t::client_t(const message_received_callback_t& callback)
  : impl_(std::make_unique<impl_t>(callback))
{}

client_t::~client_t()
{}

void client_t::send(const std::string& message)
{
  impl_->send(message);
}

client_t::impl_t::impl_t(const client_t::message_received_callback_t& callback)
  : received_serial_num_(0)
  , send_serial_num_(0)
  , received_(true)
  , server_port_(FIRST_PORT)
  , SERVER_IP(QHostAddress::LocalHost)
  , message_received_callback_(callback)
{
  QObject::connect(&socket_, &QUdpSocket::readyRead, [&]() { read(); });
  QObject::connect(&response_checker_, &QTimer::timeout, [&]() { is_message_received(); });
  QObject::connect(&connection_checker_, &QTimer::timeout, [&]() { send(msg::prepare_for_send(msg::is_server_lost_t())); });

  for (unsigned i = 0; i + FIRST_PORT < LAST_PORT; ++i)
  {
    if (socket_.bind(SERVER_IP, FIRST_PORT + i))
    {
      my_port_ = FIRST_PORT + i;
      SPDLOG_INFO("bind to {}", FIRST_PORT + i);
      break;
    }
    if (i + FIRST_PORT == LAST_PORT)
    {
      i = -1;
    }
  }

  send(msg::prepare_for_send(msg::hello_server_t()));
}

void client_t::impl_t::send(const std::string& message, const bool prev_serial_need)
{
  if (!msg::is_equal_types<msg::message_received_t>(message))
  {
    if (!is_message_received())
    {
      send_message_stack_.push_back(message);
      return;
    }
    begin_wait_receive(message);
  }

  SPDLOG_TRACE("sent={}", message);
  socket_.writeDatagram(QByteArray::fromStdString(add_serial_num(message, prev_serial_need)), SERVER_IP, server_port_);
}

void client_t::impl_t::read()
{
  QHostAddress sender_IP;
  quint16 sender_port;

  QByteArray message;
  message.resize(socket_.pendingDatagramSize());
  socket_.readDatagram(message.data(), message.size(), &sender_IP, &sender_port);

  SPDLOG_TRACE("Read=", message);

  if (sender_IP != SERVER_IP || sender_port != server_port_ || sender_port == my_port_)
  {
    SPDLOG_WARN("Wrong sender id={}, port={}!", sender_IP.toString(), sender_port);
    return;
  }

  const auto datagramm = msg::init<msg::incoming_datagramm_t>(message.toStdString());

  if (datagramm.ser_num != ++received_serial_num_)
  {
    --received_serial_num_;
    if (datagramm.ser_num == received_serial_num_ && !msg::is_equal_types<msg::message_received_t>(datagramm.data))
    {
      connection_checker_.start(CHECK_CONNECT_TIME);
      send(msg::prepare_for_send(msg::message_received_t()), true);
    }
    SPDLOG_WARN("Wrong serial number!");
    return;
  }

  if (!msg::is_equal_types<msg::message_received_t>(datagramm.data))
  {
    send(msg::prepare_for_send(msg::message_received_t()));
    if (!msg::is_equal_types<msg::is_client_lost_t>(datagramm.data))
    {
      message_received_callback_(datagramm.data);
    }
  }
  else
  {
    if (msg::is_equal_types<msg::hello_server_t>(last_send_message_))
    {
      server_port_ = sender_port;
    }

    received_ = true;
    message_received_callback_(msg::prepare_for_send(msg::server_here_t()));
  }
  connection_checker_.start(CHECK_CONNECT_TIME);
}

void client_t::impl_t::begin_wait_receive(const std::string& message)
{
  received_ = false;
  last_send_message_ = message;
  response_checker_.start(RESPONSE_WAIT_TIME);
}

bool client_t::impl_t::is_message_received()
{
  static int num_of_restarts = 0;
  if (received_)
  {
    response_checker_.stop();
    num_of_restarts = 0;
  }
  else
  {
    response_checker_.start(RESPONSE_WAIT_TIME);

    const auto t = msg::init<msg::some_datagramm_t>(last_send_message_).type;
    if (t == msg::id_v<msg::hello_server_t>)
    {
      ++server_port_;
      if (server_port_ == LAST_PORT)
      {
        server_port_ = FIRST_PORT;
      }

      response_checker_.start(RESPONSE_WAIT_TIME / 10);
    }

    socket_.writeDatagram(QByteArray::fromStdString(add_serial_num(last_send_message_, true)), SERVER_IP, server_port_);

    if (msg::id_v<msg::is_server_lost_t> == t || num_of_restarts == 5)
    {
      message_received_callback_(msg::prepare_for_send(msg::server_lost_t()));
    }

    ++num_of_restarts;
  }
  return received_;
}

std::string client_t::impl_t::add_serial_num(const std::string& data, const bool prev_serial_need)
{
  return msg::prepare_for_send(msg::incoming_datagramm_t(data, prev_serial_need ? send_serial_num_ : ++send_serial_num_));
}
