#pragma once

#include "common/helper.hpp"
#include "logic/games_manager.hpp"
#include "messages/messages.hpp"
#include "server/server.hpp"

#include <spdlog/spdlog.h>

#include <boost/mpl/begin_end.hpp>
#include <memory>
#include <string>
#include <typeinfo>

namespace server {
class handle_message_t
{
public:
  explicit handle_message_t(logic::games_manager_t&, server_t&);

  void process_server_message(const endpoint_t& addr, const std::string& message);

  // TODO: not needed as public
#define handle(str, player) process_mess<boost::mpl::begin<msg::message_types>::type>(str, player);

  template<typename T>
  void process_mess(const std::string& str, logic::player_t& player)
  {
    if (msg::is_equal_types<typename T::type>(str))
    {
      handle_fn<typename T::type>(str, player);
    }
    else
    {
      process_mess<typename boost::mpl::next<T>::type>(str, player);
    }
  }

public:
  handle_message_t(const handle_message_t&) = delete;
  handle_message_t& operator=(const handle_message_t&) = delete;
  handle_message_t(handle_message_t&&) = default;
  handle_message_t& operator=(handle_message_t&&) = default;

private:
  void board_updated(logic::player_t& player);
  void start_new_game(logic::player_t& player);

  template<typename T>
  void handle_fn(const std::string& str, logic::player_t& /*player*/)
  {
    SPDLOG_ERROR("For type={} tactic isn't defined! msg={}", typeid(T).name(), str);
  }

  logic::games_manager_t& games_manager_;
  server_t& server_;
};

template<>
void handle_message_t::process_mess<boost::mpl::end<msg::message_types>::type>(const std::string& /*str*/, logic::player_t& /*player*/);

#define handle_macro(struct_type)                                                                                                                              \
  template<>                                                                                                                                                   \
  void handle_message_t::handle_fn<struct_type>(const std::string& str, logic::player_t& player);
handle_macro(msg::login_t);
handle_macro(msg::opponent_inf_t);
handle_macro(msg::my_inf_t);
handle_macro(msg::client_lost_t);
handle_macro(msg::move_t);
handle_macro(msg::back_move_t);
handle_macro(msg::go_to_history_t);
handle_macro(msg::new_game_t);
#undef handle_macro

} // namespace server
