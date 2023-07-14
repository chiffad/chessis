#include "client.h"

#include <QByteArray>
#include <QObject>
#include <QTimer>
#include <QUdpSocket>
#include <functional>
#include <vector>

#include "client/messages.h"
#include "helper.h"
#include <spdlog/spdlog.h>

using namespace cl;

struct client_t::impl_t
{
  explicit impl_t(const client_t::message_received_callback_t& callback);
  void send(const std::string& message, bool is_prev_serial_need = false);
  void read();
  bool is_message_received();
  bool is_message_append() const;
  std::string add_serial_num(const std::string& data, bool is_prev_serial_need = false);
  void begin_wait_receive(const std::string& message);

  QUdpSocket socket;
  QTimer response_checker;
  QTimer connection_checker;

  std::string last_send_message;
  std::vector<std::string> send_message_stack;
  int received_serial_num;
  int send_serial_num;
  bool is_received;

  quint16 my_port;
  quint16 server_port;
  const QHostAddress SERVER_IP;

  client_t::message_received_callback_t message_received_callback_;

  enum
  {
    RESPONSE_WAIT_TIME = 1000,
    CHECK_CONNECT_TIME = 5000,
    FIRST_PORT = 49152,
    LAST_PORT = 49500
  };
  const char FREE_SPASE = ' ';
};

client_t::client_t(const message_received_callback_t& callback)
  : impl(std::make_unique<impl_t>(callback))
{}

client_t::~client_t()
{}

void client_t::send(const std::string& message)
{
  impl->send(message);
}

client_t::impl_t::impl_t(const client_t::message_received_callback_t& callback)
  : received_serial_num(0)
  , send_serial_num(0)
  , is_received(true)
  , server_port(FIRST_PORT)
  , SERVER_IP(QHostAddress::LocalHost)
  , message_received_callback_(callback)
{
  QObject::connect(&socket, &QUdpSocket::readyRead, [&]() { read(); });
  QObject::connect(&response_checker, &QTimer::timeout, [&]() { is_message_received(); });
  QObject::connect(&connection_checker, &QTimer::timeout, [&]() { send(msg::prepare_for_send(msg::is_server_lost_t())); });

  for (unsigned i = 0; i + FIRST_PORT < LAST_PORT; ++i)
  {
    if (socket.bind(SERVER_IP, FIRST_PORT + i))
    {
      my_port = FIRST_PORT + i;
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

void client_t::impl_t::send(const std::string& message, bool is_prev_serial_need)
{
  if (!msg::is_equal_types<msg::message_received_t>(message))
  {
    if (!is_message_received())
    {
      send_message_stack.push_back(message);
      return;
    }
    begin_wait_receive(message);
  }

  SPDLOG_TRACE("sent={}", message);

  socket.writeDatagram(QByteArray::fromStdString(add_serial_num(message, is_prev_serial_need)), SERVER_IP, server_port);
}

void client_t::impl_t::read()
{
  QHostAddress sender_IP;
  quint16 sender_port;

  QByteArray message;
  message.resize(socket.pendingDatagramSize());
  socket.readDatagram(message.data(), message.size(), &sender_IP, &sender_port);

  SPDLOG_TRACE("Read=", message);

  if (sender_IP != SERVER_IP || sender_port != server_port || sender_port == my_port)
  {
    SPDLOG_WARN("Wrong sender id={}, port={}!", sender_IP.toString(), sender_port);
    return;
  }

  const auto datagramm = msg::init<msg::incoming_datagramm_t>(message.toStdString());

  if (datagramm.ser_num != ++received_serial_num)
  {
    --received_serial_num;
    if (datagramm.ser_num == received_serial_num && !msg::is_equal_types<msg::message_received_t>(datagramm.data))
    {
      connection_checker.start(CHECK_CONNECT_TIME);
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
    if (msg::is_equal_types<msg::hello_server_t>(last_send_message))
    {
      server_port = sender_port;
    }

    is_received = true;
    message_received_callback_(msg::prepare_for_send(msg::server_here_t()));
  }
  connection_checker.start(CHECK_CONNECT_TIME);
}

void client_t::impl_t::begin_wait_receive(const std::string& message)
{
  is_received = false;
  last_send_message = message;
  response_checker.start(RESPONSE_WAIT_TIME);
}

bool client_t::impl_t::is_message_received()
{
  static int num_of_restarts = 0;
  if (is_received)
  {
    response_checker.stop();
    num_of_restarts = 0;
  }
  else
  {
    response_checker.start(RESPONSE_WAIT_TIME);

    const auto t = msg::init<msg::some_datagramm_t>(last_send_message).type;
    if (t == msg::id<msg::hello_server_t>())
    {
      ++server_port;
      if (server_port == LAST_PORT)
      {
        server_port = FIRST_PORT;
      }

      response_checker.start(RESPONSE_WAIT_TIME / 10);
    }

    socket.writeDatagram(QByteArray::fromStdString(add_serial_num(last_send_message, true)), SERVER_IP, server_port);

    if (msg::id<msg::is_server_lost_t>() == t || num_of_restarts == 5)
    {
      message_received_callback_(msg::prepare_for_send(msg::server_lost_t()));
    }

    ++num_of_restarts;
  }
  return is_received;
}

std::string client_t::impl_t::add_serial_num(const std::string& data, bool is_prev_serial_need)
{
  return msg::prepare_for_send(msg::incoming_datagramm_t(data, is_prev_serial_need ? send_serial_num : ++send_serial_num));
}
