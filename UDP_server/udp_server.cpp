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

UDP_server::~UDP_server()
{
  delete _socket;

  for(auto i : _user)
    delete i;
}

UDP_server::User::User(QObject *parent, UDP_server *parent_class, const quint16& port, const QHostAddress& ip,
                       const int received_serial_num, const int index)
                     : QObject(parent), _parent_class(parent_class),_port(port), _ip(ip), _my_index(index),
                       _send_serial_num(0), _received_serial_num(received_serial_num), _is_message_reach(true)
{
  _timer = new QTimer;
  connect(_timer, SIGNAL(timeout()), this, SLOT(timer_timeout()));

  _timer_last_received_message = new QTimer;
  connect(_timer_last_received_message, SIGNAL(timeout()), this, SLOT(timer_last_received_message_timeout()));
}

void UDP_server::send_data(QByteArray& message, User& u)
{ 
  if(!is_message_reach(message, u))
    return;

  add_serial_num(message,u);

  qDebug()<<"====sending"<<message;
  _socket->writeDatagram(message, u._ip, u._port);
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
  _socket->writeDatagram(message, u._ip, u._port);
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
      if(_user[i]->_ip == sender_IP && _user[i]->_port == sender_port)
      {
        qDebug()<<"this client already have";
        goto something_wrong;
      }

    _user.append(new User(this, this, sender_port, sender_IP, serial_num.toInt(), _user.size()));
    set_opponent(*_user[_user.size() - 1]);

    send_data(MESSAGE_RECEIVED, *_user[_user.size() - 1]);
    return;

    something_wrong:
      return;
  }

  int sender_i = 0;
  for(; sender_i < _user.size(); ++sender_i)
    if(_user[sender_i]->_port == sender_port && _user[sender_i]->_ip == sender_IP)
    {
      qDebug()<<"index: "<<sender_i;
      break;
    }

  User *sender = _user[sender_i];
  if(serial_num.toInt() != ++sender->_received_serial_num)
  {
    --sender->_received_serial_num;
    qDebug()<<"wrong serial num";
    if(serial_num.toInt() == sender->_received_serial_num && buffer.toInt() != MESSAGE_RECEIVED)
    {
      sender->_timer_last_received_message->start(TEN_SEC);
      send_data(MESSAGE_RECEIVED, *sender, true);
      qDebug()<<"prev serial num. Resent message";
    }
    return;
  }
  else sender->_timer_last_received_message->start(TEN_SEC);

  switch (buffer.toInt())
  {
    case MESSAGE_RECEIVED:
      qDebug()<<"buffer.toInt() == MESSAGE_RECEIVED"<<sender_i;
      sender->_is_message_reach = true;
      if(sender->_message_stack.size())
      {
        qDebug()<<"message stack not empty: "<<sender->_message_stack[0];
        send_data(sender->_message_stack[0], *sender);
        sender->_message_stack.remove(0);
      }
      break;

    case SERVER_LOST:
      send_data(MESSAGE_RECEIVED, *sender);
      break;

    default:
      send_data(MESSAGE_RECEIVED, *sender);
      if(sender->_opponent_index != NO_OPPONENT)
        send_data(buffer, *_user[sender->_opponent_index]);
  }
}

void UDP_server::set_opponent(User& u)
{
  u._opponent_index = NO_OPPONENT;
  
  for(int i = 0; i < _user.size() - 1; ++i)
    if(_user[i]->_opponent_index == NO_OPPONENT)
    {  
      u._opponent_index = i;
      _user[i]->_opponent_index = _user.size() - 1;
      qDebug()<<"_user[i].opponent_index: "<<_user[i]->_opponent_index;
      break;
    }
  qDebug()<<"u.opponent_index: "<<u._opponent_index;
}

void UDP_server::begin_wait_receive(User& u)
{
  qDebug()<<"====begin_wait_receive";
  u._is_message_reach = false;
  u._timer->start(SECOND);
}

void UDP_server::add_serial_num(QByteArray& message, User& u, bool is_prev_serial_need)
{
  if(!is_prev_serial_need)
    ++u._send_serial_num;

  QByteArray serial_num;
  serial_num.setNum(u._send_serial_num);
  serial_num.append(FREE_SPASE);

  message.prepend(serial_num);
}

QByteArray UDP_server::cut_serial_num_from_data(QByteArray& data) const
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

void UDP_server::User::timer_timeout() // test wariant
{
  qDebug()<<"===time out";

  if(!_is_message_reach)
  {
    if(_last_sent_message.toInt() == CLIENT_LOST)
    {
      qDebug()<<"last message was client lost";
      /*_timer->stop();
      _user[_opponent_index]._opponent_index = NO_OPPONENT;
      _user.remove(i);*/
      return;
    }
    _timer->start(SECOND);

    _parent_class->add_serial_num(_last_sent_message, *_parent_class->_user[_my_index], true);
    _parent_class->_socket->writeDatagram(_last_sent_message, _ip, _port);
  }
  else
  {
    qDebug()<<"timer stop";
    _timer->stop();
  }
}

void UDP_server::User::timer_last_received_message_timeout()
{
  qDebug()<<"===timer_from_last_received_message_timeout "<<_my_index;

  _timer_last_received_message->stop();
  _parent_class->send_data(CLIENT_LOST, *_parent_class->_user[_my_index]);
}

bool UDP_server::is_message_reach(QByteArray& message, User& u)
{
  if(!u._is_message_reach)
  {
    qDebug()<<"can't send, prev message not reach";
    u._message_stack.push_back(message);
    return false;
  }

  u._last_sent_message = message;
  return true;
}
