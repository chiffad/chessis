#pragma once

#include "client/client.h"
#include <messages/messages.hpp>
#include <spdlog/spdlog.h>

namespace cl {

template<typename T>
concept tokenized_msg_to_server = msg::tokenized_msg<T> && msg::one_of_to_server_msgs<T>;

class client_controller_t
{
public:
  using message_received_callback_t = client_t::message_received_callback_t;
  using server_status_changed_callback_t = client_t::server_status_changed_callback_t;

public:
  client_controller_t(const message_received_callback_t& callback, const server_status_changed_callback_t& server_status_changed);
  void send(std::string message);
  void process(msg::login_response_t login_response);

  template<tokenized_msg_to_server Msg_t, typename... Args>
  std::string prepare_for_send(Args&&... args) const
  {
    if (!token_)
    {
      SPDLOG_CRITICAL("there is no token!");
      throw std::logic_error("prepare_for_send with token requested, but there is no token!");
    }
    return msg::prepare_for_send(Msg_t{std::forward<Args>(args)..., *token_});
  }

  template<msg::one_of_to_server_msgs Msg_t, typename... Args>
  std::string prepare_for_send(Args&&... args) const
  {
    return msg::prepare_for_send(Msg_t{std::forward<Args>(args)...});
  }

private:
  const message_received_callback_t message_received_callback_;
  const server_status_changed_callback_t server_status_changed_callback_;
  std::unique_ptr<client_t> client_;
  std::unique_ptr<msg::token_t> token_;
};

} // namespace cl
