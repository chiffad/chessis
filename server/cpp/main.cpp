#include <QApplication>
#include <QDebug>
#include <QVector>
#include <memory>

#include "server.h"
#include "user.h"
#include "desk.h"
#include "messages.h"


int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  sr::server_t server;
  QVector<std::shared_ptr<sr::user_t>> users;

  while(true)
  {
    app.processEvents();

    for(const auto& i : server.get_clients_list())
    {
      const auto u = std::find_if(users.begin(), users.end(), [&](const auto& user){ return user->get_ip() == i.ip && user->get_port() == i.port;});

      if(u == users.end())
        { users.append(std::make_shared<sr::user_t>(i.ip, i.port)); }
    }

    for(auto& u : users)
    {
      const auto ip = u->get_ip();
      const auto port = u->get_port();
      const auto m = server.pull(port, ip);
      const auto type = messages::MESSAGE(m.mid(0, m.indexOf(" ")).toInt());

      switch(type)
      {
        case messages::OPPONENT_INF:
          if(!u->is_game_active())
            { server.push("No opponent!!", port, ip); }
          else
          {
            const auto _1 = std::find_if(users.begin(), users.end(), [&u](const auto& i){ return (i->get_board() == u->get_board() && i != u);});

            if(_1 != users.end())
              { server.push((*_1)->get_info(), port, ip); }
          }
          break;
        case messages::MY_INF:
          server.push(u->get_info(), port, ip);
          break;
        default:
          if(!u->is_game_active())
            { continue; }

          if(server.is_message_append(port, ip))
          {
            const QByteArray content = m.mid(m.indexOf(" ") + 1);

            u->push(type, content);
            server.push(u->get_board_state(), port, ip);
          }
      }
    }

    const auto u1 = std::find_if(users.begin(), users.end(), [](const auto& i){ return i->is_game_active();});
    const auto u2 = std::find_if(u1, users.end(), [](const auto& i){ return i->is_game_active();});

    if(u1 != users.end() && u2 != users.end())
    {
      std::shared_ptr<logic::desk_t> d(std::make_shared<logic::desk_t>());
      (*u1)->set_board(d);
      (*u2)->set_board(d);
    }

    server.process_event();
  }

  return 0;
}
