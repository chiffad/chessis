#pragma once

#include <boost/asio.hpp>
#include <memory>
#include <string>
#include <vector>

namespace server {

class server_t
{
public:
  struct datagram_t
  {
    datagram_t(const boost::asio::ip::udp::endpoint& addr, const std::string& mess);
    boost::asio::ip::udp::endpoint address;
    std::string message;
  };

  server_t(boost::asio::io_service& io_serv);
  ~server_t();
  void send(const std::string& message, const boost::asio::ip::udp::endpoint& destination);
  std::vector<datagram_t> pull();

public:
  server_t(const server_t&) = delete;
  server_t& operator=(const server_t&) = delete;

private:
  struct impl_t;
  std::unique_ptr<impl_t> impl_;
};

} // namespace server
