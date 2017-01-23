#include "handle_message.h"

#include "helper.h"


using namespace sr;
using namespace msg;

std::string get_board_state(const std::shared_ptr<const logic::desk_t>& d)
{
  return prepare_for_send(game_inf_t(d->get_board_mask(), d->get_moves_history(), d->is_mate(), d->get_move_num()));
}

std::string get_person_inf(const std::shared_ptr<const sr::client_t>& c)
{
  return prepare_for_send(inf_request_t("Login: " + c->get_login() +
                                        "; Elo raing: " + std::to_string(c->get_rating())));
}

template<>
void handle_message_t::handle<login_t>(const std::string& message, std::shared_ptr<sr::client_t>& client)
{
  sr::helper::log("tactic for login_t ", message);
  
  auto login = init<login_t>(message);

  if(clients.end() != std::find_if(clients.begin(), clients.end(),
                                   [&login](const auto& i){ return i->get_login() == login.login; }))
    { client->push_for_send(prepare_for_send(incorrect_log_t())); }
  else
  {
    client->set_login(login.login);
    for(const auto c2 : clients)
    {
      if(c2 == client)
        { continue; }

      if(desks.end() == get_desk(c2))
      {
        desks.push_back(std::make_shared<logic::desk_t>(client, c2));

        const auto _1 = get_board_state(desks.back());
        client->push_for_send(_1);
        c2->push_for_send(_1);
      }
    }
  }
}
  
template<>
void handle_message_t::handle<opponent_inf_t>(const std::string& message, std::shared_ptr<sr::client_t>& client)
{
  sr::helper::log("tactic for opponent_inf_t ", message);
  auto desk = get_desk(client);
  if(desk == desks.end())
  {
    sr::helper::log("desk == desk.end()"); 
    client->push_for_send(prepare_for_send(inf_request_t("No opponent: no game in progress!")));
  }
  else
  {
    client->push_for_send(get_person_inf(*get_opponent(client)));
  }
}

template<>
void handle_message_t::handle<my_inf_t>(const std::string& message, std::shared_ptr<sr::client_t>& client)
{
  sr::helper::log("tactic for my_inf_t ", message);
  client->push_for_send(get_person_inf(client));
}

template<>
void handle_message_t::handle<client_lost_t>(const std::string& message, std::shared_ptr<sr::client_t>& client)
{
  sr::helper::log("tactic for client_lost_t ", message);
  
  (*get_opponent(client))->push_for_send(prepare_for_send(opponent_lost_t()));
}

template<>
void handle_message_t::handle<move_t>(const std::string& message, std::shared_ptr<sr::client_t>& client)
{
  sr::helper::log("tactic for move_t ", message);
  
  (*get_desk(client))->make_moves_from_str((init<move_t>(message)).data);
}

template<>
void handle_message_t::handle<back_move_t>(const std::string& message, std::shared_ptr<sr::client_t>& client)
{
  sr::helper::log("tactic for back_move_t ", message);
  
  (*get_desk(client))->back_move();
}

template<>
void handle_message_t::handle<go_to_history_t>(const std::string& message, std::shared_ptr<sr::client_t>& client)
{
  sr::helper::log("tactic for go_to_history_t ", message);
  
  (*get_desk(client))->go_to_history_index((init<go_to_history_t>(message)).index);
}

template<>
void handle_message_t::handle<new_game_t>(const std::string& message, std::shared_ptr<sr::client_t>& client)
{
  sr::helper::log("tactic for start_new_game_t ", message);
  
  (*get_desk(client))->start_new_game();
}

void handle_message_t::board_updated(std::shared_ptr<sr::client_t>& client)
{
  auto opponent = get_opponent(client);

  if(opponent == clients.end())
    { sr::helper::throw_except("opponent == client.end()"); }

  const auto _1 = get_board_state(*get_desk(client));
  client->push_for_send(_1);
  (*opponent)->push_for_send(_1);
}

void handle_message_t::new_message(boost::asio::io_service& io_service, const boost::asio::ip::udp::endpoint& addr, const std::string& message)
{
  auto c = std::find_if(clients.rbegin(), clients.rend(), [&addr](const auto& i){ return (i->get_address() == addr); });

  if(c == clients.rend())
  {
    sr::helper::log("New client");
    clients.push_back(std::make_shared<sr::client_t>(io_service, addr));
    c = clients.rbegin();
  }
  (*c)->push_from_server(message);
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
  return std::find_if(desks.begin(), desks.end(), [&client](const auto& d){ return d->is_contain_player(client); });
}

std::vector<std::shared_ptr<sr::client_t>>::iterator handle_message_t::get_opponent(const std::shared_ptr<sr::client_t>& client)
{
  return std::find(clients.begin(), clients.end(), (*get_desk(client))->get_opponent(client).lock());
}

