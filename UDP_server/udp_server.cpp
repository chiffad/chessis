#include <QString>
#include <QChar>
#include <QUdpSocket>
#include "udp_server.h"

UDP_server::UDP_server(QObject *parent) : QObject(parent), _SERVER_PORT(1234), _SERVER_IP(QHostAddress::LocalHost)
{
  _socket = new QUdpSocket(this);
  _socket->bind(_SERVER_IP, _SERVER_PORT);
  connect(_socket, SIGNAL(readyRead()), this, SLOT(read_data()));

  qDebug()<<"Server start!";
}

void UDP_server::send_data(QByteArray message, const int index)
{
  add_serial_num_and_size(message, index);

  qDebug()<<"====sending"<<message;
  _socket->writeDatagram(message, _user[index].ip, _user[index].port);
}

void UDP_server::send_data(REQUEST_MESSAGES r_mes, const int index)
{
  QByteArray message;
  message.setNum(r_mes);
  add_serial_num_and_size(message, index);

  qDebug()<<"sending"<<message;
  _socket->writeDatagram(message, _user[index].ip, _user[index].port);
}

void UDP_server::read_data()
{
  QHostAddress sender_IP;
  quint16 sender_port;
  QByteArray buffer;

  buffer.resize(_socket->pendingDatagramSize());
  _socket->readDatagram(buffer.data(), buffer.size(), &sender_IP, &sender_port);
  qDebug()<<"====reading"<<buffer;

  QByteArray serial_num = cut_data_to_free_spase(buffer);
  QByteArray message_size = cut_data_to_free_spase(buffer);

  if(message_size.toInt() != buffer.size())
  {
    qDebug()<<"not fool size";
    return;
  }

  if(buffer.toInt() == HELLO_SERVER)
  {
    qDebug()<<"HELLO_SERVER";
    User u;
    u.port = sender_port;
    u.ip = sender_IP;
    u.last_received_serial_num = serial_num.toInt();
    u.send_serial_num = 0;
    _user.push_back(u);
    send_data(HELLO_CLIENT, _user.size() - 1);
    return;
  }

  int sender_index = 0;
  for(; _user[sender_index].ip != sender_IP && _user[sender_index].port != sender_port; ++sender_index)
    if(_user.size() <= sender_index)
    {
      qDebug()<<"some crap happened!";
      return;
    }

  if(serial_num.toInt() != ++_user[sender_index].last_received_serial_num)
  {
    --_user[sender_index].last_received_serial_num;
    qDebug()<<"wrong serial num";
    return;
  }

  //send_data(MESSAGE_RECEIVED, sender_index);

  qDebug()<<"received";
  int receiver_index = 0;
  for(; receiver_index != sender_index; ++receiver_index); //fool crap, but for two players work

  qDebug()<<"here1";
  send_data(buffer, receiver_index);
  return;
}

void UDP_server::add_serial_num_and_size(QByteArray& message, const int index)
{
  QByteArray serial_num_and_size;
  serial_num_and_size.setNum(++_user[index].send_serial_num);
  serial_num_and_size.append(FREE_SPASE);

  QByteArray message_size;
  message_size.setNum(message.size());
  message_size.append(FREE_SPASE);

  serial_num_and_size.append(message_size);
  message.prepend(serial_num_and_size);
}

QByteArray UDP_server::cut_data_to_free_spase(QByteArray& message)
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
