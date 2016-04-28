#include <QString>
#include <QChar>
#include <QUdpSocket>
#include "udp_server.h"

UDP_server::UDP_server(QObject *parent) : QObject(parent)
{
  _server_port = 1234;
  _server_IP = QHostAddress::LocalHost;
  _socket = new QUdpSocket(this);

  _socket->bind(_server_IP, _server_port);
  connect(_socket, SIGNAL(readyRead()), this, SLOT(read_data()));

  qDebug()<<"Server start!";
}

void UDP_server::send_data(const quint16 port, const QHostAddress& addres, const QByteArray& message)
{
    qDebug()<<"sending"<<message;
  _socket->writeDatagram(message, addres, port);
}

void UDP_server::send_data(const quint16 port, const QHostAddress& addres, REQUEST_MESSAGES r_mes)
{
  QByteArray message;
  message.setNum(r_mes);
  qDebug()<<"sending"<<message;
  _socket->writeDatagram(message, addres, port);
}

void UDP_server::read_data()
{
  QHostAddress sender_IP;
  quint16 sender_port;
  QByteArray buffer;

  buffer.resize(_socket->pendingDatagramSize());
  _socket->readDatagram(buffer.data(), buffer.size(), &sender_IP, &sender_port);
  qDebug()<<"reading"<<buffer;
  //qDebug()<<"to int"<<buffer[2]<<buffer[2];

  if(buffer.size() > 1)
  {
    qDebug()<<"here1";
    if(_user_port[0] == sender_port)
      send_data(_user_port[1], _user_addres[1], buffer);
    else send_data(_user_port[0], _user_addres[0], buffer);
    return;
  }

  switch(buffer.toInt())
  {
    case HELLO_SERVER:
      qDebug()<<"HELLO_SERVER";
      _user_port.push_back(sender_port);
      _user_addres.push_back(sender_IP);
      send_data(sender_port, sender_IP, HELLO_CLIENT);
      break;
    case IS_SERVER_WORKING:
      qDebug()<<"is server work";
      send_data(sender_port, sender_IP, HELLO_CLIENT);
      break;
    case IS_HAVE_OPPONENT:
      qDebug()<<"is have opp";
      if(_user_port.size() < 1)
        send_data(sender_port, sender_IP, HAVENT_OPPONENT);
      else send_data(sender_port, sender_IP, HAVE_OPPONENT);
      break;
    default:
      qDebug()<<"here";
      break;
  }
}
