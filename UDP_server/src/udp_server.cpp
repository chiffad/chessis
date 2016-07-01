#include <QString>
#include <QChar>
#include <QUdpSocket>
#include <QTimer>
#include "headers/udp_server.h"
#include "headers/chess.h"
#include "headers/enums.h"

#include <iostream>
#include <vector>
#include <string>

UDP_server::UDP_server(QObject *parent) : QObject(parent), _SERVER_PORT(12345), _SERVER_IP(QHostAddress::LocalHost),
                                          _socket(new QUdpSocket(this))
{
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

void UDP_server::send_data(QByteArray message, User& u)
{
  if(!is_message_reach(message, u))
    return;

  add_serial_num(message,u);

  qDebug()<<"====sending"<<message;
  _socket->writeDatagram(message, u._ip, u._port);
  begin_wait_receive(u);
}

void UDP_server::send_data(const Messages::MESSAGE r_mes, User& u, bool is_prev_serial_need)
{
  QByteArray message;
  message.setNum(r_mes);

  if(r_mes != Messages::MESSAGE_RECEIVED)
  {
    if(!is_message_reach(message, u))
      return;

    begin_wait_receive(u);
  }

  add_serial_num(message, u, is_prev_serial_need);

  qDebug()<<"====sending"<<message;
  _socket->writeDatagram(message, u._ip, u._port);
}

void UDP_server::read_data()
{
  QHostAddress sender_IP;
  quint16 sender_port;
  QByteArray message;

  message.resize(_socket->pendingDatagramSize());
  _socket->readDatagram(message.data(), message.size(), &sender_IP, &sender_port);
  qDebug()<<"====reading"<<message;

  int sender = 0;
  for(; sender < _user.size(); ++sender)
    if(_user[sender]->_port == sender_port && _user[sender]->_ip == sender_IP)
      break;

  const int serial_num = cut_serial_num(message);

  if(message.toInt() == Messages::HELLO_SERVER)
  {
    qDebug()<<"HELLO_SERVER";
    if(sender < _user.size())
    {
      qDebug()<<"this client already have";
      return;
    }

    _user.append(new User(this, this, sender_port, sender_IP, serial_num, _user.size()));

    send_data(Messages::MESSAGE_RECEIVED, *_user.last());
    set_opponent(*_user[sender]);
  }
  else if(serial_num != ++_user[sender]->_received_serial_num)
  {
    qDebug()<<"wrong serial num";
    --_user[sender]->_received_serial_num;

    if(serial_num == _user[sender]->_received_serial_num && message.toInt() != Messages::MESSAGE_RECEIVED)
    {
      _user[sender]->_timer_last_received_message->start(TEN_SEC);
      send_data(Messages::MESSAGE_RECEIVED, *_user[sender], true);
      qDebug()<<"prev serial num. Resent message";
    }
  }
  else run_message(message, *_user[sender]);
}

void UDP_server::run_message(const QByteArray &message, User& u)
{
  qDebug()<<"===run_message";

  if(message.toInt() != Messages::MESSAGE_RECEIVED)
    send_data(Messages::MESSAGE_RECEIVED, u);

  switch(message.toInt())
  {
    case Messages::MESSAGE_RECEIVED:
      qDebug()<<"message_type == MESSAGE_RECEIVED";
      u._is_message_reach = true;
      if(!u._message_stack.isEmpty())
      {
        qDebug()<<"message stack not empty: "<<u._message_stack[0];
        send_data(u._message_stack[0], u);
        u._message_stack.remove(0);
      }
      break;
    case Messages::IS_SERVER_LOST:
      break;
    case Messages::OPPONENT_INF:
      show_information(u);
      break;
    case Messages::MY_INF:
      show_information(u, false);
      break;
    default:
      push_message_to_logic(message, u);
  }
  u._timer_last_received_message->start(TEN_SEC);
}

void UDP_server::push_message_to_logic(const QByteArray &message, User& u)
{
  qDebug()<<"===push_message_to_logic";
  if(u.get_board_ind() == NO_OPPONENT)
    return;

  const QByteArray message_type(message.mid(0, message.indexOf(FREE_SPASE) - 1));
  const QByteArray message_content(message.mid(message.indexOf(FREE_SPASE) + 1));

  Desk *const board = _board[u.get_board_ind()];
  switch(message_type.toInt())
  {
    case Messages::MOVE:
    {
      qDebug()<<"MOVE";
      std::string str(message_content.constData(), message_content.length());
      board->make_moves_from_str(str);
      break;
    }
    case Messages::BACK_MOVE:
      qDebug()<<"BACK_MOVE";
      board->back_move();
      break;
    case Messages::NEW_GAME:
      qDebug()<<"NEW_GAME";
      board->start_new_game();
      break;
    case Messages::GO_TO_HISTORY:
      qDebug()<<"GO_TO_HISTORY: "<<message_content.toInt();
      board->go_to_history_index(message_content.toInt());
      break;
    default:
      qDebug()<<"sheet message!";
      return;
  }
  send_board_state(u);
}

void UDP_server::send_board_state(User& u)
{
  qDebug()<<"====send_board_state";

  QByteArray message;
  Desk *const board = _board[u.get_board_ind()];

  message.append(QString::fromStdString(board->get_board_mask()));
  message.push_back(";");

  message.append(QString::fromStdString(board->get_moves_history()));
  if(board->is_mate())
    message.append("#");
  message.push_back(";");

  message.append(QByteArray::number(board->get_actual_move()));

  send_data(message, *_user[u._opponent_index]);
  send_data(message, u);
}

void UDP_server::show_information(User& u, bool is_opponent)
{
  QByteArray inf;
  if(is_opponent && u._opponent_index == NO_OPPONENT)
     inf.append("No opponent!");
  else
  {
    const int WHOSE_INF = is_opponent ? u._opponent_index : u._my_index;
    inf.append(" Login: " + _user[WHOSE_INF]->_login + "; Rating ELO: "
           + QByteArray::number(_user[WHOSE_INF]->_rating_ELO));
  }
  send_data(inf, u);
}

void UDP_server::set_opponent(User& u)
{
  u._opponent_index = NO_OPPONENT;

  for(int i = 0; i < _user.size(); ++i)
    if(_user[i]->_opponent_index == NO_OPPONENT && u._my_index != i)
    {
      u._opponent_index = i;
      _user[i]->_opponent_index = u._my_index;
      _board.append(new Desk(u._my_index, u._opponent_index));
      send_board_state(u);
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

int UDP_server::cut_serial_num(QByteArray& data) const
{
  QByteArray serial_num(data.mid(0, data.indexOf(FREE_SPASE)));
  data.remove(0, data.indexOf(FREE_SPASE) + 1);

  return serial_num.toInt();
}

bool UDP_server::is_message_reach(const QByteArray& message, User& u)
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
                     : QObject(parent), _timer(new QTimer), _timer_last_received_message(new QTimer),
                       _parent_class(parent_class), _port(port), _ip(ip), _my_index(index),
                       _received_serial_num(received_serial_num), _send_serial_num(0), _is_message_reach(true),
                       _login("login"), _rating_ELO(1200)
{
  connect(_timer, SIGNAL(timeout()), this, SLOT(timer_timeout()));
  connect(_timer_last_received_message, SIGNAL(timeout()), this, SLOT(timer_last_received_message_timeout()));
  _timer_last_received_message->start(TEN_SEC);
}

int UDP_server::User::get_board_ind()
{
  if(_opponent_index == NO_OPPONENT)
    return NO_OPPONENT;

  int i = 0;
  for(; i < _parent_class->_board.size(); ++i)
    if((_parent_class->_board[i]->_first_player_ind == _my_index
        && _parent_class->_board[i]->_second_player_ind == _opponent_index)
       || (_parent_class->_board[i]->_first_player_ind == _opponent_index
           && _parent_class->_board[i]->_second_player_ind == _my_index))
      break;

  if(i >= _parent_class->_board.size())
    qDebug()<<"crash here";

  return i;
}

void UDP_server::User::timer_timeout()
{
  qDebug()<<"===time out";

  static int count = 0;
  if(!_is_message_reach)
  {
    ++count;
    if(_opponent_index != NO_OPPONENT && (_last_sent_message.toInt() == Messages::CLIENT_LOST || count > 5))
    {
      qDebug()<<"last message was client lost";
      _parent_class->send_data(Messages::OPPONENT_LOST, *_parent_class->_user[_opponent_index]);
    }
    _timer->start(SECOND);

    _parent_class->add_serial_num(_last_sent_message, *this, true);
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
  _parent_class->send_data(Messages::CLIENT_LOST, *this);
}

