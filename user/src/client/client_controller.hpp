#pragma once

#include "client/client.h"
#include <messages/messages.hpp>

namespace cl {

class client_controller_t
{
public:
  using message_received_callback_t = client_t::message_received_callback_t;
  using server_status_changed_callback_t = client_t::server_status_changed_callback_t;

public:
  client_controller_t(const message_received_callback_t& callback, const server_status_changed_callback_t& server_status_changed);
  void send(std::string message);

  template<msg::one_of_to_server_msgs Msg_t, typename... Args>
  std::string prepare_for_send(Args&&... args)
  {
    return ""; // msg::prepare_for_send(Msg_t{std::forward<Args>(args)..., /*TODO: add token*/});
  }

private:
  client_t client_;
};

} // namespace cl