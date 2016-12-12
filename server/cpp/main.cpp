#include <QApplication>
#include <QDebug>
#include <QVector>
#include <memory>
#include <exception>

#include "server.h"


int main(int argc, char *argv[]) try
{
  QApplication app(argc, argv);

  sr::server_t server;

  return 0;
}

catch(std::exception const& ex)
{
  qDebug()<<"Exception!"<<ex.what();
}

