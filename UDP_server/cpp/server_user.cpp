#include "server_user.h"

#include <QVector>


using namespace sr;

struct server_user::impl_t
{
  impl_t(const int port, const QHostAddress& address);
  void push(const QByteArray& m);
  QByteArray pull();
  bool is_empty() const;

  const int port;
  const QHostAddress& ip;
  int send_serial_num = 0;

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
  QByteArray m = QByteArray::number(send_serial_num);
  m.append(" ");
  m.append(messages.first());

  messages.removeFirst();
  ++send_serial_num;

  return m;
}

bool server_user::impl_t::is_empty() const
{
  return messages.isEmpty();
}

