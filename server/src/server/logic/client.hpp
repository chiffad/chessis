#pragma once

#include "server/logic/base_client.hpp"

namespace chess::server::logic {

class client_t : public base_client_t
{
public:
  using connection_status_signal_t = client_t::connection_status_signal_t;

public:
  client_t(io_service_t& io_serv, const client_uuid_t& uuid, const endpoint_t& addr);
  client_t(const client_t&) = delete;
  client_t& operator=(const client_t&) = delete;
  client_t(client_t&&) = default;
  client_t& operator=(client_t&&) = default;
  ~client_t() override;

  void message_received(const endpoint_t& e, msg::incoming_datagram_t message);
  void push_for_send(const msg::some_datagram_t& message);
  const client_uuid_t& uuid() const;

private:
  struct impl_t;
  std::unique_ptr<impl_t> impl_;
};

} // namespace chess::server::logic
