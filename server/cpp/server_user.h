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
  bool is_me(const int port, const QHostAddress& ip) const;

  void push_for_send(const QByteArray& m);
  bool is_no_message_for_send() const;
  QByteArray pull_message_for_send();

  void push_received_mess(const QByteArray& m);
  bool is_no_received_mess() const;
  QByteArray pull_received_mess();

  bool is_previous_serial_num(const int num) const;
  bool is_current_serial_num(const int num) const;
  void increase_receive_serial_num();

  void last_message_received();

private:
  struct impl_t;
  std::unique_ptr<impl_t> impl;
};

} // namespace sr

#endif //__SERVER_USER_H__BVWTQAYTGVKBLBWIDYFACWDVYW