#include <vector>
#include <string>
#include <memory>
#include <exception>
#include <algorithm>
#include <boost/asio.hpp>

#include "server.h"
#include "client.h"
#include "desk.h"
#include "log.h"
#include "messages.h"


std::string get_board_state(const std::shared_ptr<const logic::desk_t>& d);
std::string get_person_inf(const std::shared_ptr<const sr::client_t>& c);

void print(const boost::system::error_code& /*e*/)
{
  sr::log("Hello, world!");
}

int main() try
{
 /* boost::asio::io_service io;
  boost::asio::io_service::work work(io);

  boost::asio::deadline_timer t(io, boost::posix_time::seconds(1));
//  t.expires_from_now(boost::posix_time::seconds(1));
  t.cancel();
  t.async_wait(&print);
  sr::log("here!");
  io.run_one();

  t.expires_from_now(boost::posix_time::seconds(1));
  t.async_wait(&print);
//  t.expires_from_now(boost::posix_time::seconds(1));
  io.run_one();

  return 0;
*/


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
        sr::log("New client");
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
        auto _1 = c->pull_for_logic();
        sr::log("message_from_logic: ", _1);
        const auto type = messages::cut_type(_1);
        const auto message(std::move(_1));

        if(type == messages::LOGIN)
        {
          messages::login_t login(message);
          if(!login.is_ok)
            { continue; }

          if(clients.end() != std::find_if(clients.begin(), clients.end(),
                                           [&login](const auto& i){ return i->get_login() == login.login; }))
            { c->push_for_send(std::to_string(messages::INCORRECT_LOG)); }
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

                const std::string m = get_board_state(desks.back());
                c->push_for_send(m);
                c2->push_for_send(m);
              }
            }
          }

          continue;
        }

        auto desk = std::find_if(desks.begin(), desks.end(), [&c](const auto& d){ return d->is_contain_player(c); });
        if(desk == desks.end())
        {
          sr::log("desk == desk.end()");

          if(type == messages::OPPONENT_INF)
            { c->push_for_send(std::to_string(messages::INF_REQUEST) + " No opponent: no game in progress!"); }

          continue;
        }

        switch(type)
        {
          //case messages::MESSAGE_RECEIVED: //in client
          //case messages::IS_SERVER_LOST: // no need cause on this message already was sended responce
          case messages::OPPONENT_INF:
          {
            auto opp = std::find(clients.begin(), clients.end(), (*desk)->get_opponent(c).lock());
            c->push_for_send(get_person_inf(*opp));
            break;
          }
          case messages::MY_INF:
            c->push_for_send(get_person_inf(c));
            break;
          case messages::CLIENT_LOST:
          {
            auto opp = std::find(clients.begin(), clients.end(), (*desk)->get_opponent(c).lock());
            (*opp)->push_for_send(std::to_string(messages::OPPONENT_LOST));
            break;
          }
          default:
            switch(type)
            {
              case messages::MOVE:
              {
                messages::move_t move(message);
                if(!move.is_ok)
                  { continue; }

                (*desk)->make_moves_from_str(move.data);
                break;
              }
              case messages::BACK_MOVE:
                (*desk)->back_move();
                break;
              case messages::GO_TO_HISTORY:
              {
                messages::go_to_history_t gth(message);
                if(!gth.is_ok)
                  { continue; }

                (*desk)->go_to_history_index(gth.hist_ind);
                break;
              }
              case messages::NEW_GAME:
                (*desk)->start_new_game();
                break;
              default:
                sr::throw_except("Unknown message type!: ", type);
            }

            auto c2 = std::find(clients.begin(), clients.end(), (*desk)->get_opponent(c).lock());

            if(c2 == clients.end())
              { sr::throw_except("c2 == client.end()"); }

            const std::string m = get_board_state(*desk);
            c->push_for_send(m);
            (*c2)->push_for_send(m);
        }
      }
    }
  }

  return 0;
}

catch(std::exception const& ex)
{
  qDebug()<<"Exception!"<<ex.what();
}

std::string get_board_state(const std::shared_ptr<const logic::desk_t>& d)
{
  return (std::to_string(messages::GAME_INF) + " " + d->get_board_mask() + ";"
          + d->get_moves_history()+ ";" + (d->is_mate() ? "#;" : ";")
          + std::to_string(d->get_move_num()));
}

std::string get_person_inf(const std::shared_ptr<const sr::client_t>& c)
{
  return (std::to_string(messages::INF_REQUEST) + " Login: " + c->get_login()
          + "; Elo rating: " + std::to_string(c->get_rating()));
}

