#include "client/client_controller.hpp"

namespace cl {

namespace {
std::unique_ptr<cl::connection_strategy_t> build_search_port_strategy()
{
  return std::make_unique<cl::search_port_t>(cl::endpoint_t{"127.0.0.1", cl::login_server::FIRST_PORT}, cl::login_server::FIRST_PORT,
                                             cl::login_server::LAST_PORT);
}

} // namespace

client_controller_t::client_controller_t(const message_received_callback_t& callback, const server_status_changed_callback_t& server_status_changed)
  : client_{callback, server_status_changed, build_search_port_strategy()}
{}

void client_controller_t::send(std::string message)
{
  client_.send(std::move(message));
}

} // namespace cl