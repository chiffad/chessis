#ifndef __CLIENT_H__GOVLIYVFTYUVFTYFDIKVGLUIHL
#define __CLIENT_H__GOVLIYVFTYUVFTYFDIKVGLUIHL

#include <QByteArray>
#include <QHostAddress>
#include <memory>


namespace sr
{

class client_t
{
public:
  client_t(const int port, const QHostAddress& ip);
  ~client_t();
  void push_from_server(const QByteArray& message);
  void push_for_send(const QByteArray& message);
  bool is_message_for_server_append() const;
  bool is_message_for_logic_append() const;
  QByteArray pull_for_server();
  QByteArray pull_for_logic();
  int get_port() const;
  QHostAddress get_ip() const;

  void set_login(const QByteArray& log);
  QByteArray get_login() const;
  void set_rating(const int rating);
  int get_rating() const;

public:
  client_t(const client_t&) = delete;
  client_t& operator=(const client_t&) = delete;

private:
  struct impl_t;
  std::unique_ptr<impl_t> impl;
};

} // namespace inet

#endif // __CLIENT_H__GOVLIYVFTYUVFTYFDIKVGLUIHL

