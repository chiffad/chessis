#include <QApplication>
#include <QByteArray>
#include <QDebug>
#include <QVector>
#include <memory>
#include <exception>
#include <algorithm>

#include "server.h"
#include "client.h"
#include "desk.h"
#include "log.h"
#include "messages.h"


QByteArray get_board_state(const std::shared_ptr<const logic::desk_t>& d);
QByteArray get_person_inf(const std::shared_ptr<const sr::client_t>& c);

int main(int argc, char *argv[]) try
{
  QApplication app(argc, argv);

  sr::server_t server;
  QVector<std::shared_ptr<sr::client_t>> clients;
  QVector<std::shared_ptr<logic::desk_t>> desks;

  while(true)
  {
    app.processEvents();

    for(auto data : server.pull())
    {
      auto c = std::find_if(clients.cbegin(), clients.cend(), [&data](const auto& i){ return (i->get_port() == data.port && i->get_ip() == data.ip); });

      if(c == clients.end())
      {
        qDebug()<<"main: New client";
        clients.append(std::make_shared<sr::client_t>(data.port, data.ip));
        c = &clients.last();

        for(const auto c2 : clients)
        {
          if(c2 == (*c))
            { continue; }

          if(desks.end() == std::find_if(desks.cbegin(), desks.cend(), [&](const auto& d){ return d->is_contain_player(c2); }))
          {
            desks.append(std::make_shared<logic::desk_t>((*c), c2));

            const QByteArray m = get_board_state(desks.last());
            (*c)->push_for_send(m);
            (c2)->push_for_send(m);
          }
        }
      }

      (*c)->push_from_server(data.message);
    }

    for(auto c : clients)
    {
      if(c->is_message_for_server_append())
        { server.send(c->pull_for_server(), c->get_port(), c->get_ip()); }

      if(c->is_message_for_logic_append())
      {
        const auto message = c->pull_for_logic();
        qDebug()<<"main: message_from_logic:"<<message;
        auto desk = std::find_if(desks.begin(), desks.end(), [&c](const auto& d){ return d->is_contain_player(c); });

        const int type = message.mid(0, message.indexOf(" ")).toInt();

        if(type == messages::HELLO_SERVER)
        {
          const auto log = message.mid(message.indexOf(" ") + 1);
          if(clients.end() != std::find_if(clients.begin(), clients.end(),
                                           [&log](const auto& i){ return i->get_login() == log; }))
            { c->push_for_send(QByteArray::number(messages::INCORRECT_LOG)); }
          else c->set_login(log);

          continue;
        }

        if(desk == desks.end())
        {
          qDebug()<<"main: desk == desk.end()";

          if(type == messages::OPPONENT_INF)
            { c->push_for_send("No opponent: no game in progress!"); }

          continue;
        }

        const auto data = message.mid(message.indexOf(" ") + 1);
        qDebug()<<"main: message: "<<data;

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
            (*opp)->push_for_send(QByteArray::number(messages::OPPONENT_LOST));
            break;
          }
          default:
            switch(type)
            {
              case messages::MOVE:
                (*desk)->make_moves_from_str(data.toStdString());
                break;
              case messages::BACK_MOVE:
                (*desk)->back_move();
                break;
              case messages::GO_TO_HISTORY:
                (*desk)->go_to_history_index(data.toInt());
                break;
              case messages::NEW_GAME:
                (*desk)->start_new_game();
                break;
              default:
                sr::throw_exception("Unknown message type!: ", type);
            }

            auto c2 = std::find(clients.begin(), clients.end(), (*desk)->get_opponent(c).lock());

            if(c2 == clients.end())
              { sr::throw_exception("c2 == client.end()"); }

            const QByteArray m = get_board_state(*desk);
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

QByteArray get_board_state(const std::shared_ptr<const logic::desk_t>& d)
{
  return (QByteArray::number(messages::GAME_INF) + " " + QByteArray::fromStdString(d->get_board_mask()) + ";"
          + QByteArray::fromStdString(d->get_moves_history()) + (d->is_mate() ? "#;" : ";")
          + QByteArray::number(d->get_move_num()));
}

QByteArray get_person_inf(const std::shared_ptr<const sr::client_t>& c)
{
  return (QByteArray::number(messages::INF_REQUEST)
          + " Login: " + c->get_login() + "; Elo rating: " + QByteArray::number(c->get_rating()));
}

