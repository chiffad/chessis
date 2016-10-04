#include <QQmlContext>
#include <QQmlApplicationEngine>
#include <QApplication>
#include <QWindow>
#include <ctime>
#include <memory>
#include "board_graphic.h"
#include "exporter.h"
#include "udp_socket.h"
#include "fb_obj.h"

bool quit(const QGuiApplication &app);

int main(int argc, char *argv[])
{
  qmlRegisterType<Fb_obj>("CubeRendering", 1, 0, "Cube");

  QGuiApplication app(argc, argv);
  QQmlApplicationEngine engine;
  Board_graphic board_graphic;

  engine.rootContext()->setContextProperty("FigureModel", &board_graphic);
  engine.load(QUrl(QStringLiteral("../chessis/res/app.qml")));

  UDP_socket udp_socet;
  Exporter exporter(board_graphic, udp_socet);

  const double CHECK_TIME = 0.015 * CLOCKS_PER_SEC;
  clock_t timer = clock();

  while(!quit(app))
  {
    app.processEvents();

    if((clock() - timer) > CHECK_TIME)
    {
      timer = clock();

      while(board_graphic.is_new_message_for_server_appear())
        exporter.push_to_socet(exporter.pull_from_graphic());

      while(udp_socet.is_new_message_received())
        exporter.push_to_graphic(exporter.pull_from_socet());
    }
  }

  return 0;
}

bool quit(const QGuiApplication &app)
{
//  return false;
  return (app.allWindows().isEmpty() || !app.allWindows().last()->visibility());
}
