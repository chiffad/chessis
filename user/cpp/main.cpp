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
#include "messages.h"
#include "helper.h"
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
      {
        const auto message = client.pull();

        switch(msg::init<msg::some_datagramm_t>(message).type)
        {
          case msg::id<msg::server_lost_t  >(): handle::process(board_graphic, message, handle::type_2_type<msg::server_lost_t  >()); break;
          case msg::id<msg::server_here_t  >(): handle::process(board_graphic, message, handle::type_2_type<msg::server_here_t  >()); break;
          case msg::id<msg::opponent_lost_t>(): handle::process(board_graphic, message, handle::type_2_type<msg::opponent_lost_t>()); break;
          case msg::id<msg::inf_request_t  >(): handle::process(board_graphic, message, handle::type_2_type<msg::inf_request_t  >()); break;
          case msg::id<msg::game_inf_t     >(): handle::process(board_graphic, message, handle::type_2_type<msg::game_inf_t     >()); break;
          case msg::id<msg::get_login_t    >(): handle::process(board_graphic, message, handle::type_2_type<msg::get_login_t    >()); break;
          case msg::id<msg::incorrect_log_t>(): handle::process(board_graphic, message, handle::type_2_type<msg::incorrect_log_t>()); break;
          default:
            cl::helper::log("Warning! In main. Unknown message type: ", msg::init<msg::some_datagramm_t>(message).type);
        }
      }
    }
  }

  return 0;
}
