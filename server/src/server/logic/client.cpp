#include "server/logic/client.hpp"
#include <messages/messages_io.hpp>

#include <set>
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
try
{
  SPDLOG_TRACE("Client={}; addr={}; Received data={}", uuid(), e, m);
  set_address(e);

  if (!default_preprocess_and_is_futher_processing_needed(m)) return;
  if (m.data.type == msg::id_v<msg::is_server_lost_t>) return;
  if (m.data.type == msg::id_v<msg::hello_server_t>) return;

  SPDLOG_INFO("Client={}; Push msg={}; to logic!", uuid(), m.data);
  add_for_logic(std::move(m.data));
}
catch (const std::exception& ex)
{
  SPDLOG_CRITICAL("Failed to process mess=\"{}\"; from={}; ex={}!!!", m, e, ex.what());
  throw;
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
