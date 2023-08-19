#pragma once

#include "common/helper.hpp"
#include "logic/boards_holder.hpp"
#include "logic/desk.hpp"
#include "messages/messages.hpp"
#include "server/clients_holder.hpp"
#include <spdlog/spdlog.h>

#include <boost/asio.hpp>
#include <boost/mpl/begin_end.hpp>
#include <memory>
#include <string>
#include <typeinfo>

namespace server {
class handle_message_t
{
public:
  explicit handle_message_t(clients_holder_t&, logic::boards_holder_t&);

#define handle(str, client) process_mess<boost::mpl::begin<msg::message_types>::type>(str, client);
  void new_message(io_service_t& io_service, const endpoint_t& addr, const std::string& message);
  clients_arr_t::iterator begin() noexcept;
  clients_arr_t::iterator end() noexcept;

  template<typename T>
  void process_mess(const std::string& str, std::shared_ptr<client_t>& client)
  {
    if (msg::is_equal_types<typename T::type>(str))
    {
      handle_fn<typename T::type>(str, client);
    }
    else
    {
      process_mess<typename boost::mpl::next<T>::type>(str, client);
    }
  }

public:
  handle_message_t(const handle_message_t&) = delete;
  handle_message_t& operator=(const handle_message_t&) = delete;

private:
  desks_arr_t::iterator get_desk(const std::shared_ptr<client_t>& client);
  clients_arr_t::iterator get_opponent(const std::shared_ptr<client_t>& client);
  void board_updated(std::shared_ptr<client_t>& client);
  void start_new_game(std::shared_ptr<client_t>& client);

  template<typename T>
  void handle_fn(const std::string& str, std::shared_ptr<client_t>& /*client*/)
  {
    SPDLOG_ERROR("For type={} tactic isn't defined! msg={}", typeid(T).name(), str);
  }

  clients_holder_t& clients_;
  logic::boards_holder_t& desks_;
};

template<>
void handle_message_t::process_mess<boost::mpl::end<msg::message_types>::type>(const std::string& /*str*/, std::shared_ptr<client_t>& /*client*/);

#define handle_macro(struct_type)                                                                                                                              \
  template<>                                                                                                                                                   \
  void handle_message_t::handle_fn<struct_type>(const std::string& str, std::shared_ptr<client_t>& client);
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
