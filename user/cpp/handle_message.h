#include "messages.h"
#include "board_graphic.h"
#include "helper.h"

namespace handle
{

template<typename T>
struct type_2_type
{ typedef T original_t; };

template<typename struct_t>
void process(graphic::board_graphic_t& /*bg*/, const std::string& /*message*/, type_2_type<struct_t>)
{ cl::helper::log("handle::process: No tactic for process ", msg::id<struct_t>()); }

template<>
void process(graphic::board_graphic_t& bg, const std::string& message, type_2_type<msg::inf_request_t>)
{
  cl::helper::log("msg::inf_request_t");
  auto inf = msg::init<msg::inf_request_t>(message);

  bg.add_to_command_history(QString::fromStdString(inf.data));
}

template<>
void process(graphic::board_graphic_t& bg, const std::string& message, type_2_type<msg::game_inf_t>)
{
  cl::helper::log("msg::game_inf_t");
  auto game_inf = msg::init<msg::game_inf_t>(message);

  bg.set_connected_status();
  bg.set_board_mask(QString::fromStdString(game_inf.board_mask));
  bg.set_move_color(game_inf.move_num);
  bg.set_moves_history(QString::fromStdString(game_inf.moves_history));
  bg.update_hilight(game_inf.move_num, QString::fromStdString(game_inf.moves_history));
  bg.redraw_board();

  if(game_inf.is_mate)
    { bg.set_check_mate(); }
}

template<>
void process(graphic::board_graphic_t& bg, const std::string& /*message*/, type_2_type<msg::get_login_t>)
{
  cl::helper::log("msg::get_login_t");
  bg.get_login();
}

template<>
void process(graphic::board_graphic_t& bg, const std::string& /*message*/, type_2_type<msg::incorrect_log_t>)
{
  cl::helper::log("msg::incorrect_log_t");
  bg.get_login("This login already exist. Enter another login!");
}

template<>
void process(graphic::board_graphic_t& bg, const std::string& /*message*/, type_2_type<msg::server_lost_t>)
{
  cl::helper::log("type == msg::server_lost_t");
  bg.set_disconnected_status();
}

template<>
void process(graphic::board_graphic_t& bg, const std::string& /*message*/, type_2_type<msg::server_here_t>)
{
  cl::helper::log("type == msg::server_here_t");
  bg.set_connected_status();
}

template<>
void process(graphic::board_graphic_t& bg, const std::string& /*message*/, type_2_type<msg::opponent_lost_t>)
{
  cl::helper::log("type == msg::opponent_lost_t");
  bg.set_opponent_disconnected_status();
}

} // namespace handle
