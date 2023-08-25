#pragma once
#include "common/helper.hpp"
#include "messages/messages.hpp"
#include "server/datagram.hpp"

#include <boost/asio.hpp>
#include <memory>
#include <string>
#include <vector>

namespace server {

class server_t
{
public:
  server_t(io_service_t& io_serv);
  server_t(const server_t&) = delete;
  server_t& operator=(const server_t&) = delete;
  server_t(server_t&&) = default;
  server_t& operator=(server_t&&) = default;
  ~server_t();

  template<msg::one_of_msg_types T>
  void send(T&& data, const endpoint_t& destination)
  {
    send(msg::prepare_for_send(std::forward<T>(data)), destination);
  }

  void process();
  std::vector<datagram_t> read();

private:
  void send(const std::string& message, const endpoint_t& destination);

  struct impl_t;
  std::unique_ptr<impl_t> impl_;
};

} // namespace server
