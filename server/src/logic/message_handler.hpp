#pragma once

#include "common/helper.hpp"
#include "logic/games_manager.hpp"
#include "server/server.hpp"
#include "user/user_status_monitor.hpp"
#include "user/users_data_manager.hpp"

#include <messages/messages.hpp>

#include <memory>
#include <string>

namespace chess::logic {

class message_handler_t
{
public:
  message_handler_t(games_manager_t&, server::server_t&, server::user::users_data_manager_t&, server::user::user_status_monitor_t&);
  message_handler_t(const message_handler_t&) = delete;
  message_handler_t(message_handler_t&&) = default;
  message_handler_t& operator=(const message_handler_t&) = delete;
  message_handler_t& operator=(message_handler_t&&) = default;
  ~message_handler_t();

  void process_server_message(const client_uuid_t& uuid, const msg::some_datagram_t& message);

private:
  struct impl_t;
  std::unique_ptr<impl_t> impl_;
};

} // namespace chess::logic
