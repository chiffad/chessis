#ifndef __SERVER_H__BVGHVJHBDCWFDVAKYUVLWBALIGDVGSADW
#define __SERVER_H__BVGHVJHBDCWFDVAKYUVLWBALIGDVGSADW

#include <QByteArray>
#include <QHostAddress>
#include <vector>
#include <memory>


namespace sr
{

class server_t
{
public:

  struct datagram_t
  {
    int port;
    QHostAddress ip;
    QByteArray message;
  };

  server_t();
  ~server_t();
  void send(const QByteArray& message, const int port, const QHostAddress& ip);
  std::vector<datagram_t> pull();

public:
  server_t(const server_t&) = delete;
  server_t& operator=(const server_t&) = delete;


private:
  struct impl_t;
  std::unique_ptr<impl_t> impl;
};

} // namespace sr

#endif // __SERVER_H__BVGHVJHBDCWFDVAKYUVLWBALIGDVGSADW