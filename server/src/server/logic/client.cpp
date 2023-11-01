#include "server/logic/client.hpp"
#include <spdlog/spdlog.h>

namespace chess::server::logic {

struct client_t::impl_t
{
  impl_t(const client_uuid_t& uuid)
    : uuid_{uuid}
  {}
  const client_uuid_t uuid_;
};

client_t::client_t(io_service_t& io_serv, const client_uuid_t& uuid, const endpoint_t& addr)
  : base_client_t(io_serv, addr)
  , impl_(std::make_unique<impl_t>(uuid))
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
    return;
  }

  SPDLOG_TRACE("Push msg type={} to logic", some_datagram.type);
  add_for_logic(std::move(some_datagram));
}

void client_t::push_for_send(const std::string& message)
{
  add_for_send(message);
}

const client_uuid_t& client_t::uuid() const
{
  return impl_->uuid_;
}

} // namespace chess::server::logic
