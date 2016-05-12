#include <QString>
#include <QChar>
#include <QUdpSocket>
#include <QTimer>
#include "udp_server.h"

UDP_server::UDP_server(QObject *parent) : QObject(parent), _SERVER_PORT(1234), _SERVER_IP(QHostAddress::LocalHost)
{
  _timer = new QTimer;
  connect(_timer, SIGNAL(timeout()), this, SLOT(checked_is_message_received()));

  _socket = new QUdpSocket(this);
  _socket->bind(_SERVER_IP, _SERVER_PORT);
  connect(_socket, SIGNAL(readyRead()), this, SLOT(read_data()));

  qDebug()<<"Server start!";
}

void UDP_server::send_data(QByteArray& message, User& u, bool is_prev_serial_need)
{
  if(!u.is_message_reach)
  {
    qDebug()<<"can't send, prev message not reach";
    u.message_stack.push_back(message);
    return;
  }

  add_serial_num(message, u, is_prev_serial_need);

  qDebug()<<"====sending"<<message;
  u.last_sent_message = message;
  _socket->writeDatagram(message, u.ip, u.port);

  begin_wait_receive(u);
}

void UDP_server::send_data(REQUEST_MESSAGES r_mes, User& u, bool is_prev_serial_need)
{
  QByteArray message;
  message.setNum(r_mes);

  if(!u.is_message_reach)
  {
    qDebug()<<"can't send, prev message not reach";
    u.message_stack.push_back(message);
    return;
  }

  add_serial_num(message, u, is_prev_serial_need);

  qDebug()<<"sending"<<message;
  u.last_sent_message = message;
  _socket->writeDatagram(message, u.ip, u.port);

  if(r_mes != MESSAGE_RECEIVED)
    begin_wait_receive(u);
}

void UDP_server::begin_wait_receive(User& u)
{
  qDebug()<<"====begin_wait_receive";
  u.is_message_reach = false;
  _timer->start(SECOND);
}

void UDP_server::checked_is_message_received() // test wariant
{
  qDebug()<<"===time out";

  _timer->stop();
  for(int index = 0; index < _user.size(); ++index)
    if(!_user[index].is_message_reach)
    {
      qDebug()<<"timer restart"<<index;
      _timer->start(SECOND);
      _socket->writeDatagram(_user[index].last_sent_message, _user[index].ip, _user[index].port);
    }
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
    u.is_message_reach = true;
    set_opponent(u);
    
    _user.push_back(u);

    send_data(MESSAGE_RECEIVED, _user[_user.size() - 1]);
    return;
  }

  int sender_index;
  for(int i = 0; i < _user.size(); ++i)
    if(_user[i].port == sender_port && _user[i].ip == sender_IP)
    {
      sender_index = i;
      qDebug()<<"index: "<<i;
      break;
    }

  User *sender = &_user[sender_index];
  if(serial_num.toInt() != ++sender->last_received_serial_num)
  {
    --sender->last_received_serial_num;
    qDebug()<<"wrong serial num";
    if(serial_num.toInt() == sender->last_received_serial_num && buffer.toInt() != MESSAGE_RECEIVED)
    {
      send_data(MESSAGE_RECEIVED, *sender, true);
      qDebug()<<"prev serial num. Resent message";
    }
  }
  else if(buffer.toInt() == MESSAGE_RECEIVED)
  {
    sender->is_message_reach = true;
    if(sender->message_stack.size())
    {
      qDebug()<<"message stack not empty: "<<sender->message_stack[0];
      send_data(sender->message_stack[0], *sender);
      sender->message_stack.remove(0);
    }
    qDebug()<<"buffer.toInt() == MESSAGE_RECEIVED"<<sender_index;
  }
  else
  {
    send_data(MESSAGE_RECEIVED, *sender);

    if(sender->opponent_index != NO_OPPONENT)
      send_data(buffer, _user[sender->opponent_index]);
  }
}

void UDP_server::set_opponent(User& u)
{
  u.opponent_index = NO_OPPONENT;  
  
  for(int i = 0; i < _user.size(); ++i)
    if(_user[i].opponent_index == NO_OPPONENT)
    {  
      u.opponent_index = i;
      _user[i].opponent_index = _user.size();
      qDebug()<<"_user[i].opponent_index: "<<_user[i].opponent_index;
      break;
    }
  qDebug()<<"u.opponent_index: "<<u.opponent_index;
}

void UDP_server::add_serial_num(QByteArray& message, User& u, bool is_prev_serial_need)
{
  if(!is_prev_serial_need)
    ++u.send_serial_num;

  QByteArray serial_num;
  serial_num.setNum(u.send_serial_num);
  serial_num.append(FREE_SPASE);

  message.prepend(serial_num);
}

QByteArray UDP_server::cut_serial_num_from_data(QByteArray& data)
{
  QByteArray serial_num;
  QChar first_data_simbol = QChar(data[0]);
  while(data.size() > 0 && first_data_simbol.isNumber())
  {
    qDebug()<<"====cut_serial_num_from_data; doing while";
    serial_num.append(data[0]);
    data.remove(0,1);
    first_data_simbol = QChar(data[0]);
  }

  while(QChar(data[0]) == FREE_SPASE)
    data.remove(0,1);

  return serial_num;
}
