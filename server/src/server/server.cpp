#include "server/server.hpp"

namespace chess::server {

server_t::server_t(io_service_t& io_service, user::users_data_manager_t& users_data_manager, const client_connection_changed_callback_t& client_connection_changed)
  : logic_server_{io_service, client_connection_changed}
  , authentication_server_{io_service, users_data_manager, logic_server_.address(), [this](const auto& addr, auto uuid) { logic_server_.add_client(std::move(uuid), addr); }}
{}

void server_t::process()
{
  logic_server_.process();
}

std::map<client_uuid_t, std::vector<msg::some_datagram_t>> server_t::read()
{
  return logic_server_.read();
}

} // namespace chess::server