#include <QApplication>
#include <QDebug>
#include "udp_server.h"

#include "server.h"


int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

//  UDP_server client;

  sr::server_t server;

  while(true)
  {
    app.processEvents();
    server.process_event();

  }

  return 0;
//  return app.exec();
}
