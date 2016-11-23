#include <QQmlContext>
#include <QQmlApplicationEngine>
#include <QApplication>
#include <QWindow>
#include <ctime>
#include <exception>

#include "board_graphic.h"
#include "exporter.h"
#include "my_socket.h"
#include "fb_obj.h"


int main(int argc, char *argv[])
{
  qmlRegisterType<graphic::Fb_obj>("CubeRendering", 1, 0, "Cube");

  QGuiApplication app(argc, argv);
  QQmlApplicationEngine engine;
  graphic::Board_graphic board_graphic;

  engine.rootContext()->setContextProperty("FigureModel", &board_graphic);
  engine.load(QUrl(QStringLiteral("../c/res/app.qml")));

  inet::my_socket socket;
  Exporter exporter(board_graphic, socket);

  const double CHECK_TIME = 0.015 * CLOCKS_PER_SEC;
  clock_t timer = clock();

  while(!app.allWindows().isEmpty() && app.allWindows().last()->visibility())
  {
    app.processEvents();

    if((clock() - timer) > CHECK_TIME)
    {
      timer = clock();

      while(board_graphic.is_new_message_for_server_appear())
        { exporter.push_to_socet(board_graphic.pull_first_messages_for_server()); }

      while(socket.is_message_append())
        { exporter.push_to_graphic(socket.pull()); }
    }
  }

  return 0;
}
