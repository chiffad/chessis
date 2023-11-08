#pragma once

#include "common/helper.hpp"
#include "server/authentication/server.hpp"
#include "server/datagram.hpp"
#include "server/logic/server.hpp"
#include <messages/messages.hpp>

namespace chess::server {

class server_t
{
public:
  using client_connection_changed_callback_t = chess::server::logic::clients_holder_t::connection_status_signal_t::slot_type;
  using client_authenticated_callback_t = std::function<void(client_uuid_t)>;

public:
  server_t(io_service_t& io_service, const client_connection_changed_callback_t& client_connection_changed, const client_authenticated_callback_t& client_authenticated);
  void process();
  std::map<client_uuid_t, std::vector<msg::some_datagram_t>> read();

  template<msg::one_of_to_client_msgs T>
  void send(T&& data, const client_uuid_t& client_uuid)
  {
    logic_server_.send(msg::to_some_datagram(std::forward<T>(data)), client_uuid);
  }

private:
  logic::server_t logic_server_;
  authentication::server_t authentication_server_;
};

} // namespace chess::server
