#include "handle_message.h"

#include "helper.h"
#include <spdlog/spdlog.h>

using namespace sr;
using namespace msg;

namespace {
inline std::string get_board_state(const std::shared_ptr<const logic::desk_t>& d, const bool playing_white)
{
  return prepare_for_send(game_inf_t(d->get_board_mask(), d->get_moves_history(), d->is_mate(), d->get_move_num(), playing_white));
}

inline std::string get_person_inf(const std::shared_ptr<const sr::client_t>& c)
{
  return prepare_for_send(inf_request_t("Login: " + c->get_login() + "; Elo rating: " + std::to_string(c->get_rating())));
}

} // namespace

template<>
void handle_message_t::handle_fn<login_t>(const std::string& message, std::shared_ptr<sr::client_t>& client)
{
  SPDLOG_DEBUG("tactic for login_t; msg={}", message);

  auto login = init<login_t>(message);

  if (clients.end() !=
      std::find_if(clients.begin(), clients.end(), [&login](const auto& i) { return i->get_login() == login.login && i->get_pwd() == login.pwd; }))
  {
    client->push_for_send(prepare_for_send(incorrect_log_t()));
  }
  else
  {
    client->set_login_pwd(login.login, login.pwd);
    for (const auto c2 : clients)
    {
      if (c2 == client) continue;
      if (desks.end() != get_desk(c2)) continue;

      desks.push_back(std::make_shared<logic::desk_t>(client, c2));
      start_new_game(client);
    }
  }
}

template<>
void handle_message_t::handle_fn<opponent_inf_t>(const std::string& message, std::shared_ptr<sr::client_t>& client)
{
  SPDLOG_DEBUG("tactic for opponent_inf_t; msg={}", message);
  auto opp = get_opponent(client);
  if (opp == clients.end())
  {
    client->push_for_send(prepare_for_send(inf_request_t("No opponent: no game in progress!")));
  }
  else
  {
    client->push_for_send(get_person_inf(*opp));
  }
}

template<>
void handle_message_t::handle_fn<my_inf_t>(const std::string& message, std::shared_ptr<sr::client_t>& client)
{
  SPDLOG_DEBUG("tactic for my_inf_t; msg={}", message);
  client->push_for_send(get_person_inf(client));
}

template<>
void handle_message_t::handle_fn<client_lost_t>(const std::string& message, std::shared_ptr<sr::client_t>& client)
{
  SPDLOG_DEBUG("tactic for client_lost_t; msg={}", message);

  auto opp = get_opponent(client);
  if (opp != clients.end())
  {
    (*opp)->push_for_send(prepare_for_send(opponent_lost_t()));
  }
}

template<>
void handle_message_t::handle_fn<move_t>(const std::string& message, std::shared_ptr<sr::client_t>& client)
{
  SPDLOG_DEBUG("tactic for move_t; msg={}", message);

  (*get_desk(client))->make_moves_from_str((init<move_t>(message)).data);
  board_updated(client);
}

template<>
void handle_message_t::handle_fn<back_move_t>(const std::string& message, std::shared_ptr<sr::client_t>& client)
{
  SPDLOG_DEBUG("tactic for back_move_t; msg={}", message);

  (*get_desk(client))->back_move();
  board_updated(client);
}

template<>
void handle_message_t::handle_fn<go_to_history_t>(const std::string& message, std::shared_ptr<sr::client_t>& client)
{
  SPDLOG_DEBUG("tactic for go_to_history_t; msg={}", message);

  (*get_desk(client))->go_to_history_index((init<go_to_history_t>(message)).index);
  board_updated(client);
}

template<>
void handle_message_t::handle_fn<new_game_t>(const std::string& message, std::shared_ptr<sr::client_t>& client)
{
  SPDLOG_DEBUG("tactic for start_new_game_t; msg={}", message);
  start_new_game(client);
}

void handle_message_t::board_updated(std::shared_ptr<sr::client_t>& client)
{
  auto opponent = get_opponent(client);
  if (opponent == clients.end())
  {
    sr::helper::throw_except("opponent == client.end()");
  }

  const auto& desk = *get_desk(client);
  client->push_for_send(get_board_state(desk, client->playing_white()));
  (*opponent)->push_for_send(get_board_state(desk, (*opponent)->playing_white()));
}

void handle_message_t::new_message(boost::asio::io_service& io_service, const boost::asio::ip::udp::endpoint& addr, const std::string& message)
{
  auto c = std::find_if(clients.rbegin(), clients.rend(), [&addr](const auto& i) { return (i->get_address() == addr); });

  if (c == clients.rend())
  {
    SPDLOG_INFO("New client! addr={}", addr);
    clients.push_back(std::make_shared<sr::client_t>(io_service, addr));
    c = clients.rbegin();
  }
  (*c)->push_from_server(message);
}

template<>
void handle_message_t::process_mess<boost::mpl::end<msg::message_types>::type>(const std::string& /*str*/, std::shared_ptr<sr::client_t>& /*client*/)
{
  SPDLOG_ERROR("no type found");
}

std::vector<std::shared_ptr<sr::client_t>>::iterator handle_message_t::begin() noexcept
{
  return clients.begin();
}

std::vector<std::shared_ptr<sr::client_t>>::iterator handle_message_t::end() noexcept
{
  return clients.end();
}

std::vector<std::shared_ptr<logic::desk_t>>::iterator handle_message_t::get_desk(const std::shared_ptr<sr::client_t>& client)
{
  return std::find_if(desks.begin(), desks.end(), [&client](const auto& d) { return d->is_contain_player(client); });
}

std::vector<std::shared_ptr<sr::client_t>>::iterator handle_message_t::get_opponent(const std::shared_ptr<sr::client_t>& client)
{
  const auto d = get_desk(client);
  if (d == desks.end())
  {
    return clients.end();
  } // throw std::logic_error("In handle_message_t::get_opponent: No desk found!"); }

  return std::find(clients.begin(), clients.end(), (*d)->get_opponent(client).lock());
}

void handle_message_t::start_new_game(std::shared_ptr<sr::client_t>& client)
{
  (*get_desk(client))->start_new_game();
  const bool white_player = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() % 2 == 0;
  client->set_playing_white(white_player);
  (*get_opponent(client))->set_playing_white(!client->playing_white());
  board_updated(client);
}
