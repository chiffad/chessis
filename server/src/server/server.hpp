#pragma once
#include "common/helper.hpp"

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
    datagram_t(const endpoint_t& addr, const std::string& mess);
    endpoint_t address;
    std::string message;
  };

  server_t(io_service_t& io_serv);
  ~server_t();
  void send(const std::string& message, const endpoint_t& destination);
  std::vector<datagram_t> pull();

public:
  server_t(const server_t&) = delete;
  server_t& operator=(const server_t&) = delete;

private:
  struct impl_t;
  std::unique_ptr<impl_t> impl_;
};

} // namespace server
