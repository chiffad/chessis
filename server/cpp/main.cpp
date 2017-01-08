#include <vector>
#include <string>
#include <memory>
#include <exception>
#include <algorithm>
#include <boost/asio.hpp>

#include "server.h"
#include "client.h"
#include "desk.h"
#include "helper.h"
#include "messages.h"


std::string get_board_state(const std::shared_ptr<const logic::desk_t>& d);
std::string get_person_inf(const std::shared_ptr<const sr::client_t>& c);

int main() try
{
  boost::asio::io_service io_service;
  sr::server_t server(io_service);
  std::vector<std::shared_ptr<sr::client_t>> clients;
  std::vector<std::shared_ptr<logic::desk_t>> desks;

  while(true)
  {
    io_service.run_one();

    for(auto data : server.pull())
    {
      auto c = std::find_if(clients.cbegin(), clients.cend(), [&data](const auto& i){ return (i->get_address() == data.address); });

      if(c == clients.end())
      {
        sr::helper::log("New client");
        clients.push_back(std::make_shared<sr::client_t>(io_service, data.address));
        c = --clients.end();
      }
      (*c)->push_from_server(data.message);
    }

    for(auto c : clients)
    {
      if(c->is_message_for_server_append())
        { server.send(c->pull_for_server().data(), c->get_address()); }

      if(c->is_message_for_logic_append())
      {
        const auto message = c->pull_for_logic();
        sr::helper::log("message_from_logic: ", message);
        const auto type = msg::get_msg_type(message);

        if(type == msg::get_type<msg::login_t>::value)
        {
          auto login = msg::init<msg::login_t>(message);

          if(clients.end() != std::find_if(clients.begin(), clients.end(),
                                           [&login](const auto& i){ return i->get_login() == login.login; }))
            { c->push_for_send(msg::prepare_for_send(msg::incorrect_log_t())); }
          else
          {
            c->set_login(login.login);
            for(const auto c2 : clients)
            {
              if(c2 == c)
                { continue; }

              if(desks.end() == std::find_if(desks.cbegin(), desks.cend(), [&](const auto& d){ return d->is_contain_player(c2); }))
              {
                desks.push_back(std::make_shared<logic::desk_t>(c, c2));

                const auto _1 = get_board_state(desks.back());
                c->push_for_send(_1);
                c2->push_for_send(_1);
              }
            }
          }

          continue;
        }

        auto desk = std::find_if(desks.begin(), desks.end(), [&c](const auto& d){ return d->is_contain_player(c); });
        if(desk == desks.end())
        {
          sr::helper::log("desk == desk.end()");

          if(type == msg::get_type<msg::opponent_inf_t>::value)
            { c->push_for_send(msg::prepare_for_send(msg::inf_request_t("No opponent: no game in progress!"))); }

          continue;
        }

        switch(type)
        {
          //case msg::message_received_t: //in client
          //case msg::is_server_lost_t: // no need cause on this message already was sended responce
          case msg::get_type<msg::opponent_inf_t>::value:
          {
            auto opp = std::find(clients.begin(), clients.end(), (*desk)->get_opponent(c).lock());
            c->push_for_send(get_person_inf(*opp));
            break;
          }
          case msg::get_type<msg::my_inf_t>::value:
            c->push_for_send(get_person_inf(c));
            break;
          case msg::get_type<msg::client_lost_t>::value:
          {
            auto opp = std::find(clients.begin(), clients.end(), (*desk)->get_opponent(c).lock());
            (*opp)->push_for_send(msg::prepare_for_send(msg::opponent_lost_t()));
            break;
          }
          default:
            switch(type)
            {
              case msg::get_type<msg::move_t>::value:
              {
                auto move = msg::init<msg::move_t> (message);
                (*desk)->make_moves_from_str(move.data);
                break;
              }
              case msg::get_type<msg::back_move_t>::value:
                (*desk)->back_move();
                break;
              case msg::get_type<msg::go_to_history_t>::value:
              {
                auto gth = msg::init<msg::go_to_history_t> (message);
                (*desk)->go_to_history_index(gth.index);
                break;
              }
              case msg::get_type<msg::new_game_t>::value:
                (*desk)->start_new_game();
                break;
              default:
                sr::helper::throw_except("Unknown message type!: ", type);
            }

            auto c2 = std::find(clients.begin(), clients.end(), (*desk)->get_opponent(c).lock());

            if(c2 == clients.end())
              { sr::helper::throw_except("c2 == client.end()"); }

            const auto _1 = get_board_state(*desk);
            c->push_for_send(_1);
            (*c2)->push_for_send(_1);
        }
      }
    }
  }
  
  return 0;
}

catch(std::exception const& ex)
{
  std::cout<<"Exception! "<<ex.what()<<std::endl;
}

std::string get_board_state(const std::shared_ptr<const logic::desk_t>& d)
{
  return msg::prepare_for_send(msg::game_inf_t(d->get_board_mask(), d->get_moves_history(),
                                               d->is_mate(), d->get_move_num()));
}

std::string get_person_inf(const std::shared_ptr<const sr::client_t>& c)
{
  return msg::prepare_for_send(msg::inf_request_t("Login: " + c->get_login() +
                                                  "; Elo raing: " + std::to_string(c->get_rating())));
}
