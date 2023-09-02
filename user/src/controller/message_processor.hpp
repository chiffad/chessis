#pragma once

#include "controller/board.hpp"
#include "controller/login_input.hpp"
#include "controller/menu_layout.hpp"
#include <messages/messages.hpp>

#include <string>

namespace controller {

class message_processor_t
{
public:
  message_processor_t(menu_layout_t& menu_layout, board_t& board, login_input_t& login_input);

  void process_server_message(const std::string& server_message);
  void server_status_changed(bool server_online);

  void process(msg::inf_request_t info_request);
  void process(msg::game_inf_t game_info);
  void process(msg::get_login_t get_login);
  void process(msg::incorrect_log_t incorrect_log);
  void process(msg::opponent_lost_t opp_lst);

private:
  menu_layout_t& menu_layout_;
  board_t& board_;
  login_input_t& login_input_;
};

} // namespace controller
