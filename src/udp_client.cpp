#include <QObject>
#include <QUdpSocket>
#include "headers/udp_client.h"

UDP_client::UDP_client(QObject *parent) : QObject(parent), SERVER_PORT(1234), SERVER_IP(QHostAddress::LocalHost)
{
  _last_received_serial_num = 0;
  _serial_num = 0;
  _socket = new QUdpSocket(this);
  _socket->bind(SERVER_IP, SERVER_PORT);
  connect(_socket, SIGNAL(readyRead()), this, SLOT(read_data()));

  send_data(HELLO_SERVER);
}

void UDP_client::send_data(QByteArray message)
{
  add_serial_num_and_size(message);
  qDebug()<<"====Sending data to server===="<<message;
  _socket->writeDatagram(message, SERVER_IP, SERVER_PORT);
}

void UDP_client::send_data(REQUEST_MESSAGES r_mes)
{
  QByteArray message;
  message.setNum(r_mes);
  add_serial_num_and_size(message);
  qDebug()<<"====Sending data to server===="<<message;
  _socket->writeDatagram(message, SERVER_IP, SERVER_PORT);
}

void UDP_client::read_data()
{
  _data.resize(_socket->pendingDatagramSize());
  _socket->readDatagram(_data.data(), _data.size());

  qDebug()<<"=====socket read"<<_data;
  QByteArray serial_num = cut_data_to_free_spase(_data);
  QByteArray message_size = cut_data_to_free_spase(_data);

  if(serial_num.toInt() != ++_last_received_serial_num || message_size.toInt() != _data.size())
  {
    qDebug()<<"==========some wrong data";
    --_last_received_serial_num;
    return;
  }
  else
  {
    qDebug()<<"received";
    //send_data(MESSAGE_RECEIVED);
    emit some_data_came();
  }
}

void UDP_client::export_readed_data_to_chess(QString& message)
{
  message.clear();
  message.append(_data);
}

void UDP_client::add_serial_num_and_size(QByteArray& message)
{
  QByteArray serial_num_and_size;
  serial_num_and_size.setNum(++_serial_num);
  serial_num_and_size.append(FREE_SPASE);

  QByteArray message_size;
  message_size.setNum(message.size());
  message_size.append(FREE_SPASE);

  serial_num_and_size.append(message_size);
  message.prepend(serial_num_and_size);
}

QByteArray UDP_client::cut_data_to_free_spase(QByteArray& message)
{
  QByteArray serial_num;
  while(message.size() > 0 && QChar(message[0]) != FREE_SPASE)
  {
    serial_num.append(message[0]);
    message.remove(0,1);
  }

  while(QChar(message[0]) == FREE_SPASE)
    message.remove(0,1);

  return serial_num;
}
