#pragma once
#include "common/helper.hpp"
#include "messages/messages.hpp"
#include "server/datagram.hpp"
#include "server/logic/clients_holder.hpp"

#include <boost/asio.hpp>
#include <memory>
#include <string>
#include <vector>

namespace chess::server::logic {

class server_t
{
public:
  server_t(io_service_t& io_serv, const clients_holder_t::connection_status_signal_t::slot_type& subscriber);
  server_t(const server_t&) = delete;
  server_t& operator=(const server_t&) = delete;
  server_t(server_t&&) = default;
  server_t& operator=(server_t&&) = default;
  ~server_t();

  void send(const std::string& message, const endpoint_t& destination);
  void process();
  std::vector<datagram_t<msg::some_datagram_t>> read();
  endpoint_t address() const;

private:
  struct impl_t;
  std::unique_ptr<impl_t> impl_;
};

} // namespace chess::server::logic
