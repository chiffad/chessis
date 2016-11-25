#include "server_user.h"

#include <QVector>


using namespace sr;

struct server_user::impl_t
{
  impl_t(const int port, const QHostAddress& address);
  void push(const QByteArray& m);
  QByteArray pull();
  bool is_empty() const;

  bool is_previous_serial_num(const int num) const;
  bool is_current_serial_num(const int num) const;
  void add_receive_serial_num();

  const int port;
  const QHostAddress& ip;
  int send_serial_num = 0;
  int receiv_serial_num = 1;

  QVector<QByteArray> messages;
};

server_user::server_user(const int port, const QHostAddress& ip)
    : impl(new impl_t(port, ip))
{
}

server_user::~server_user()
{
}

void server_user::push(const QByteArray& m)
{
  impl->push(m);
}

QByteArray server_user::pull()
{
  return impl->pull();
}

bool server_user::is_empty() const
{
  return impl->is_empty();
}

bool server_user::is_previous_serial_num(const int num) const
{
  return impl->is_previous_serial_num(num);
}

bool server_user::is_current_serial_num(const int num) const
{
  return impl->is_current_serial_num(num);
}

void server_user::add_receive_serial_num()
{
  impl->add_receive_serial_num();
}

server_user::impl_t::impl_t(const int p, const QHostAddress& address)
    : port(), ip(address)
{
}

void server_user::impl_t::push(const QByteArray& m)
{
  messages.append(m);
}

QByteArray server_user::impl_t::pull()
{
  QByteArray m = QByteArray::number(++send_serial_num);
  m.append(" ");
  m.append(messages.first());

  messages.removeFirst();

  return m;
}

bool server_user::impl_t::is_empty() const
{
  return messages.isEmpty();
}

bool server_user::impl_t::is_previous_serial_num(const int num) const
{
  return receiv_serial_num -1 == num;
}

bool server_user::impl_t::is_current_serial_num(const int num) const
{
  return receiv_serial_num == num;
}

void server_user::impl_t::add_receive_serial_num()
{
  ++receiv_serial_num;
}


