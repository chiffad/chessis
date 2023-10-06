#include "client/client_controller.hpp"

namespace cl {

namespace {
std::unique_ptr<cl::connection_strategy_t> build_search_port_strategy()
{
  return std::make_unique<cl::search_port_t>(cl::endpoint_t{"127.0.0.1", cl::login_server::FIRST_PORT}, cl::login_server::FIRST_PORT, cl::login_server::LAST_PORT);
}

std::unique_ptr<cl::connection_strategy_t> build_connect_port_strategy(std::string address, uint16_t port)
{
  return std::make_unique<cl::connect_port_t>(cl::endpoint_t{std::move(address), port});
}

} // namespace

client_controller_t::client_controller_t(const message_received_callback_t& callback, const server_status_changed_callback_t& server_status_changed)
  : message_received_callback_{callback}
  , server_status_changed_callback_{server_status_changed}
  , client_{std::make_unique<client_t>(message_received_callback_, server_status_changed_callback_, build_search_port_strategy())}
{}

void client_controller_t::send(std::string message)
{
  client_->send(std::move(message));
}

void client_controller_t::process(msg::login_response_t data)
{
  SPDLOG_INFO("New login info received: token={}, logic_server={}:{}", data.token.uuid, data.logic_server_address, data.logic_server_port);
  token_ = std::make_unique<msg::token_t>(std::move(data.token));
  client_ = std::make_unique<client_t>(message_received_callback_, server_status_changed_callback_,
                                       build_connect_port_strategy(std::move(data.logic_server_address), data.logic_server_port));
}

} // namespace cl
