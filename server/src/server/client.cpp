#include "server/client.hpp"
#include <spdlog/spdlog.h>

namespace server {

struct client_t::impl_t
{};

client_t::client_t(io_service_t& io_serv, const endpoint_t& addr)
  : base_client_t(io_serv, addr)
  , impl_(std::make_unique<impl_t>())
{}

client_t::~client_t() = default;

void client_t::message_received(const std::string& m)
{
  SPDLOG_INFO("Received data={}", m);
  const auto some_datagram_opt = preprocess_message(m);
  if (!some_datagram_opt) return;

  auto some_datagram = std::move(some_datagram_opt.value());
  if (some_datagram.type == msg::id_v<msg::is_server_lost_t>) return;
  if (some_datagram.type == msg::id_v<msg::hello_server_t>)
  {
    SPDLOG_INFO("Received hello_server_t msg from address={}", address());
    add_for_server(msg::prepare_for_send(msg::get_login_t()));
    return;
  }

  SPDLOG_TRACE("Push msg type={} to logic", some_datagram.type);
  add_for_logic(std::move(some_datagram));
}

} // namespace server
