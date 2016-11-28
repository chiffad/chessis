#include "server_user.h"

#include <QVector>
#include <QTimer>


using namespace sr;

struct server_user_t::impl_t
{
  impl_t(const int port, const QHostAddress& address);
  bool is_me(const int _port, const QHostAddress& _ip) const;
  int get_port() const;
  QHostAddress get_ip() const;

  void push_for_send(const QByteArray& m);
  QByteArray pull_message_for_send();
  bool is_no_message_for_send() const;

  void push_received_mess(const QByteArray& m);
  bool is_no_received_mess() const;
  QByteArray pull_received_mess();

  bool is_previous_serial_num(const int num) const;
  bool is_current_serial_num(const int num) const;
  void increase_receive_serial_num();

  void receiving_timeout();
  void last_message_received();

  const int port;
  const QHostAddress ip;
  int send_serial_num = 0;
  int receiv_serial_num = 1;

  QVector<QByteArray> mess_for_send;
  QByteArray last_sent_mess;
  QVector<QByteArray> received_mess;

  bool is_last_mess_reach;
};

server_user_t::server_user_t(const int port, const QHostAddress& ip)
    : impl(new impl_t(port, ip))
{
}

server_user_t::~server_user_t()
{
}

bool server_user_t::is_me(const int port, const QHostAddress& ip) const
{
  return impl->is_me(port, ip);
}
int server_user_t::get_port() const
{
  return impl->get_port();
}

QHostAddress server_user_t::get_ip() const
{
  return impl->get_ip();
}

void server_user_t::push_for_send(const QByteArray& m)
{
  impl->push_for_send(m);
}

QByteArray server_user_t::pull_message_for_send()
{
  return impl->pull_message_for_send();
}

bool server_user_t::is_no_message_for_send() const
{
  return impl->is_no_message_for_send();
}

void server_user_t::push_received_mess(const QByteArray& m)
{
  impl->push_received_mess(m);
}

bool server_user_t::is_no_received_mess() const
{
  return impl->is_no_received_mess();
}

QByteArray server_user_t::pull_received_mess()
{
  return impl->pull_received_mess();
}

bool server_user_t::is_previous_serial_num(const int num) const
{
  return impl->is_previous_serial_num(num);
}

bool server_user_t::is_current_serial_num(const int num) const
{
  return impl->is_current_serial_num(num);
}

void server_user_t::increase_receive_serial_num()
{
  impl->increase_receive_serial_num();
}

void server_user_t::last_message_received()
{
  impl->last_message_received();
}

server_user_t::impl_t::impl_t(const int p, const QHostAddress& address)
    : port(p), ip(address)
{
}

bool server_user_t::impl_t::is_me(const int _port, const QHostAddress& _ip) const
{
  return (port == _port && ip == _ip);
}

int server_user_t::impl_t::get_port() const
{
  return port;
}

QHostAddress server_user_t::impl_t::get_ip() const
{
  return ip;
}

void server_user_t::impl_t::push_for_send(const QByteArray& m)
{
  mess_for_send.append(QByteArray::number(++send_serial_num) + " " + m);
}

QByteArray server_user_t::impl_t::pull_message_for_send()
{
  last_sent_mess = mess_for_send.first();
  mess_for_send.removeFirst();

  const int RECEIV_CHECK_TIME = 5000;
  QTimer::singleShot(RECEIV_CHECK_TIME, [&](){receiving_timeout();});

  return last_sent_mess;
}

void server_user_t::impl_t::push_received_mess(const QByteArray& m)
{
  received_mess.append(m);
}

bool server_user_t::impl_t::is_no_received_mess() const
{
  return received_mess.isEmpty();
}

QByteArray server_user_t::impl_t::pull_received_mess()
{
  const auto m = received_mess.first();
  received_mess.removeFirst();

  return m;
}

bool server_user_t::impl_t::is_no_message_for_send() const
{
  return mess_for_send.isEmpty() && is_last_mess_reach;
}

bool server_user_t::impl_t::is_previous_serial_num(const int num) const
{
  return receiv_serial_num -1 == num;
}

bool server_user_t::impl_t::is_current_serial_num(const int num) const
{
  return receiv_serial_num == num;
}

void server_user_t::impl_t::increase_receive_serial_num()
{
  ++receiv_serial_num;
}

void server_user_t::impl_t::receiving_timeout()
{
  if(is_last_mess_reach)
    { return; }

  mess_for_send.push_front(last_sent_mess);
}

void server_user_t::impl_t::last_message_received()
{
  is_last_mess_reach = true;
}
