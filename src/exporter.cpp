#include <QByteArray>
#include <QString>
#include "headers/exporter.h"
#include "headers/board_graphic.h"
#include "headers/udp_socket.h"
#include "headers/enums.h"

Exporter::Exporter(Board_graphic *const board_graphic, UDP_socket *const udp_socet)
                  : _board_graphic(board_graphic), _udp_socet(udp_socet)
{}

void Exporter::push_to_graphic(const QString& message)
{
 // qDebug()<<"===push_message"<<message;

  if(message.indexOf(";") > 0)
  {
    const int INDEX = message.indexOf(";");
    const QString board_mask = message.mid(0, INDEX);
    const QString moves_history = message.mid(INDEX + 1, message.indexOf(";", INDEX) - INDEX);
    const QString move_num = message.mid(message.indexOf(";", INDEX + 1) + 1);

    qDebug()<<"board_mask: "<<board_mask;
    qDebug()<<"moves_history: "<<moves_history;
    qDebug()<<"move_num: "<<move_num;

    _board_graphic->set_board_mask(board_mask);
    _board_graphic->set_move_color(move_num.toInt());
    _board_graphic->set_connect_status(Messages::SERVER_HERE);
    if(!moves_history.isEmpty())
      _board_graphic->set_moves_history(moves_history);

    if(moves_history.endsWith("#"))
      _board_graphic->set_check_mate();
  }
  else _board_graphic->set_connect_status(message.toInt());
}

void Exporter::push_to_socet(const QString& message)
{
  //qDebug()<<"====push_message_to_socet";
  QByteArray m;
  m.append(message);
  _udp_socet->send_data(m);
}

const QString Exporter::pull_from_graphic()
{
  //qDebug()<<"====pull_message_from_graphic";
  return _board_graphic->pull_first_messages_for_server();
}

const QString Exporter::pull_from_socet()
{
  //qDebug()<<"====pull_message_from_socet";
  return _udp_socet->pull_received_message();
}
