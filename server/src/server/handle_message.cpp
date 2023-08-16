#include "server/handle_message.hpp"

#include "helper.h"
#include <spdlog/spdlog.h>

namespace server {

namespace {
inline std::string get_board_state(const logic::desk_t& d, const bool playing_white)
{
  return prepare_for_send(msg::game_inf_t(d.get_board_mask(), d.get_moves_history(), d.mate(), d.get_move_num(), playing_white));
}

inline std::string get_person_inf(const client_t& c)
{
  return prepare_for_send(msg::inf_request_t("Login: " + c.get_login() + "; Elo rating: " + std::to_string(c.get_rating())));
}

} // namespace

handle_message_t::handle_message_t(clients_holder_t& cl)
  : clients_(cl)
{}

template<>
void handle_message_t::handle_fn<msg::login_t>(const std::string& message, std::shared_ptr<client_t>& client)
{
  SPDLOG_DEBUG("tactic for login_t; msg={}", message);
  auto login = msg::init<msg::login_t>(message);

  if (clients_.end() != clients_.get(login.login))
  {
    client->push_for_send(prepare_for_send(msg::incorrect_log_t()));
    return;
  }

  client->set_login_pwd(login.login, login.pwd);
  for (const auto c2 : clients_)
  {
    if (c2 == client) continue;
    if (desks_.end() != get_desk(c2)) continue;

    desks_.emplace_back(client, c2);
    start_new_game(client);
  }
}

template<>
void handle_message_t::handle_fn<msg::opponent_inf_t>(const std::string& message, std::shared_ptr<client_t>& client)
{
  SPDLOG_DEBUG("tactic for opponent_inf_t; msg={}", message);
  auto opp = get_opponent(client);
  if (opp == clients_.end())
  {
    client->push_for_send(prepare_for_send(msg::inf_request_t("No opponent: no game in progress!")));
  }
  else
  {
    client->push_for_send(get_person_inf(*(*opp)));
  }
}

template<>
void handle_message_t::handle_fn<msg::my_inf_t>(const std::string& message, std::shared_ptr<client_t>& client)
{
  SPDLOG_DEBUG("tactic for my_inf_t; msg={}", message);
  client->push_for_send(get_person_inf(*client));
}

template<>
void handle_message_t::handle_fn<msg::client_lost_t>(const std::string& message, std::shared_ptr<client_t>& client)
{
  SPDLOG_DEBUG("tactic for client_lost_t; msg={}", message);

  auto opp = get_opponent(client);
  if (opp != clients_.end())
  {
    (*opp)->push_for_send(prepare_for_send(msg::opponent_lost_t()));
  }
}

template<>
void handle_message_t::handle_fn<msg::move_t>(const std::string& message, std::shared_ptr<client_t>& client)
{
  SPDLOG_DEBUG("tactic for move_t; msg={}", message);

  logic::make_moves_from_str((msg::init<msg::move_t>(message)).data, *get_desk(client));
  board_updated(client);
}

template<>
void handle_message_t::handle_fn<msg::back_move_t>(const std::string& message, std::shared_ptr<client_t>& client)
{
  SPDLOG_DEBUG("tactic for back_move_t; msg={}", message);

  get_desk(client)->back_move();
  board_updated(client);
}

template<>
void handle_message_t::handle_fn<msg::go_to_history_t>(const std::string& message, std::shared_ptr<client_t>& client)
{
  SPDLOG_DEBUG("tactic for go_to_history_t; msg={}", message);

  get_desk(client)->go_to_history(msg::init<msg::go_to_history_t>(message).index);
  board_updated(client);
}

template<>
void handle_message_t::handle_fn<msg::new_game_t>(const std::string& message, std::shared_ptr<client_t>& client)
{
  SPDLOG_DEBUG("tactic for start_new_game_t; msg={}", message);
  start_new_game(client);
}

void handle_message_t::board_updated(std::shared_ptr<client_t>& client)
{
  auto opponent = get_opponent(client);
  if (opponent == clients_.end())
  {
    helper::throw_except("opponent == client.end()");
  }

  const auto& desk = *get_desk(client);

  client->push_for_send(get_board_state(desk, client->playing_white()));
  (*opponent)->push_for_send(get_board_state(desk, (*opponent)->playing_white()));
}

void handle_message_t::new_message(boost::asio::io_service& io_service, const boost::asio::ip::udp::endpoint& addr, const std::string& message)
{
  auto c = clients_.get(addr);
  if (c != clients_.end())
  {
    (*c)->push_from_server(message);
    return;
  }

  SPDLOG_INFO("New client! addr={}", addr);
  clients_.add(addr).push_from_server(message);
  ;
}

template<>
void handle_message_t::process_mess<boost::mpl::end<msg::message_types>::type>(const std::string& /*str*/, std::shared_ptr<client_t>& /*client*/)
{
  SPDLOG_ERROR("no type found");
}

handle_message_t::desks_arr_t::iterator handle_message_t::get_desk(const std::shared_ptr<client_t>& client)
{
  return std::find_if(desks_.begin(), desks_.end(), [&client](const auto& d) { return d.contains_player(client); });
}

clients_arr_t::iterator handle_message_t::get_opponent(const std::shared_ptr<client_t>& client)
{
  const auto d = get_desk(client);
  if (d == desks_.end())
  {
    return clients_.end();
  } // throw std::logic_error("In handle_message_t::get_opponent: No desk found!"); }

  return std::find(clients_.begin(), clients_.end(), d->get_opponent(client).lock());
}

void handle_message_t::start_new_game(std::shared_ptr<client_t>& client)
{
  get_desk(client)->start_new_game();
  const bool white_player = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() % 2 == 0;
  client->set_playing_white(white_player);
  (*get_opponent(client))->set_playing_white(!client->playing_white());
  board_updated(client);
}

} // namespace server