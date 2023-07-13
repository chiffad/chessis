#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QString>
#include <QWindow>
#include <ctime>
#include <exception>

#include "logger.hpp"
#include <spdlog/spdlog.h>

#include "board_graphic.h"
#include "client.h"
#include "handle_message.h"

int main(int argc, char* argv[])
{
  logger::logger_t::get().init();

  QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

  qmlRegisterSingletonType(QUrl("qrc:/res/Constants.qml"), "Constants", 1, 0, "Constants");

  QGuiApplication app(argc, argv);
  QQmlApplicationEngine engine;
  graphic::board_graphic_t board_graphic;

  engine.rootContext()->setContextProperty("FigureModel", &board_graphic);
  engine.load(QUrl(QStringLiteral("qrc:/res/app.qml")));

  cl::client_t client;
  const double CHECK_TIME = 0.015 * CLOCKS_PER_SEC;
  clock_t timer = clock();

  while (!app.allWindows().isEmpty() && app.allWindows().last()->visibility())
  {
    app.processEvents();

    if ((clock() - timer) > CHECK_TIME)
    {
      timer = clock();

      while (board_graphic.is_message_appear())
      {
        client.send(board_graphic.pull().toStdString());
      }

      while (client.is_message_append())
      {
        handler::handle(client.pull(), board_graphic);
      }
    }
  }

  return 0;
}
