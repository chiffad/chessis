#include "client.h"

#include <QObject>
#include <QUdpSocket>
#include <QByteArray>
#include <QTimer>

#include "messages.h"
#include "log.h"


using namespace cl;

struct client_t::impl_t
{
  impl_t();
  void send(const QByteArray& message, bool is_prev_serial_need = false);
  void send(const messages::MESSAGE r_mes, bool is_prev_serial_need = false);
  QByteArray pull();
  void read();
  bool is_message_received();
  void connection_checker_timeout();
  bool is_message_append() const;
  QByteArray add_serial_num(const QByteArray& data, bool is_prev_serial_need = false);
  int cut_serial_num(QByteArray& data) const;
  void begin_wait_receive(const QByteArray& message);

  QUdpSocket socket;
  QTimer response_checker;
  QTimer connection_checker;

  QByteArray last_send_message;
  QVector<QByteArray> send_message_stack;
  QVector<QByteArray> received_message_stack;
  int received_serial_num;
  int send_serial_num;
  bool is_received;

  quint16 my_port;
  quint16 server_port;
  const QHostAddress SERVER_IP;

  enum { RESPONSE_WAIT_TIME = 1000, CHECK_CONNECT_TIME = 5000, FIRST_PORT = 49152, LAST_PORT = 49500 };
  const char FREE_SPASE = ' ';
};


client_t::client_t()
    : impl(new impl_t())
{
}

client_t::~client_t()
{
}

void client_t::send(const QByteArray& message)
{
  impl->send(message);
}

QByteArray client_t::pull()
{
  return impl->pull();
}

bool client_t::is_message_append() const
{
  return impl->is_message_append();
}

client_t::impl_t::impl_t()
    : received_serial_num(0), send_serial_num(0), is_received(true),
      server_port(FIRST_PORT), SERVER_IP(QHostAddress::LocalHost)
{
  QObject::connect(&socket, &QUdpSocket::readyRead, [&](){ read(); });
  QObject::connect(&response_checker, &QTimer::timeout, [&](){ is_message_received(); });
  QObject::connect(&connection_checker, &QTimer::timeout, [&](){ connection_checker_timeout(); });

  for(unsigned i = 0; i + FIRST_PORT < LAST_PORT; ++i)
  {
    if(socket.bind(SERVER_IP, FIRST_PORT + i))
    {
      my_port = FIRST_PORT + i;
      log("bind: ", FIRST_PORT + i);
      break;
    }
    if(i + FIRST_PORT == LAST_PORT - 1)
     { i = 0; }
  }

  send(messages::HELLO_SERVER);
}

void client_t::impl_t::send(const QByteArray& message, bool is_prev_serial_need)
{
  if(!is_message_received())
  {
    send_message_stack.push_back(message);
    return;
  }

  log("send: ", message);

  socket.writeDatagram(add_serial_num(message, is_prev_serial_need), SERVER_IP, server_port);
  begin_wait_receive(message);
}

void client_t::impl_t::send(const messages::MESSAGE r_mes, bool is_prev_serial_need)
{
  QByteArray message;
  message.setNum(r_mes);

  if(r_mes != messages::MESSAGE_RECEIVED)
  {
    if(!is_message_received())
    {
      send_message_stack.push_back(message);
      return;
    }
    begin_wait_receive(message);
  }

  log("send: ", message);

  socket.writeDatagram(add_serial_num(message, is_prev_serial_need), SERVER_IP, server_port);
}

void client_t::impl_t::read()
{
  QHostAddress sender_IP;
  quint16 sender_port;

  QByteArray message;
  message.resize(socket.pendingDatagramSize());
  socket.readDatagram(message.data(), message.size(), &sender_IP, &sender_port);

  const int serial_num = cut_serial_num(message);

  log("read: ", message);
  if(sender_IP != SERVER_IP || sender_port != server_port || sender_port == my_port)
  {
    log("Warning! in read_data: Wrong sender!", message);
    return;
  }

  if(serial_num != ++received_serial_num)
  {
    --received_serial_num;
    if(serial_num == received_serial_num && message.toInt() != messages::MESSAGE_RECEIVED)
    {
      connection_checker.start(CHECK_CONNECT_TIME);
      send(messages::MESSAGE_RECEIVED, true);
    }
    log("Warning! in read_data: Wrong serial number!");
    return;
  }

  if(message.toInt() != messages::MESSAGE_RECEIVED)
  {
    send(messages::MESSAGE_RECEIVED);
    if(message.toInt() != messages::IS_CLIENT_LOST)
      { received_message_stack.push_back(message); }
  }
  else
  {
    if(last_send_message.mid(0, last_send_message.indexOf(FREE_SPASE)).toInt() == messages::HELLO_SERVER)
      { server_port = sender_port; }

    is_received = true;
    received_message_stack.push_back(QByteArray::number(messages::SERVER_HERE));
  }
  connection_checker.start(CHECK_CONNECT_TIME);
}

QByteArray client_t::impl_t::pull()
{
  QByteArray m(received_message_stack.first());
  received_message_stack.removeFirst();

  return m;
}

bool client_t::impl_t::is_message_append() const
{
  return !received_message_stack.isEmpty();
}

void client_t::impl_t::begin_wait_receive(const QByteArray& message)
{
  is_received = false;
  last_send_message = message;
  response_checker.start(RESPONSE_WAIT_TIME);
}

void client_t::impl_t::connection_checker_timeout()
{
  send(messages::IS_SERVER_LOST);
}

bool client_t::impl_t::is_message_received()
{
  static int num_of_restarts = 0;
  if(is_received)
  {
    response_checker.stop();
    num_of_restarts = 0;
  }
  else
  {
    response_checker.start(RESPONSE_WAIT_TIME);

    const QString t = last_send_message.mid(0, last_send_message.indexOf(FREE_SPASE));
    if(t.toInt() == messages::HELLO_SERVER)
    {
      ++server_port;
      if(server_port == LAST_PORT)
        { server_port = FIRST_PORT; }

      response_checker.start(RESPONSE_WAIT_TIME/10);
    }

    socket.writeDatagram(add_serial_num(last_send_message, true), SERVER_IP, server_port);

    if(last_send_message.toInt() == messages::IS_SERVER_LOST || num_of_restarts == 5)
      { received_message_stack.push_back(QByteArray::number(messages::SERVER_LOST)); }

    ++num_of_restarts;
  }
  return is_received;
}

QByteArray client_t::impl_t::add_serial_num(const QByteArray& data, bool is_prev_serial_need)
{
  if(!is_prev_serial_need)
    { ++send_serial_num; }

  QByteArray message;
  message.setNum(send_serial_num);
  message.append(FREE_SPASE);
  message.append(data);

  return message;
}

int client_t::impl_t::cut_serial_num(QByteArray& data) const
{
  QByteArray serial_num(data.mid(0, data.indexOf(FREE_SPASE)));
  data.remove(0, data.indexOf(FREE_SPASE) + 1);

  return serial_num.toInt();
}
