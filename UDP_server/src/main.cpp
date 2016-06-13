#include <QApplication>
#include "headers/udp_server.h"

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  UDP_server client;
  return a.exec();
}
