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
      {
        QByteArray m;
        m.append(board_graphic.pull());
        client.send(m);
      }

      while(client.is_message_append())
      {
        auto _1 = client.pull().toStdString();
        const auto type = messages::cut_type(_1);
        const auto message(std::move(_1));

        if(type == messages::SERVER_LOST
          || type == messages::SERVER_HERE
          || type == messages::OPPONENT_LOST)
        {
          type == messages::SERVER_LOST ? cl::helper::log("type == messages::SERVER_LOST") 
                                        :  type == messages::SERVER_HERE ? cl::helper::log("type == messages::SERVER_HERE") 
                                                                         : cl::helper::log("type == messages::OPPONENT_LOST");
          board_graphic.set_connect_status(type);
          continue;
        }

        else try
        {
          switch(type)
          {
            case messages::INF_REQUEST:
            {
              cl::helper::log("messages::INF_REQUEST");
              messages::inf_request_t inf;
              inf.from_json(message);

              board_graphic.add_to_command_history(QString::fromStdString(inf.data));
              break;
            }
            case messages::GAME_INF:
            {
              cl::helper::log("messages::GAME_INF");
              messages::game_inf_t game_inf;
              game_inf.from_json(message);

              board_graphic.set_board_mask(QString::fromStdString(game_inf.board_mask));
              board_graphic.set_move_color(game_inf.move_num);
              board_graphic.set_connect_status(messages::SERVER_HERE);
              board_graphic.set_moves_history(QString::fromStdString(game_inf.moves_history));
              board_graphic.update_hilight(game_inf.move_num, QString::fromStdString(game_inf.moves_history));
              board_graphic.redraw_board();

              if(game_inf.is_mate)
                { board_graphic.set_check_mate(); }

              break;
            }
            case messages::GET_LOGIN:
              cl::helper::log("messages::GET_LOGIN");
              board_graphic.get_login();
              break;
            case messages::INCORRECT_LOG:
              cl::helper::log("messages::INCORRECT_LOG");
              board_graphic.get_login("This login already exist. Enter another login!");
              break;
            default:
              cl::helper::log("Warning! Unknown message type: ", type);
          }
        }
        catch(const messages::my_except& e)
          { cl::helper::log("Exception! ", e.what()); }
      }
    }
  }

  return 0;
}
