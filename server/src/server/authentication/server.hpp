#pragma once

#include "common/helper.hpp"
#include "common/unique_id_generator.hpp"
#include "server/authentication/message_handler.hpp"

#include <functional>
#include <memory>

namespace chess::server::authentication {

class server_t
{
public:
  using client_authenticated_callback_t = message_handler_t::client_authenticated_callback_t;

public:
  server_t(io_service_t& io_serv, const endpoint_t& authenticated_clients_server_endpoint, const client_authenticated_callback_t& callback);
  server_t(const server_t&) = delete;
  server_t& operator=(const server_t&) = delete;
  server_t(server_t&&) = default;
  server_t& operator=(server_t&&) = default;
  ~server_t();

private:
  struct impl_t;
  std::unique_ptr<impl_t> impl_;
};

} // namespace chess::server::authentication
