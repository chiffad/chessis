#include "exporter.h"

#include <QByteArray>
#include "enums.h"



Exporter::Exporter(graphic::Board_graphic& board_graphic, inet::my_socket& udp_socket)
                  : _board_graphic(board_graphic), socket(udp_socket)
{
}

void Exporter::push_to_graphic(const QString& message)
{
  const int type = message.mid(0, message.indexOf(" ")).toInt();
  if(type == Messages::INF_REQUEST)
    _board_graphic.add_to_command_history(message.mid(message.indexOf(" ")));

  else if(type == Messages::SERVER_LOST
       || type == Messages::SERVER_HERE
       || type == Messages::OPPONENT_LOST)
  {
    _board_graphic.set_connect_status(message.toInt());
  }

  else if(type == Messages::GAME_INF)
  {
    QString m = message.mid(message.indexOf(" ") + 1);
    const int INDEX = m.indexOf(";");
    const int NEXT_IND = INDEX + 1;
    const QString board_mask = m.mid(0, INDEX);
    const QString moves_history = m.mid(NEXT_IND, m.indexOf(";", NEXT_IND) - NEXT_IND);
    const QString move_num = m.mid(m.indexOf(";", NEXT_IND) + 1);

    _board_graphic.set_board_mask(board_mask);
    _board_graphic.set_move_color(move_num.toInt());
    _board_graphic.set_connect_status(Messages::SERVER_HERE);
    _board_graphic.set_moves_history(moves_history);
    _board_graphic.update_hilight(move_num.toInt(),moves_history);

    if(moves_history.endsWith("#"))
      _board_graphic.set_check_mate();
  }
  else qDebug()<<"Warning! in Exporter::push_to_graphic: Unknown message type";
}

void Exporter::push_to_socet(const QString& message)
{
  QByteArray m;
  m.append(message);
  socket.send(m);
}
