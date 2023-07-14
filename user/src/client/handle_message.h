#pragma once

#include <boost/mpl/begin_end.hpp>
#include <spdlog/spdlog.h>
#include <typeinfo>

#include "board_graphic.h"
#include "client/messages.h"
#include "helper.h"

namespace handler {

template<typename T>
struct type_2_type
{
  typedef T original_t;
};

template<typename T>
void process_mess(const std::string& str, graphic::board_graphic_t& bg, type_2_type<T>)
{
  if (msg::is_equal_types<typename T::type>(str))
  {
    process(bg, str, type_2_type<typename T::type>());
  }
  else
  {
    process_mess<typename boost::mpl::next<T>::type>(str, bg, type_2_type<typename boost::mpl::next<T>::type>());
  }
}

template<>
void process_mess(const std::string& /*str*/, graphic::board_graphic_t& /*bg*/, type_2_type<typename boost::mpl::end<msg::message_types>::type>)
{
  SPDLOG_ERROR("no type found!!");
}

#define handle(str, graphic)                                                                                                                                   \
  process_mess<boost::mpl::begin<msg::message_types>::type>(str, graphic, handler::type_2_type<typename boost::mpl::begin<msg::message_types>::type>());

template<typename struct_t>
void process(graphic::board_graphic_t& /*bg*/, const std::string& /*message*/, type_2_type<struct_t>)
{
  SPDLOG_ERROR("No tactic for process {}", typeid(struct_t).name());
}

template<>
void process(graphic::board_graphic_t& bg, const std::string& message, type_2_type<msg::inf_request_t>)
{
  SPDLOG_DEBUG("msg::inf_request_t");
  auto inf = msg::init<msg::inf_request_t>(message);

  bg.add_to_command_history(QString::fromStdString(inf.data));
}

template<>
void process(graphic::board_graphic_t& bg, const std::string& message, type_2_type<msg::game_inf_t>)
{
  SPDLOG_DEBUG("msg::game_inf_t");
  auto game_inf = msg::init<msg::game_inf_t>(message);

  bg.set_connect_status(msg::id<msg::server_here_t>());
  bg.set_board_mask(QString::fromStdString(game_inf.board_mask));
  bg.set_move_color(game_inf.move_num);
  bg.set_moves_history(QString::fromStdString(game_inf.moves_history));
  bg.update_hilight(game_inf.move_num, QString::fromStdString(game_inf.moves_history));
  bg.redraw_board();

  if (game_inf.is_mate)
  {
    bg.set_check_mate();
  }
}

template<>
void process(graphic::board_graphic_t& bg, const std::string& /*message*/, type_2_type<msg::get_login_t>)
{
  SPDLOG_DEBUG("msg::get_login_t");
  bg.get_login();
}

template<>
void process(graphic::board_graphic_t& bg, const std::string& /*message*/, type_2_type<msg::incorrect_log_t>)
{
  SPDLOG_DEBUG("msg::incorrect_log_t");
  bg.get_login("This login already exist. Enter another login!");
}

template<>
void process(graphic::board_graphic_t& bg, const std::string& /*message*/, type_2_type<msg::server_lost_t>)
{
  SPDLOG_DEBUG("type == msg::server_lost_t");
  bg.set_connect_status(msg::id<msg::server_lost_t>());
}

template<>
void process(graphic::board_graphic_t& bg, const std::string& /*message*/, type_2_type<msg::server_here_t>)
{
  SPDLOG_DEBUG("type == msg::server_here_t");
  bg.set_connect_status(msg::id<msg::server_here_t>());
}

template<>
void process(graphic::board_graphic_t& bg, const std::string& /*message*/, type_2_type<msg::opponent_lost_t>)
{
  SPDLOG_DEBUG("type == msg::opponent_lost_t");
  bg.set_connect_status(msg::id<msg::opponent_lost_t>());
}

} // namespace handler
