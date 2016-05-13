#include <QString>
#include <QChar>
#include <QUdpSocket>
#include <QTimer>
#include "udp_server.h"

UDP_server::UDP_server(QObject *parent) : QObject(parent), _SERVER_PORT(1234), _SERVER_IP(QHostAddress::LocalHost)
{
  _socket = new QUdpSocket(this);
  _socket->bind(_SERVER_IP, _SERVER_PORT);
  connect(_socket, SIGNAL(readyRead()), this, SLOT(read_data()));

  qDebug()<<"Server start!";
}

void UDP_server::send_data(QByteArray& message, User& u)
{ 
  if(!is_message_reach(message, u))
    return;

  add_serial_num(message,u);

  qDebug()<<"====sending"<<message;
  _socket->writeDatagram(message, u.ip, u.port);
  begin_wait_receive(u);
}

void UDP_server::send_data(REQUEST_MESSAGES r_mes, User& u, bool is_prev_serial_need)
{
  QByteArray message;
  message.setNum(r_mes);

  if(r_mes != MESSAGE_RECEIVED)
    if(!is_message_reach(message, u))
      return;

  add_serial_num(message, u, is_prev_serial_need);

  qDebug()<<"====sending"<<message;
  _socket->writeDatagram(message, u.ip, u.port);
  if(r_mes != MESSAGE_RECEIVED)
    begin_wait_receive(u);
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

    _user.resize(_user.size() + 1);
    User *new_user = &_user[_user.size() - 1];
    new_user->port = sender_port;
    new_user->ip = sender_IP;
    new_user->received_serial_num = serial_num.toInt();
    new_user->send_serial_num = 0;
    new_user->is_message_reach = true;
    new_user->timer = new QTimer(this);
    connect(new_user->timer, SIGNAL(timeout()), this, SLOT(timer_timeout()));
    new_user->timer_last_received_message = new QTimer(this);
    connect(new_user->timer_last_received_message, SIGNAL(timeout()), this, SLOT(timer_last_received_message_timeout()));
    set_opponent(*new_user);

    send_data(MESSAGE_RECEIVED, *new_user);
    return;
  }

  int sender_i;
  for(sender_i = 0; sender_i < _user.size(); ++sender_i)
    if(_user[sender_i].port == sender_port && _user[sender_i].ip == sender_IP)
    {
      qDebug()<<"index: "<<sender_i;
      break;
    }

  User *sender = &_user[sender_i];
  if(serial_num.toInt() != ++sender->received_serial_num)
  {
    --sender->received_serial_num;
    qDebug()<<"wrong serial num";
    if(serial_num.toInt() == sender->received_serial_num && buffer.toInt() != MESSAGE_RECEIVED)
    {
      sender->timer_last_received_message->start(TEN_SEC);
      send_data(MESSAGE_RECEIVED, *sender, true);
      qDebug()<<"prev serial num. Resent message";
    }
    return;
  }
  else sender->timer_last_received_message->start(TEN_SEC);

  switch (buffer.toInt())
  {
    case MESSAGE_RECEIVED:
      qDebug()<<"buffer.toInt() == MESSAGE_RECEIVED"<<sender_i;
      sender->is_message_reach = true;
      if(sender->message_stack.size())
      {
        qDebug()<<"message stack not empty: "<<sender->message_stack[0];
        send_data(sender->message_stack[0], *sender);
        sender->message_stack.remove(0);
      }
      break;

    case SERVER_LOST:
      send_data(MESSAGE_RECEIVED, *sender);
      break;

    default:
      send_data(MESSAGE_RECEIVED, *sender);
      if(sender->opponent_index != NO_OPPONENT)
        send_data(buffer, _user[sender->opponent_index]);
  }
}

void UDP_server::set_opponent(User& u)
{
  u.opponent_index = NO_OPPONENT;  
  
  for(int i = 0; i < _user.size() - 1; ++i)
    if(_user[i].opponent_index == NO_OPPONENT)
    {  
      u.opponent_index = i;
      _user[i].opponent_index = _user.size() - 1;
      qDebug()<<"_user[i].opponent_index: "<<_user[i].opponent_index;
      break;
    }
  qDebug()<<"u.opponent_index: "<<u.opponent_index;
}

void UDP_server::begin_wait_receive(User& u)
{
  qDebug()<<"====begin_wait_receive";
  u.is_message_reach = false;
  u.timer->start(SECOND);
}

void UDP_server::timer_timeout() // test wariant
{
  qDebug()<<"===time out";

  for(int i = 0; i < _user.size(); ++i)
    if(_user[i].timer->remainingTime())
    {
      if(!_user[i].is_message_reach)
      {
        if(_user[i].last_sent_message.toInt() == CLIENT_LOST)
        {
          qDebug()<<"last message was client lost";
          /*_user[i].timer->stop();
          _user[_user[i].opponent_index].opponent_index = NO_OPPONENT;
          _user.remove(i);*/
          return;
        }
        qDebug()<<"timer restart"<<i;
        _user[i].timer->start(SECOND);

        QByteArray m = _user[i].last_sent_message;
        add_serial_num(m,_user[i], true);
        _socket->writeDatagram(m, _user[i].ip, _user[i].port);
      }
      else
      {
        qDebug()<<"timer stop";
        _user[i].timer->stop();
      }
    }
}

void UDP_server::timer_last_received_message_timeout()
{
  qDebug()<<"===timer_from_last_received_message_timeout";

  for(int i = 0; i < _user.size(); ++i)
    if(_user[i].timer_last_received_message->remainingTime())
    {
      qDebug()<<"user: "<<i;
      _user[i].timer_last_received_message->stop();
      send_data(CLIENT_LOST,_user[i]);
    }
}

bool UDP_server::is_message_reach(QByteArray& message, User& u)
{
  if(!u.is_message_reach)
  {
    qDebug()<<"can't send, prev message not reach";
    u.message_stack.push_back(message);
    return false;
  }

  u.last_sent_message = message;
  return true;
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
    qDebug()<<"====cut_serial_num_from_data";
    serial_num.append(data[0]);
    data.remove(0,1);
    first_data_simbol = QChar(data[0]);
  }

  while(QChar(data[0]) == FREE_SPASE)
    data.remove(0,1);

  return serial_num;
}
