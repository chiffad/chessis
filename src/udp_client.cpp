#include <QObject>
#include <QUdpSocket>
#include "headers/udp_client.h"

UDP_client::UDP_client(QObject *parent) : QObject(parent)
{
  _socket = new QUdpSocket(this);
  _socket->bind(_SERVER_IP, _SERVER_PORT);

  connect(_socket, SIGNAL(readyRead()), this, SLOT(read_data()));
  if(_socket->waitForConnected(1000))
  {
    qDebug()<<"Connected to server! ( is writable? : "<<_socket->isWritable()<<")";
    QByteArray create_connection;
    create_connection.append("Hello, server!");
    send_data(create_connection);
  }
  else qDebug()<<"Can't connected to server! "<<_socket->error();
}

void UDP_client::send_data(const QByteArray& message)
{
  qDebug()<<"====Sending data to server====";
  if(_socket->isValid())//_socket->isWritable()
    _socket->writeDatagram(message, _SERVER_IP, _SERVER_PORT);
  else qDebug()<<"error: "<<_socket->error();
  qDebug()<<"====Sending data to server====";
}

void UDP_client::read_data()
{
  qDebug()<<"======read========";
  _data.resize(_socket->pendingDatagramSize());

  QHostAddress sender;
  quint16 senderPort;

  if(_socket->isReadable()) //_socket->isValid()
    _socket->readDatagram(_data.data(), _data.size(), &sender, &senderPort);
  qDebug()<<"is readable? : "<<_socket->isReadable();

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
