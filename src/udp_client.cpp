#include <QObject>
#include <QUdpSocket>
#include "headers/udp_client.h"

UDP_client::UDP_client(QObject *parent) : QObject(parent)
{
 // _SERVER_PORT = 1234;
 // _SERVER_IP = QHostAddress::LocalHost;
  _socket = new QUdpSocket(this);
  _socket->bind(_SERVER_IP, _SERVER_PORT);

  connect(_socket, SIGNAL(readyRead()), this, SLOT(read_data()));

  QByteArray create_connection;
  create_connection.append("Hello, server!");
  send_data(create_connection);
}

void UDP_client::send_data(const QByteArray& message)
{
  if(_socket->isValid())//_socket->isWritable()
    _socket->writeDatagram(message, _SERVER_IP, _SERVER_PORT);
  else
  {
    qDebug()<<"error: "<<_socket->error();
    qDebug()<<"====Sending data to server====";
  }
}

void UDP_client::read_data()
{
  _data.resize(_socket->pendingDatagramSize());

  QHostAddress sender;
  quint16 senderPort;

  _socket->readDatagram(_data.data(), _data.size(), &sender, &senderPort);
  emit some_data_came();
}

void UDP_client::export_readed_data_to_chess(QString& move)
{
  move.clear();
  move.append(_data);
}
