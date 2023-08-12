#pragma once

#include <boost/asio.hpp>
#include <boost/mpl/begin_end.hpp>
#include <memory>
#include <string>
#include <typeinfo>
#include <vector>

#include "client.h"
#include "desk.h"
#include "helper.h"
#include "messages/messages.hpp"
#include <spdlog/spdlog.h>

namespace server {
class handle_message_t
{
public:
  handle_message_t() = default;
#define handle(str, client) process_mess<boost::mpl::begin<msg::message_types>::type>(str, client);
  void new_message(boost::asio::io_service& io_service, const boost::asio::ip::udp::endpoint& addr, const std::string& message);
  std::vector<std::shared_ptr<server::client_t>>::iterator begin() noexcept;
  std::vector<std::shared_ptr<server::client_t>>::iterator end() noexcept;

  template<typename T>
  void process_mess(const std::string& str, std::shared_ptr<server::client_t>& client)
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
  std::vector<std::shared_ptr<logic::desk_t>>::iterator get_desk(const std::shared_ptr<server::client_t>& client);
  std::vector<std::shared_ptr<server::client_t>>::iterator get_opponent(const std::shared_ptr<server::client_t>& client);
  void board_updated(std::shared_ptr<server::client_t>& client);
  void start_new_game(std::shared_ptr<server::client_t>& client);

  template<typename T>
  void handle_fn(const std::string& str, std::shared_ptr<server::client_t>& /*client*/)
  {
    SPDLOG_ERROR("For type={} tactic isn't defined! msg={}", typeid(T).name(), str);
  }

  std::vector<std::shared_ptr<server::client_t>> clients;
  std::vector<std::shared_ptr<logic::desk_t>> desks;
};

template<>
void handle_message_t::process_mess<boost::mpl::end<msg::message_types>::type>(const std::string& /*str*/, std::shared_ptr<server::client_t>& /*client*/);

#define handle_macro(struct_type)                                                                                                                              \
  template<>                                                                                                                                                   \
  void handle_message_t::handle_fn<struct_type>(const std::string& str, std::shared_ptr<server::client_t>& client);
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
