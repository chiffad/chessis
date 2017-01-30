#include <QQmlContext>
#include <QQmlApplicationEngine>
#include <QApplication>
#include <QWindow>
#include <QString>
#include <ctime>
#include <exception>

#include "board_graphic.h"
#include "client.h"
#include "fb_obj.h"
#include "handle_message.h"


int main(int argc, char *argv[])
{
  qmlRegisterType<graphic::fb_obj_t>("CubeRendering", 1, 0, "Cube");

  QGuiApplication app(argc, argv);
  QQmlApplicationEngine engine;
  graphic::board_graphic_t board_graphic;

  engine.rootContext()->setContextProperty("FigureModel", &board_graphic);
  engine.load(QUrl(QStringLiteral("../res/app.qml")));

  cl::client_t client;
  const double CHECK_TIME = 0.015 * CLOCKS_PER_SEC;
  clock_t timer = clock();

  while(!app.allWindows().isEmpty() && app.allWindows().last()->visibility())
  {
    app.processEvents();

    if((clock() - timer) > CHECK_TIME)
    {
      timer = clock();

      while(board_graphic.is_message_appear())
        { client.send(board_graphic.pull().toStdString()); }

      while(client.is_message_append())
        { handler::handle(client.pull(), board_graphic); }
    }
  }

  return 0;
}
