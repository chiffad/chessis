#include <QObject>
#include <QUdpSocket>
#include "headers/udp_client.h"

UDP_client::UDP_client(QObject *parent) : QObject(parent), SERVER_PORT(1234), SERVER_IP(QHostAddress::LocalHost)
{
  _socket = new QUdpSocket(this);
  _socket->bind(SERVER_IP, SERVER_PORT);
  connect(_socket, SIGNAL(readyRead()), this, SLOT(read_data()));

  send_data(HELLO_SERVER);
}

void UDP_client::send_data(QByteArray message)
{
  qDebug()<<"====Sending data to server====";
  qDebug()<<message;
  _socket->writeDatagram(message, SERVER_IP, SERVER_PORT);
}

void UDP_client::send_data(REQUEST_MESSAGES r_mes)
{
  QByteArray message;
  message.setNum(r_mes);
  _socket->writeDatagram(message, SERVER_IP, SERVER_PORT);
}

void UDP_client::read_data()
{
  _data.resize(_socket->pendingDatagramSize());

  qDebug()<<"=====socket read";
  _socket->readDatagram(_data.data(), _data.size());

}

void UDP_client::export_readed_data_to_chess(QString& message)
{
  message.clear();
  message.append(_data);
}
