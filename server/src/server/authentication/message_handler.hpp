#pragma once

#include "common/helper.hpp"
#include "user/users_data_manager.hpp"

#include <functional>
#include <memory>
#include <string>

namespace chess::server::authentication {

class message_handler_t
{
public:
  using client_authenticated_callback_t = std::function<void(const endpoint_t& addr, client_uuid_t)>;
  using send_to_client_callback_t = std::function<void(const std::string& message, const endpoint_t& destination)>;

public:
  message_handler_t(user::users_data_manager_t& users_data_manager, const endpoint_t& logic_server_endpoint, const client_authenticated_callback_t& client_authenticated_callback,
                    const send_to_client_callback_t& send_to_client);
  message_handler_t(const message_handler_t&) = delete;
  message_handler_t(message_handler_t&&) = default;
  message_handler_t& operator=(const message_handler_t&) = delete;
  message_handler_t& operator=(message_handler_t&&) = default;
  ~message_handler_t();

  void handle(const endpoint_t& addr, const std::string& message);

private:
  struct impl_t;
  std::unique_ptr<impl_t> impl_;
};

} // namespace chess::server::authentication
