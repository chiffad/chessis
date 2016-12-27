#ifndef __SERVER_H__BVGHVJHBDCWFDVAKYUVLWBALIGDVGSADW
#define __SERVER_H__BVGHVJHBDCWFDVAKYUVLWBALIGDVGSADW

#include <QByteArray>
#include <QHostAddress>
#include <vector>
#include <memory>

#include <boost/asio.hpp>
#include <string>

namespace sr
{

class server_t
{
public:

  struct datagram_t
  {
//    int port;
//    QHostAddress ip;
//    QByteArray message;
//    boost::asio::ip::udp::endpoint sender;
//    std::string message;
  };

  server_t(boost::asio::io_service& io_serv);
  ~server_t();
//  void send(const QByteArray& message, const int port, const QHostAddress& ip);
//  void send(const std::string& message, const boost::asio::ip::udp::endpoint& destination);
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