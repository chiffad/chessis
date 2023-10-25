#pragma once

#include "client/client.h"
#include <memory>
#include <messages/messages.hpp>
#include <string>

namespace chess::cl {

class client_controller_t
{
public:
  using message_received_callback_t = client_t::message_received_callback_t;
  using server_status_changed_callback_t = client_t::server_status_changed_callback_t;

public:
  client_controller_t(const message_received_callback_t& callback, const server_status_changed_callback_t& server_status_changed);
  ~client_controller_t();

  void send_login(std::string login, std::string pwd);
  void send_move(std::string data);
  void send_go_to_history(uint16_t history_i);
  void send_new_game();
  void send_back_move();
  void send_opponent_inf();
  void send_my_inf();

  void process(msg::login_response_t login_response);

private:
  struct impl_t;
  std::unique_ptr<impl_t> impl_;
};

} // namespace chess::cl
