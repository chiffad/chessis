#include "client.h"

#include <QObject>
#include <QTimer>
#include <QVector>

#include "messages.h"
#include "log.h"


using namespace sr;

struct client_t::impl_t
{
  impl_t(const int _port, const QHostAddress& _ip);
  void push_from_server(QByteArray message);
  void push_for_send(const QByteArray& message);
  bool is_message_for_server_append() const;
  bool is_message_for_logic_append() const;
  QByteArray pull_for_server();
  QByteArray pull_for_logic();
  int get_port() const;
  QHostAddress get_ip() const;

  bool check_ser_num(QByteArray& m);
  void add_for_server(const QByteArray& message, bool is_extra_message = false);
  void add_for_server(const messages::MESSAGE r_mes, bool is_extra_message = false);
  bool is_message_received();
  void connection_timer_timeout();
  void begin_wait_receive(const QByteArray& message);
  QByteArray add_serial_num(const QByteArray& data, const int num) const;
  int cut_serial_num(QByteArray& data) const;

  struct server_mess_t
  {
    server_mess_t(const QByteArray& m, const bool is_extra);
    server_mess_t() = default;
    QByteArray message;
    bool is_extra;
  };

  QVector<server_mess_t> messages_for_server;
  QVector<QByteArray> messages_for_logic;
  QByteArray last_send_message;

  QTimer response_timer;
  QTimer connection_timer;

  int received_serial_num;
  int send_serial_num;
  bool is_received;

  int port;
  QHostAddress ip;
  QByteArray login;

  const QChar FREE_SPASE = ' ';
};


client_t::client_t(const int port, const QHostAddress& ip)
    : impl(new impl_t(port, ip))
{
}

client_t::~client_t()
{
}

void client_t::push_from_server(const QByteArray& message)
{
  impl->push_from_server(message);
}

void client_t::push_for_send(const QByteArray& message)
{
  impl->push_for_send(message);
}

bool client_t::is_message_for_server_append() const
{
  return impl->is_message_for_server_append();
}

bool client_t::is_message_for_logic_append() const
{
  return impl->is_message_for_logic_append();
}

QByteArray client_t::pull_for_server()
{
  return impl->pull_for_server();
}

QByteArray client_t::pull_for_logic()
{
  return impl->pull_for_logic();
}

int client_t::get_port() const
{
  return impl->get_port();
}

QHostAddress client_t::get_ip() const
{
  return impl->get_ip();
}

client_t::impl_t::impl_t(const int _port, const QHostAddress& _ip)
    : received_serial_num(1), send_serial_num(0), is_received(true), port(_port), ip(_ip)
{
  enum { RESPONSE_WAIT_TIME = 1500, CHECK_CONNECT_TIME = 7000};
  response_timer.setInterval(RESPONSE_WAIT_TIME);
  connection_timer.setInterval(CHECK_CONNECT_TIME);

  QObject::connect(&response_timer, &QTimer::timeout, [&](){ is_message_received(); });
  QObject::connect(&connection_timer, &QTimer::timeout, [&](){ connection_timer_timeout(); });
}

void client_t::impl_t::push_from_server(QByteArray m)
{
  qDebug()<<"!client_t::impl_t::push"<<m;

  if(!check_ser_num(m))
    { return; }

  ++received_serial_num;
  connection_timer.start();

  const auto type = m.mid(0, m.indexOf(FREE_SPASE)).toInt();

  switch(type)
  {
    case messages::MESSAGE_RECEIVED:
      is_received = true;
      break;
    case messages::IS_SERVER_LOST:
      add_for_server(messages::MESSAGE_RECEIVED);
      break;
    default:
      add_for_server(messages::MESSAGE_RECEIVED);
      messages_for_logic.append(m);
  }
}

void client_t::impl_t::push_for_send(const QByteArray& m)
{
  messages_for_server.append(server_mess_t(m, false));
}

bool client_t::impl_t::is_message_for_server_append() const
{
  return (!messages_for_server.isEmpty() && (is_received || (messages_for_server.first().is_extra && messages_for_server.first().message == last_send_message)));
}

bool client_t::impl_t::is_message_for_logic_append() const
{
  return !messages_for_logic.isEmpty();
}

QByteArray client_t::impl_t::pull_for_server()
{
  qDebug()<<"client_t::impl_t::pull_for_server()";
//  if(!is_message_for_server_append())
//    { throw_exception("messages_for_server.isEmpty()"); }

  const auto& _1 = messages_for_server.first();
  const QByteArray m = add_serial_num(_1.message, _1.is_extra ? send_serial_num : ++send_serial_num);

  if(_1.message.toInt() != messages::MESSAGE_RECEIVED)
    { begin_wait_receive(_1.message); }

  messages_for_server.removeFirst();

  return m;
}

QByteArray client_t::impl_t::pull_for_logic()
{
//  if(!is_message_for_logic_append())
//    { throw_exception("messages_for_logic.isEmpty()"); }
  qDebug()<<"client_t::impl_t::pull_for_logic()";

  const QByteArray m = messages_for_logic.first();
  messages_for_logic.removeFirst();

  return m;
}

int client_t::impl_t::get_port() const
{
  return port;
}

QHostAddress client_t::impl_t::get_ip() const
{
  return ip;
}

bool client_t::impl_t::check_ser_num(QByteArray& m)
{
  const int serial_num = cut_serial_num(m);

  if(serial_num == received_serial_num - 1 && m.toInt() != messages::MESSAGE_RECEIVED)
  {
    connection_timer.start();
    add_for_server(messages::MESSAGE_RECEIVED, true);
    return false;
  }

  if(serial_num != received_serial_num)
  {
    qDebug()<<"Warning! in UDP_socket::read_data: Wrong serial number!";
    return false;
  }

  return true;
}

void client_t::impl_t::add_for_server(const QByteArray& m, bool is_extra_message)
{
  qDebug()<<"add_for_server:"<<m;
  auto _1 = server_mess_t(m, is_extra_message);
  is_extra_message ? messages_for_server.push_front(_1)
                   : messages_for_server.append(_1);
}

void client_t::impl_t::add_for_server(const messages::MESSAGE r_mes, bool is_extra_message)
{
  qDebug()<<"add_for_server messages::MESSAGE:"<<r_mes;
  auto _1 = server_mess_t(QByteArray::number(r_mes), is_extra_message);
  is_extra_message ? messages_for_server.push_front(_1)
                   : messages_for_server.append(_1);
}

void client_t::impl_t::begin_wait_receive(const QByteArray& message)
{
  is_received = false;
  last_send_message = message;
  response_timer.start();
}

bool client_t::impl_t::is_message_received()
{
  static int num_of_restarts = 0;
  if(is_received)
  {
    response_timer.stop();
    num_of_restarts = 0;
  }
  else
  {
    add_for_server(last_send_message, true);

    if(last_send_message.toInt() == messages::IS_CLIENT_LOST || num_of_restarts == 3)
      { messages_for_logic.push_back(QByteArray::number(messages::CLIENT_LOST)); }

    ++num_of_restarts;
    response_timer.start();
  }
  return is_received;
}

void client_t::impl_t::connection_timer_timeout()
{
  add_for_server(messages::IS_CLIENT_LOST);
  connection_timer.stop();
}

QByteArray client_t::impl_t::add_serial_num(const QByteArray& data, const int num) const
{
  QByteArray message;
  message.setNum(num);
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

client_t::impl_t::server_mess_t::server_mess_t(const QByteArray& m, const bool is_extra_message)
    : message(m), is_extra(is_extra_message)
{
}