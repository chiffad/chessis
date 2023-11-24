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
  server_t(io_service_t& io_serv, user::user_status_monitor_t& user_status_monitor);
  server_t(const server_t&) = delete;
  server_t& operator=(const server_t&) = delete;
  server_t(server_t&&) = default;
  server_t& operator=(server_t&&) = default;
  ~server_t();

  void add_client(const client_uuid_t& uuid, const endpoint_t& addr);
  void send(const msg::some_datagram_t& message, const client_uuid_t& client_uuid);
  void process();
  std::map<client_uuid_t, std::vector<msg::some_datagram_t>> read();
  endpoint_t address() const;

private:
  struct impl_t;
  std::unique_ptr<impl_t> impl_;
};

} // namespace chess::server::logic
