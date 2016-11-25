#include "server_user.h"

#include <QVector>

using namespace sr;

struct server_user::impl_t
{
  impl_t(const int port, const QHostAddress& ip);
  void push(const QByteArray& m);
  QByteArray pull();
  bool is_empty() const;

  const int port;
  const QHostAddress& ip;
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
