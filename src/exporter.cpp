#include <QByteArray>
#include <QString>
#include "headers/exporter.h"
#include "headers/board_graphic.h"
#include "headers/udp_socet.h"

Exporter::Exporter(Board_graphic *const board_graphic, UDP_socet *const udp_socet)
                  : _board_graphic(board_graphic), _udp_socet(udp_socet)
{}

void Exporter::push_to_graphic(const QString& message)
{
  qDebug()<<"===push_message"<<message;

  if(message.indexOf(";") > 0)
  {
    QString board_mask = message.mid(0, message.indexOf(";"));
    QString moves_history = message.mid(message.indexOf(";"));

    qDebug()<<"board_mask: "<<board_mask;
    qDebug()<<"moves_history: "<<moves_history;

    _board_graphic->set_moves_history(moves_history);
    _board_graphic->set_board_mask(board_mask);
  }
  else _board_graphic->set_connect_status(message.toInt());
}

void Exporter::push_to_socet(const QString& message)
{
  qDebug()<<"====push_message_to_socet";
  QByteArray m;
  m.append(message);
  _udp_socet->send_data(m);
}

const QString Exporter::pull_from_graphic()
{
  qDebug()<<"====pull_message_from_graphic";
  return _board_graphic->pull_first_command();
}

const QString Exporter::pull_from_socet()
{
  qDebug()<<"====pull_message_from_socet";
  return _udp_socet->pull_received_message();
}
