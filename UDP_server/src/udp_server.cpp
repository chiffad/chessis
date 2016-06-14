#include <QString>
#include <QChar>
#include <QUdpSocket>
#include <QTimer>
#include "headers/udp_server.h"
#include "headers/chess.h"

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

  for(auto i : _board)
    delete i;
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
  QByteArray message;

  message.resize(_socket->pendingDatagramSize());
  _socket->readDatagram(message.data(), message.size(), &sender_IP, &sender_port);
  qDebug()<<"====reading"<<message;

  QByteArray serial_num = cut_serial_num_from_data(message);

  int sender_i = 0;
  for(; sender_i < _user.size(); ++sender_i)
    if(_user[sender_i]->_port == sender_port && _user[sender_i]->_ip == sender_IP)
    {
      qDebug()<<"index: "<<sender_i;
      break;
    }

  if(message.toInt() == HELLO_SERVER)
  {
    qDebug()<<"HELLO_SERVER";
    if(sender_i < _user.size())
    {
      qDebug()<<"this client already have";
      return;
    }

    _user.append(new User(this, this, sender_port, sender_IP, serial_num.toInt(), _user.size()));
    set_opponent(*_user[sender_i]);

    send_data(MESSAGE_RECEIVED, *_user.last());
    return;
  }

  if(serial_num.toInt() != ++_user[sender_i]->_received_serial_num)
  {
    --_user[sender_i]->_received_serial_num;
    qDebug()<<"wrong serial num";
    if(serial_num.toInt() == _user[sender_i]->_received_serial_num && message.toInt() != MESSAGE_RECEIVED)
    {
      _user[sender_i]->_timer_last_received_message->start(TEN_SEC);
      send_data(MESSAGE_RECEIVED, *_user[sender_i], true);
      qDebug()<<"prev serial num. Resent message";
    }
    return;
  }
  else
  {
    _user[sender_i]->_timer_last_received_message->start(TEN_SEC);
    run_message(message, *_user[sender_i]);
  }
}

void UDP_server::run_message(const QByteArray& message, User& u)
{
  QByteArray message_type;
  QByteArray message_content = message;
  while(message_content.size() && QChar(message_content[0]) != FREE_SPASE)
  {
    message_type.append(message_content[0]);
    message_content.remove(0,1);
  }

  bool is_to_board = false;
  switch(message_type.toInt())
  {
    case MESSAGE_RECEIVED:
      qDebug()<<"message_type == MESSAGE_RECEIVED";
      u._is_message_reach = true;
      if(u._message_stack.size())
      {
        qDebug()<<"message stack not empty: "<<u._message_stack[0];
        send_data(u._message_stack[0], u);
        u._message_stack.remove(0);
      }
      break;
    case IS_SERVER_LOST:
      break;
    case OPPONENT_INF:
      show_information(u);
      break;
    case MY_INF:
      show_information(u, false);
      break;
    case MOVE:
      is_to_board = true;
      break;
    case BACK_MOVE:
    //  back_move();
      is_to_board = true;
      break;
    case NEW_GAME:
    //  start_new_game();
      is_to_board = true;
      break;
    case GO_TO_HISTORY:
     // go_to_history_index(message.toInt());
      is_to_board = true;
      break;
    default:
      qDebug()<<"sheet message!";
  }

  /*if(u._opponent_index != NO_OPPONENT && is_to_board)
    send_data(message, *_user[u._opponent_index]);*/

  if(message_type.toInt() != MESSAGE_RECEIVED)
    send_data(MESSAGE_RECEIVED, u);
}

void UDP_server::show_information(const User& u, bool is_to_opponent)
{
  QByteArray inf;
  int receiver = u._my_index;
  if(is_to_opponent && u._opponent_index == NO_OPPONENT)
     inf.append("No opponent!");
  else
  {
    if(is_to_opponent)
      receiver = u._opponent_index;
    inf.append(" Login: " + _user[receiver]->_login + "; Rating ELO: "
           + QByteArray::number(_user[receiver]->_rating_ELO));
  }
  send_data(inf, *_user[receiver]);
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
      _board.append(new Desk(u._my_index, u._opponent_index));
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

void UDP_server::User::timer_timeout()
{
  qDebug()<<"===time out";

  static int count = 0;
  if(!_is_message_reach)
  {
    ++count;
    if(_last_sent_message.toInt() == CLIENT_LOST || count > 5)
    {
      qDebug()<<"last message was client lost";
      _parent_class->send_data(OPPONENT_LOST_FROM_SERVER, *_parent_class->_user[_opponent_index]);
    }
    _timer->start(SECOND);

    _parent_class->add_serial_num(_last_sent_message, *_parent_class->_user[_my_index], true);
    _parent_class->_socket->writeDatagram(_last_sent_message, _ip, _port);
  }
  else
  {
    qDebug()<<"timer stop";
    _timer->stop();
    count = 0;
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

UDP_server::User::User(QObject *parent, UDP_server *parent_class, const quint16& port, const QHostAddress& ip,
                       const int received_serial_num, const int index)
                     : QObject(parent), _parent_class(parent_class),_port(port), _ip(ip), _my_index(index),
                       _received_serial_num(received_serial_num), _send_serial_num(0), _is_message_reach(true)
{
  _login = "login";
  _rating_ELO = 1200;
  _timer = new QTimer;
  connect(_timer, SIGNAL(timeout()), this, SLOT(timer_timeout()));

  _timer_last_received_message = new QTimer;
  connect(_timer_last_received_message, SIGNAL(timeout()), this, SLOT(timer_last_received_message_timeout()));
}

int UDP_server::User::get_desk_ind()
{
  if(_opponent_index == NO_OPPONENT)
    return NO_OPPONENT;

  int i = 0;
  for(; i < _parent_class->_board.size(); ++i)
    if(_parent_class->_board[i]->_first_player_ind == _my_index
       && _parent_class->_board[i]->_second_player_ind == _opponent_index)
      break;

  return i;
}
