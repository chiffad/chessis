#include "client.h"

#include <QObject>
#include <QTimer>

#include "messages.h"


using namespace inet;

struct client_t::impl_t
{
  impl_t(const int _port, const QHostAddress& _ip);
  void push(const QByteArray& message);
  QVector<QByteArray> pull_for_server();
  QVector<QByteArray> pull_for_logic();
  int get_port() const;
  QHostAddress get_ip() const;

  bool check_ser_num(QByteArray& m);
  void add_for_server(const QByteArray& message, bool is_prev_serial_need = false);
  void add_for_server(const Messages::MESSAGE r_mes, bool is_prev_serial_need = false);
  bool is_message_received();
  void connection_checker_timeout();
  void begin_wait_receive(const QByteArray& message);
  QByteArray add_serial_num(const QByteArray& data, bool is_prev_serial_need = false);
  int cut_serial_num(QByteArray& data) const;

  QTimer response_timer;
  QTimer connection_timer;

  QByteArray last_send_message;
  QVector<QByteArray> messages_for_server;
  QVector<QByteArray> messages_for_logic;

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

void client_t::push(const QByteArray& message)
{
  impl->push(message);
}

QVector<QByteArray> client_t::pull_for_server()
{
  return impl->pull_for_server();
}

QVector<QByteArray> client_t::pull_for_logic()
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
  enum { RESPONSE_WAIT_TIME = 1000, CHECK_CONNECT_TIME = 5000};
  response_timer.setInterval(RESPONSE_WAIT_TIME);
  connection_timer.setInterval(CHECK_CONNECT_TIME);

  QObject::connect(&response_timer, &QTimer::timeout, [&](){ is_message_received(); });
  QObject::connect(&connection_timer, &QTimer::timeout, [&](){ connection_timer_timeout(); });
}

void client_t::impl_t::push(const QByteArray& message)
{
  QByteArray m = message;

  qDebug()<<"read!!"<<message;

  const auto type = m.mid(0, m.indexOf(FREE_SPASE)).toInt();

  switch(type)
  {
    case Messages::MESSAGE_RECEIVED:
      is_received = true;
    default:
      add_for_server(Messages::MESSAGE_RECEIVED);
      messages_for_logic.append(m);
  }

  ++received_serial_num;
  connection_timer.start();
}

bool client_t::impl_t::check_ser_num(QByteArray& m)
{
  const int serial_num = cut_serial_num(m);

  if(serial_num == received_serial_num - 1 && m.toInt() != Messages::MESSAGE_RECEIVED)
  {
    connection_timer.start();
    add_for_server(Messages::MESSAGE_RECEIVED, true);
    return false;
  }

  if(serial_num != received_serial_num)
  {
    qDebug()<<"Warning! in UDP_socket::read_data: Wrong serial number!";
    return false;
  }

  return true;
}

void client_t::impl_t::add_for_server(const QByteArray& message, bool is_prev_serial_need)
{
  qDebug()<<"add_for_server:"<<message;

  is_prev_serial_need ? messages_for_server.push_front(add_serial_num(m, true))
                      : messages_for_server.append(add_serial_num(m));
}

void client_t::impl_t::add_for_server(const Messages::MESSAGE r_mes, bool is_prev_serial_need)
{
  QByteArray m;
  m.setNum(r_mes);

  qDebug()<<"add_for_server:"<<message;

  is_prev_serial_need ? messages_for_server.push_front(add_serial_num(m, true))
                      : messages_for_server.append(add_serial_num(m));
}

void client_t::impl_t::begin_wait_receive(const QByteArray& message)
{
  is_received = false;
  last_send_message = message;
  response_timer.start();
}

void client_t::impl_t::connection_checker_timer_timeout()
{
  add_for_server(Messages::IS_SERVER_LOST);
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
    response_timer.start();

    add_for_server(add_serial_num(last_send_message, true));

    if(last_send_message.toInt() == Messages::IS_CLIENT_LOST || num_of_restarts == 5)
      { messages_for_logic.push_back(QByteArray::number(Messages::CLIENT_LOST)); }

    ++num_of_restarts;
  }
  return is_received;
}

QByteArray client_t::impl_t::add_serial_num(const QByteArray &data, bool is_prev_serial_need)
{
  if(!is_prev_serial_need)
    { ++send_serial_num; }

  QByteArray message;
  message.setNum(send_serial_num);
  message.append(FREE_SPASE);
  message.append(data);

  return message;
}

int client_t::impl_t::cut_serial_num(QByteArray &data) const
{
  QByteArray serial_num(data.mid(0, data.indexOf(FREE_SPASE)));
  data.remove(0, data.indexOf(FREE_SPASE) + 1);

  return serial_num.toInt();
}
