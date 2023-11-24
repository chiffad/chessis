#include "controller/message_processor.hpp"

namespace chess::controller {

message_processor_t::message_processor_t(menu_layout_t& menu_layout, board_t& board, login_input_t& login_input)
  : menu_layout_{menu_layout}
  , board_{board}
  , login_input_{login_input}
{}

void message_processor_t::server_status_changed(const bool server_online)
{
  SPDLOG_INFO("Server online={}", server_online);
  menu_layout_.set_connect_status(server_online ? menu_layout_t::connection_status_t::server_available : menu_layout_t::connection_status_t::server_lost);
}

void message_processor_t::process(const msg::inf_request_t info_request)
{
  SPDLOG_DEBUG("Process msg::inf_request_t");
  menu_layout_.add_to_command_history(QString::fromStdString(info_request.data));
}

void message_processor_t::process(const msg::game_inf_t game_info)
{
  SPDLOG_DEBUG("Process msg::game_inf_t");

  menu_layout_.set_connect_status(menu_layout_t::connection_status_t::server_available);
  menu_layout_.update_game_info(game_info.move_num, game_info.moves_history);
  board_.update_game_info(game_info);
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

void message_processor_t::process(const msg::opponent_lost_t /*opp_lst*/)
{
  menu_layout_.set_connect_status(menu_layout_t::connection_status_t::opponent_lost);
}

void message_processor_t::process(const msg::opponent_online_t /*opp_lst*/)
{
  menu_layout_.set_connect_status(menu_layout_t::connection_status_t::opponent_online);
}

} // namespace chess::controller
