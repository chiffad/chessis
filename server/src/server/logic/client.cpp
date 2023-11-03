#include "server/logic/client.hpp"
#include <messages/messages_io.hpp>
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

void client_t::message_received(const endpoint_t& e, msg::incoming_datagram_t m)
{
  SPDLOG_INFO("Client={}; Received data={}", uuid(), m);
  set_address(e);
  const auto some_datagram_opt = preprocess_message(m);
  if (!some_datagram_opt) return;

  auto some_datagram = std::move(some_datagram_opt.value());
  if (some_datagram.type == msg::id_v<msg::is_server_lost_t>) return;
  if (some_datagram.type == msg::id_v<msg::hello_server_t>)
  {
    SPDLOG_INFO("Received hello_server_t msg from client={}, address={}", uuid(), address());
    return;
  }

  SPDLOG_TRACE("Client={}; Push msg type={} to logic", uuid(), some_datagram.type);
  add_for_logic(std::move(some_datagram));
}

void client_t::push_for_send(const msg::some_datagram_t& message)
{
  add_for_send(message);
}

const client_uuid_t& client_t::uuid() const
{
  return impl_->uuid_;
}

} // namespace chess::server::logic
