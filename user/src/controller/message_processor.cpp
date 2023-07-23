#include "controller/message_processor.hpp"

#include "client/handle_message.h"

namespace controller {

message_processor_t::message_processor_t(menu_layout_t& menu_layout, board_graphic_t& board, login_input_t& login_input)
  : menu_layout_{menu_layout}
  , board_{board}
  , login_input_{login_input}
{}

void message_processor_t::process_server_message(const std::string& server_message)
{
  handler::handle(server_message, *this);
}

void message_processor_t::process(const msg::inf_request_t info_request)
{
  SPDLOG_DEBUG("Process msg::inf_request_t");
  menu_layout_.add_to_command_history(QString::fromStdString(info_request.data));
}

void message_processor_t::process(const msg::game_inf_t game_info)
{
  SPDLOG_DEBUG("Process msg::game_inf_t");

  menu_layout_.set_connect_status(msg::id<msg::server_here_t>());
  menu_layout_.set_move_color(game_info.move_num);
  menu_layout_.set_moves_history(QString::fromStdString(game_info.moves_history));

  board_.set_board_mask(QString::fromStdString(game_info.board_mask));
  board_.update_hilight(game_info.move_num, QString::fromStdString(game_info.moves_history));
  board_.redraw_board();

  if (game_info.is_mate)
  {
    board_.set_check_mate();
  }
}

void message_processor_t::process(const msg::get_login_t /*get_login*/)
{
  SPDLOG_DEBUG("Process msg::get_login_t");
  login_input_.get_login();
}

void message_processor_t::process(const msg::incorrect_log_t /*incorrect_log*/)
{
  SPDLOG_DEBUG("Process msg::incorrect_log_t");
  login_input_.get_login("This login already exist. Enter another login!");
}

void message_processor_t::process(const msg::server_lost_t /*server_lost*/)
{
  SPDLOG_DEBUG("Process msg::server_lost_t");
  menu_layout_.set_connect_status(msg::id<msg::server_lost_t>());
}

void message_processor_t::process(const msg::server_here_t /*server_here*/)
{
  SPDLOG_DEBUG("Process msg::server_here_t");
  menu_layout_.set_connect_status(msg::id<msg::server_here_t>());
}

void message_processor_t::process(const msg::opponent_lost_t /*opp_lst*/)
{
  SPDLOG_DEBUG("Process msg::opponent_lost_t");
  menu_layout_.set_connect_status(msg::id<msg::opponent_lost_t>());
}

} // namespace controller