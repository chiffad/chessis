#include <qqmlcontext.h>
#include <QQmlApplicationEngine>
#include <QApplication>
#include <QQmlComponent>
#include <QQmlProperty>
#include "headers/board_graphic.h"
#include "headers/exporter.h"
#include "headers/udp_socet.h"

int main(int argc, char *argv[])
{
  QGuiApplication app(argc, argv);
  Board_graphic *board_graphic = new Board_graphic;
  UDP_socet *udp_socet = new UDP_socet;
  Exporter *exporter = new Exporter(board_graphic, udp_socet);

  QQmlApplicationEngine engine;
  engine.rootContext()->setContextProperty("FigureModel", board_graphic);
  //engine.load(QUrl(QStringLiteral("chessis/res/app.qml")));

  while(!app.closingDown())
  {
    if(board_graphic->is_new_command_appear())
      exporter->push_to_socet(exporter->pull_from_graphic());

    if(udp_socet->is_new_message_received())
      exporter->push_to_graphic(exporter->pull_from_socet());
  }

  delete board_graphic;
  delete udp_socet;
  delete exporter;

  return app.exec();
}
