#include <QApplication>
#include <QDebug>
#include <QVector>
#include <memory>
#include <exception>
#include <algorithm>

#include "server.h"
#include "client.h"
#include "desk.h"


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

      (*c)->push(data.message);
    }

    for(auto c : clients)
    {
      if(c->is_message_for_server_append())
        { server.send(c->pull_for_server(), c->get_port(), c->get_ip()); }

      if(c->is_message_for_logic_append())
      {
        const auto message = c->pull_for_logic();
        auto desk = std::find_if(desks.begin(), desks.end(), [&c](auto d){ return d->is_contain_player(c); });

        if(desk == desks.end())
          { qDebug()<<"main: desk == desk.end()"; continue; }
      }
    }
  }

  return 0;
}

catch(std::exception const& ex)
{
  qDebug()<<"Exception!"<<ex.what();
}

