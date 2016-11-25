#ifndef __SERVER_USER_H__BVWTQAYTGVKBLBWIDYFACWDVYW
#define __SERVER_USER_H__BVWTQAYTGVKBLBWIDYFACWDVYW

#include <memory>
#include <QByteArray>
#include <QHostAddress>


namespace sr
{

class server_user
{
public:
  server_user(const int port, const QHostAddress& ip);
  ~server_user();
  void push(const QByteArray& m);
  QByteArray pull();
  bool is_empty() const;

private:
  struct impl_t;
  std::unique_ptr<impl_t> impl;
};

}

#endif //__SERVER_USER_H__BVWTQAYTGVKBLBWIDYFACWDVYW