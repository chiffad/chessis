#include "server/server.hpp"

namespace chess::server {

server_t::server_t(io_service_t& io_service, const client_connection_changed_callback_t& client_connection_changed, const client_authenticated_callback_t& client_authenticated)
  : logic_server_{io_service, client_connection_changed}
  , authentication_server_{io_service, logic_server_.address(), [client_authenticated, this](const endpoint_t& addr, chess::client_uuid_t uuid) {
                             logic_server_.add_client(uuid, addr);
                             client_authenticated(std::move(uuid));
                           }}
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