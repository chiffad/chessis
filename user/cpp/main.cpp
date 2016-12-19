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
#include "log.h"


int main(int argc, char *argv[])
{
  qmlRegisterType<graphic::fb_obj_t>("CubeRendering", 1, 0, "Cube");

  QGuiApplication app(argc, argv);
  QQmlApplicationEngine engine;
  graphic::board_graphic_t board_graphic;

  engine.rootContext()->setContextProperty("FigureModel", &board_graphic);
//  engine.load(QUrl(QStringLiteral("../res/app.qml")));

  cl::client_t client;
  const double CHECK_TIME = 0.015 * CLOCKS_PER_SEC;
  clock_t timer = clock();

  while(true)//!app.allWindows().isEmpty() && app.allWindows().last()->visibility())
  {
    app.processEvents();

    if((clock() - timer) > CHECK_TIME)
    {
      timer = clock();

      while(board_graphic.is_message_appear())
      {
        QByteArray m;
        m.append(board_graphic.pull());
        client.send(m);
      }

      while(client.is_message_append())
      {
        const QString message = client.pull();
        const int type = message.mid(0, message.indexOf(" ")).toInt();

        if(type == messages::SERVER_LOST
          || type == messages::SERVER_HERE
          || type == messages::OPPONENT_LOST)
        {
          cl::log("type == messages::SERVER_LOST || type == messages::SERVER_HERE || type == messages::OPPONENT_LOST");
          board_graphic.set_connect_status(message.toInt());
          continue;
        }

        else switch(type)
        {
          case messages::INF_REQUEST:
            cl::log("messages::INF_REQUEST");
            board_graphic.add_to_command_history(message.mid(message.indexOf(" ")));
            break;
          case messages::GAME_INF:
          {
            cl::log("messages::GAME_INF");
            QString m = message.mid(message.indexOf(" ") + 1);
            const int INDEX = m.indexOf(";");
            const int NEXT_IND = INDEX + 1;
            const QString board_mask = m.mid(0, INDEX);
            const QString moves_history = m.mid(NEXT_IND, m.indexOf(";", NEXT_IND) - NEXT_IND);
            const QString move_num = m.mid(m.indexOf(";", NEXT_IND) + 1);

            board_graphic.set_board_mask(board_mask);
            board_graphic.set_move_color(move_num.toInt());
            board_graphic.set_connect_status(messages::SERVER_HERE);
            board_graphic.set_moves_history(moves_history);
            board_graphic.update_hilight(move_num.toInt(),moves_history);

            if(moves_history.endsWith("#"))
              { board_graphic.set_check_mate(); }

            break;
          }
          case messages::GET_LOGIN:
            cl::log("messages::GET_LOGIN");
            board_graphic.get_login();
            break;
          case messages::INCORRECT_LOG:
            cl::log("messages::INCORRECT_LOG");
            board_graphic.get_login("This login already exist. Enter another login!");
            break;
          default:
            cl::log("Warning! Unknown message type", type);
        }
      }
    }
  }

  return 0;
}
