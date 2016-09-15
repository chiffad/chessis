#include <QByteArray>
#include <QString>
#include "exporter.h"
#include "board_graphic.h"
#include "udp_socket.h"
#include "enums.h"

Exporter::Exporter(Board_graphic *const board_graphic, UDP_socket *const udp_socet)
                  : _board_graphic(board_graphic), _udp_socet(udp_socet)
{}

void Exporter::push_to_graphic(const QString& message)
{
  qDebug()<<"Exporter::push_message"<<message;

  const QString type = message.mid(0, message.indexOf(" "));
  qDebug()<<"type: "<<type;

  if(type.toInt() == Messages::INF_REQUEST)
    _board_graphic->add_to_command_history(message.mid(message.indexOf(" ")));

  else if(type.toInt() == Messages::SERVER_LOST
       || type.toInt() == Messages::SERVER_HERE
       || type.toInt() == Messages:: OPPONENT_LOST)
  {
    _board_graphic->set_connect_status(message.toInt());
  }

  else if(message.indexOf(";") > 0)
  {
    const int INDEX = message.indexOf(";");
    const int NEXT_IND = INDEX + 1;
    const QString board_mask = message.mid(0, INDEX);
    const QString moves_history = message.mid(NEXT_IND, message.indexOf(";", NEXT_IND) - NEXT_IND);
    const QString move_num = message.mid(message.indexOf(";", NEXT_IND) + 1);

    qDebug()<<"board_mask: "<<board_mask;
    qDebug()<<"moves_history: "<<moves_history;
    qDebug()<<"move_num: "<<move_num;

    _board_graphic->set_board_mask(board_mask);
    _board_graphic->set_move_color(move_num.toInt());
    _board_graphic->set_connect_status(Messages::SERVER_HERE);
    _board_graphic->set_moves_history(moves_history);
    _board_graphic->update_hilight(move_num.toInt(),moves_history);

    if(moves_history.endsWith("#"))
      _board_graphic->set_check_mate();
  }
}

void Exporter::push_to_socet(const QString& message)
{
  //qDebug()<<"Exporter::push_message_to_socet";
  QByteArray m;
  m.append(message);
  _udp_socet->send_data(m);
}

const QString Exporter::pull_from_graphic()
{
  //qDebug()<<"Exporter::pull_message_from_graphic";
  return _board_graphic->pull_first_messages_for_server();
}

const QString Exporter::pull_from_socet()
{
  //qDebug()<<"Exporter::pull_message_from_socet";
  return _udp_socet->pull_received_message();
}
