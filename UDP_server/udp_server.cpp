#include <QString>
#include <QChar>
#include <QUdpSocket>
#include <QTimer>
#include "udp_server.h"

UDP_server::UDP_server(QObject *parent) : QObject(parent), _SERVER_PORT(1234), _SERVER_IP(QHostAddress::LocalHost)
{
   _timer = new QTimer;

  _socket = new QUdpSocket(this);
  _socket->bind(_SERVER_IP, _SERVER_PORT);
  connect(_socket, SIGNAL(readyRead()), this, SLOT(read_data()));

  qDebug()<<"Server start!";
  //begin_wait_confirm();
}

void UDP_server::begin_wait_confirm(const int i)
{
  _user[i].is_message_reach = false;
  connect(_timer, SIGNAL(timeout()), this, SLOT(repeat_message()));
  _timer->start(SECOND);
}

void UDP_server::repeat_message() // test wariant
{
  _timer->stop();
  qDebug()<<"time out";
  for(int index = 0; index < _user.size(); ++index)
  {
    if(!_user[index].is_message_reach)
    {
      qDebug()<<"timer stoped";
      _timer->start();
      _socket->writeDatagram(_user[index].last_sent_message, _user[index].ip, _user[index].port);
    }
    else _user[index].is_message_reach = false;
  }
}

void UDP_server::send_data(QByteArray message, const int i)
{
  add_serial_num(message, i);

  qDebug()<<"====sending"<<message;
  _user[i].last_sent_message = message;
  _socket->writeDatagram(message, _user[i].ip, _user[i].port);

  begin_wait_confirm(i);
}

void UDP_server::send_data(REQUEST_MESSAGES r_mes, const int i)
{
  QByteArray message;
  message.setNum(r_mes);
  add_serial_num(message, i);

  qDebug()<<"sending"<<message;
  _user[i].last_sent_message = message;
  _socket->writeDatagram(message, _user[i].ip, _user[i].port);
  if(r_mes != MESSAGE_RECEIVED)
    begin_wait_confirm(i);
}

void UDP_server::read_data()
{
  QHostAddress sender_IP;
  quint16 sender_port;
  QByteArray buffer;

  buffer.resize(_socket->pendingDatagramSize());
  _socket->readDatagram(buffer.data(), buffer.size(), &sender_IP, &sender_port);
  qDebug()<<"====reading"<<buffer;

  QByteArray serial_num = cut_serial_num_from_data(buffer);

  if(buffer.toInt() == HELLO_SERVER)
  {
    qDebug()<<"HELLO_SERVER";
    for(int i = 0; i < _user.size(); ++i)
      if(_user[i].ip == sender_IP && _user[i].port == sender_port)
      {
        qDebug()<<"this client already have";
        return;
      }
    User u;
    u.port = sender_port;
    u.ip = sender_IP;
    u.last_received_serial_num = serial_num.toInt();
    u.send_serial_num = 0;
    _user.push_back(u);
    return;
  }

  int sender_index;
  if(_user[0].port == sender_port)//fool crap
     sender_index = 0;
  else sender_index = 1;

  User *sender = &_user[sender_index];
  if(serial_num.toInt() != ++sender->last_received_serial_num)
  {
    --sender->last_received_serial_num;
    if(serial_num.toInt() == sender->last_received_serial_num && buffer.toInt() == MESSAGE_RECEIVED)
      _socket->writeDatagram(sender->last_sent_message, sender->ip, sender->port);
    qDebug()<<"wrong serial num";
    return;
  }
  else
  {
    if(buffer.toInt() == MESSAGE_RECEIVED)
      return;
    else send_data(MESSAGE_RECEIVED, sender_index);
  }

  int receiver_index;
  if(sender_index)  //fool crap, but for two players work
    receiver_index = 0;
  else receiver_index = 1;

  send_data(buffer, receiver_index);
}

void UDP_server::add_serial_num(QByteArray& message, const int i)
{
  QByteArray serial_num;
  serial_num.setNum(++_user[i].send_serial_num);
  serial_num.append(FREE_SPASE);

  message.prepend(serial_num);
}

QByteArray UDP_server::cut_serial_num_from_data(QByteArray& data)
{
  QByteArray serial_num;
  while(data.size() > 0 && QChar(data[0]) != FREE_SPASE)
  {
    serial_num.append(data[0]);
    data.remove(0,1);
  }

  while(QChar(data[0]) == FREE_SPASE)
    data.remove(0,1);

  return serial_num;
}
