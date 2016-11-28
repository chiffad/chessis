#ifndef __SERVER_USER_H__BVWTQAYTGVKBLBWIDYFACWDVYW
#define __SERVER_USER_H__BVWTQAYTGVKBLBWIDYFACWDVYW

#include <memory>
#include <QByteArray>
#include <QHostAddress>


namespace sr
{

class server_user_t
{
public:
  server_user_t(const int port, const QHostAddress& ip);
  ~server_user_t();
  void push(const QByteArray& m);
  QByteArray pull();
  bool is_empty() const;

  bool is_previous_serial_num(const int num) const;
  bool is_current_serial_num(const int num) const;
  void add_receive_serial_num();

private:
  struct impl_t;
  std::unique_ptr<impl_t> impl;
};

}

#endif //__SERVER_USER_H__BVWTQAYTGVKBLBWIDYFACWDVYW