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
      auto c = std::find_if(clients.cbegin(), clients.cend(), [&data](auto i){ return (i->get_port() == data.port && i->get_ip() == data.ip); });

      if(c == clients.end())
      {
        clients.append(std::make_shared<sr::client_t>(data.port, data.ip));
        c = &clients.last();

        for(const auto c2 : clients)
        {
          if(desks.end() == std::find_if(desks.cbegin(), desks.cend(), [&](auto d){ return (c2 != (*c) && d->is_contain_player(c2)); }))
            { desks.append(std::make_shared<logic::desk_t>((*c), c2)); }
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
        auto desk = std::find_if(desks.begin(), desks.end(), [&c](auto d){ return d->is_contain_player(c); });

        const int type = message.mid(0, message.indexOf(" ")).toInt();

        if(desk == desks.end())
        {
          qDebug()<<"main: desk == desk.end()";

          if(type == messages::OPPONENT_INF)
            { c->push_for_send("No opponent: no game in progress!"); }

          continue;
        }

        const auto data = message.mid(message.indexOf(" ") + 1);
        qDebug()<<"main: message!!: "<<data;

        switch(type)
        {
          //case messages::MESSAGE_RECEIVED: //in client
          //case messages::IS_SERVER_LOST: // no need cause on this message already was sended responce

          case messages::OPPONENT_INF:
            //need update!
            break;
          case messages::MY_INF:
            //need update!
            break;
          case messages::MOVE:
            (*desk)->make_moves_from_str(data.toStdString());
            break;

          default:
            switch(type)
            {
              case messages::MOVE:
                (*desk)->make_moves_from_str(data.toStdString());
                break;
//need more cases!!
              default:
                sr::throw_exception("Uncnown message type!: ", type);
            }
            const QByteArray m(QByteArray::number(messages::GAME_INF)
                        + " "
                        + QByteArray::fromStdString((*desk)->get_board_mask())
                        + ";"
                        + QByteArray::fromStdString((*desk)->get_moves_history())
                        + ((*desk)->is_mate() ? "#;" : ";")
                        + QByteArray::number((*desk)->get_move_num()));
            c->push_for_send(m);

            const auto const_c2 = (*desk)->get_first_player().lock() == c ? (*desk)->get_second_player().lock() : (*desk)->get_first_player().lock();
            auto c2 = std::find(clients.begin(), clients.end(), const_c2);
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
