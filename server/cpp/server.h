#ifndef __SERVER_H__BVGHVJHBDCWFDVAKYUVLWBALIGDVGSADW
#define __SERVER_H__BVGHVJHBDCWFDVAKYUVLWBALIGDVGSADW

#include <QByteArray>
#include <QHostAddress>
#include <QUdpSocket>
#include <memory>


namespace sr
{

class server_t
{

public:
  server_t();
  ~server_t();
  void process_event();
  void push(const QByteArray& message, const int port, const QHostAddress& ip);
  bool is_message_append(const int port, const QHostAddress& ip) const;
  QByteArray pull(const int port, const QHostAddress& ip);

public:
  server_t(const server_t&) = delete;
  server_t& operator=(const server_t&) = delete;

private:
  struct impl_t;
  std::unique_ptr<impl_t> impl;
};

} // namespace sr

#endif // __SERVER_H__BVGHVJHBDCWFDVAKYUVLWBALIGDVGSADW