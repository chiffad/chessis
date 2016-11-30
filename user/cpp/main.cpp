#include <QQmlContext>
#include <QQmlApplicationEngine>
#include <QApplication>
#include <QWindow>
#include <ctime>
#include <exception>

#include "board_graphic.h"
#include "my_socket.h"
#include "fb_obj.h"


int main(int argc, char *argv[])
{
  qmlRegisterType<graphic::Fb_obj>("CubeRendering", 1, 0, "Cube");

  QGuiApplication app(argc, argv);
  QQmlApplicationEngine engine;
  graphic::Board_graphic board_graphic;

  engine.rootContext()->setContextProperty("FigureModel", &board_graphic);
//  engine.load(QUrl(QStringLiteral("../c/res/app.qml")));

  inet::my_socket socket;
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
        qDebug()<<"sending:"<<m;
        socket.send(m);
      }

      while(socket.is_message_append())
      {
         const QString message = socket.pull();
         const int type = message.mid(0, message.indexOf(" ")).toInt();
         if(type == Messages::INF_REQUEST)
           { board_graphic.add_to_command_history(message.mid(message.indexOf(" "))); }

         else if(type == Messages::SERVER_LOST
                 || type == Messages::SERVER_HERE
                 || type == Messages::OPPONENT_LOST)
           { board_graphic.set_connect_status(message.toInt()); }

         else if(type == Messages::GAME_INF)
         {
           QString m = message.mid(message.indexOf(" ") + 1);
           const int INDEX = m.indexOf(";");
           const int NEXT_IND = INDEX + 1;
           const QString board_mask = m.mid(0, INDEX);
           const QString moves_history = m.mid(NEXT_IND, m.indexOf(";", NEXT_IND) - NEXT_IND);
           const QString move_num = m.mid(m.indexOf(";", NEXT_IND) + 1);

           board_graphic.set_board_mask(board_mask);
           board_graphic.set_move_color(move_num.toInt());
           board_graphic.set_connect_status(Messages::SERVER_HERE);
           board_graphic.set_moves_history(moves_history);
           board_graphic.update_hilight(move_num.toInt(),moves_history);

           if(moves_history.endsWith("#"))
             { board_graphic.set_check_mate(); }
         }
         else qDebug()<<"Warning! in Exporter::push_to_graphic: Unknown message type";
      }
    }
  }

  return 0;
}
