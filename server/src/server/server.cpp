#include "server/server.hpp"

namespace chess::server {

server_t::server_t(io_service_t& io_service, const client_connection_changed_callback_t& client_connection_changed, const client_authenticated_callback_t& client_authenticated)
  : logic_server_{io_service, client_connection_changed}
  , authentication_server_{io_service, logic_server_.address(), client_authenticated}
{}

void server_t::process()
{
  logic_server_.process();
}

std::vector<datagram_t<msg::some_datagram_t>> server_t::read()
{
  return logic_server_.read();
}

} // namespace chess::server