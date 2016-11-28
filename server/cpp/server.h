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

public:
  void send(const QByteArray& message, const int port, const QHostAddress& ip);
  bool is_message_append() const;
  QByteArray pull();

public:
  server_t(const server_t&) = delete;
  server_t& operator=(const server_t&) = delete;

private:
  struct impl_t;
  std::unique_ptr<impl_t> impl;
};

} // namespace sr

#endif // __SERVER_H__BVGHVJHBDCWFDVAKYUVLWBALIGDVGSADW