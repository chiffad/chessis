#pragma once

#include "client/messages.h"
#include "controller/board_graphic.h"
#include "controller/login_input.hpp"

#include <string>

namespace controller {

class message_processor_t
{
public:
  message_processor_t(board_graphic_t& board, login_input_t& login_input);

  void process_server_message(const std::string& server_message);
  void process(msg::inf_request_t info_request);
  void process(msg::game_inf_t game_info);
  void process(msg::get_login_t get_login);
  void process(msg::incorrect_log_t incorrect_log);
  void process(msg::server_lost_t server_lost);
  void process(msg::server_here_t server_here);
  void process(msg::opponent_lost_t opp_lst);

private:
  board_graphic_t& board_;
  login_input_t& login_input_;
};

} // namespace controller
