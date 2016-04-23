#include <QObject>
#include <QUdpSocket>
#include "headers/udp_client.h"

UDP_client::UDP_client(QObject *parent) : QObject(parent)
{
  _socket = new QUdpSocket(this);
  _socket->bind(_SERVER_IP, _SERVER_PORT);

  connect(_socket, SIGNAL(readyRead()), this, SLOT(read_data()));
  create_connection();
}

void UDP_client::create_connection()
{
  qDebug()<<"====Create connection====";

  QByteArray create_connection;
  create_connection.append("Hello, server!");
  _socket->writeDatagram(create_connection, _SERVER_IP, _SERVER_PORT);
}

void UDP_client::send_data(const QByteArray& message)
{
  qDebug()<<"====Sending data to server====";
  _socket->writeDatagram(message, _SERVER_IP, _SERVER_PORT);
}

void UDP_client::read_data()
{
  _data.resize(_socket->pendingDatagramSize());

  QHostAddress sender;
  quint16 senderPort;

  _socket->readDatagram(_data.data(), _data.size(), &sender, &senderPort);

  qDebug()<<"======read========";
  qDebug()<<"Message from: "<<sender.toString();
  qDebug()<<"Message port: "<<senderPort;
  qDebug()<<"Message: "<<_data;
  qDebug()<<"==================";
  emit some_data_came();
}

void UDP_client::export_readed_data_to_chess(QString& move)
{
  move.clear();
  move.append(_data);
}
