#pragma once

#include "server/base_client.hpp"

namespace server {

class client_t : public base_client_t
{
public:
  using connection_status_signal_t = client_t::connection_status_signal_t;

public:
  client_t(io_service_t& io_serv, const endpoint_t& addr);
  client_t(const client_t&) = delete;
  client_t& operator=(const client_t&) = delete;
  client_t(client_t&&) = default;
  client_t& operator=(client_t&&) = default;
  ~client_t() override;

  void message_received(const std::string& message);
  void push_for_send(const std::string& message);

private:
  struct impl_t;
  std::unique_ptr<impl_t> impl_;
};

} // namespace server
