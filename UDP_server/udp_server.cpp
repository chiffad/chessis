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
  _socket->writeDatagram(message, addres, port);
}

void UDP_server::read_data()
{
  const QString FIRST_CONECTION = "Hello, server!";
  QHostAddress sender_IP;
  quint16 sender_port;
  QByteArray buffer;

  buffer.resize(_socket->pendingDatagramSize());
  _socket->readDatagram(buffer.data(), buffer.size(), &sender_IP, &sender_port);

  if(buffer == FIRST_CONECTION)
  {
    _user_port.push_back(sender_port);
    _user_addres.push_back(sender_IP);
  }

  else if(_user_port.size() > 1)
  {
    if(_user_port[0] == sender_port)
      send_data(_user_port[1], _user_addres[1], buffer);

    else send_data(_user_port[0], _user_addres[0], buffer);
  }
}
