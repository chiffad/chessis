#include <QApplication>
#include <QDebug>
#include <QVector>
#include <memory>
#include <exception>
#include <algorithm>

#include "server.h"
#include "client.h"


int main(int argc, char *argv[]) try
{
  QApplication app(argc, argv);

  sr::server_t server;
  QVector<std::shared_ptr<sr::client_t>> clients;

  while(true)
  {
    app.processEvents();
    for(auto data : server.pull())
    {
      auto client = std::find_if(clients.begin(), clients.end(), [&data](const auto& i){ return (i->get_port() == data.port && i->get_ip() == data.ip); });

      if(client == clients.end())
      {
        clients.append(std::make_shared<sr::client_t>(data.port, data.ip));
        client = &clients.last();
      }

      (*client)->push(data.message);
    }

    for(auto client : clients)
    {
      if(client->is_message_for_server_append())
        { server.send(client->pull_for_server(), client->get_port(), client->get_ip()); }

      if(client->is_message_for_logic_append())
        { server.send(client->pull_for_server(), client->get_port(), client->get_ip()); }
    }
  }

  return 0;
}

catch(std::exception const& ex)
{
  qDebug()<<"Exception!"<<ex.what();
}

