#include <QString>
#include <QChar>
#include <QUdpSocket>
#include <QTimer>
#include <algorithm>
#include <cmath>
#include <QIODevice>
#include <QFile>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonDocument>
#include "udp_server.h"
#include "chess.h"
#include "enums.h"

UDP_server::UDP_server(QObject *parent) : QObject(parent), _SERVER_IP(QHostAddress::LocalHost),
                                          _socket(new QUdpSocket(this))
{
  for(int i = 0; i + FIRST_PORT < LAST_PORT; ++i)
  {
    if(_socket->bind(_SERVER_IP, FIRST_PORT + i))
    {
      qDebug()<<"UDP_server::bind: "<<FIRST_PORT + i;
      break;
    }
    if(i + FIRST_PORT == LAST_PORT - 1)
     i = -1;
  }

  connect(_socket, SIGNAL(readyRead()), this, SLOT(read_data()));

  load_users_inf();
  qDebug()<<"Server start!";
}

UDP_server::~UDP_server()
{
  save_users_inf();

  delete _socket;
  for(auto &i : _user)
    delete i;

  for(auto &i : _board)
    delete i;
}

void UDP_server::send_data(const QByteArray &message, User &u)
{
  if(!is_message_reach(message, u))
    return;

  qDebug()<<"UDP_server::sending"<<message;
  _socket->writeDatagram(add_serial_num(message,u), u._ip, u._port);
  begin_wait_receive(u);
}

void UDP_server::send_data(const Messages::MESSAGE r_mes, User &u, bool is_prev_serial_need)
{
  QByteArray message;
  message.setNum(r_mes);

  if(r_mes != Messages::MESSAGE_RECEIVED)
  {
    if(!is_message_reach(message, u))
      return;

    begin_wait_receive(u);
  }

  qDebug()<<"UDP_server::sending"<<message;
  _socket->writeDatagram(add_serial_num(message, u, is_prev_serial_need), u._ip, u._port);
}

void UDP_server::read_data()
{
  QHostAddress sender_IP;
  quint16 sender_port;
  QByteArray message;

  message.resize(_socket->pendingDatagramSize());
  _socket->readDatagram(message.data(), message.size(), &sender_IP, &sender_port);

  qDebug()<<"UDP_server::reading"<<message;

  auto sender = std::find_if(_user.begin(), _user.end(),
                             [sender_port, sender_IP](auto const &i){return(i->_port == sender_port && i->_ip == sender_IP);});

  const int serial_num = cut_serial_num(message);

  const QString t = message.mid(0, message.indexOf(FREE_SPASE));
  if(t.toInt() == Messages::HELLO_SERVER)
  {
    if(sender != _user.end())
    {
      qDebug()<<"this client already have";
      return;
    }

    const QString log = message.mid(message.indexOf(FREE_SPASE) + 1);
    qDebug()<<"HELLO_SERVER"<<log;
    auto u = std::find_if(_user.begin(), _user.end(), [log](auto const &i){return(i->_login == log);});
    if(u != _user.end())
    {
      (*u)->_port = sender_port;
      (*u)->_ip = sender_IP;
      (*u)->_received_serial_num = serial_num;
      (*u)->_send_serial_num = 0;
    }
    else
    {
      _user.append(new User(this, this, sender_port, sender_IP, serial_num, _user.size(), log));
      _user.last()->start_check_connect_timer();

      send_data(Messages::MESSAGE_RECEIVED, *_user.last());
      set_opponent(*_user.last());
    }
  }
  else if(serial_num != ++(*sender)->_received_serial_num)
  {
    qDebug()<<"wrong serial num";

    --(*sender)->_received_serial_num;

    if(serial_num == (*sender)->_received_serial_num && message.toInt() != Messages::MESSAGE_RECEIVED)
    {
      (*sender)->start_check_connect_timer();
      send_data(Messages::MESSAGE_RECEIVED, *_user[_user.indexOf(*sender)], true);
      qDebug()<<"prev serial num. Resent message";
    }
  }
  else run_message(message, *_user[_user.indexOf(*sender)]);
}

void UDP_server::run_message(const QByteArray &message, User &u)
{
  qDebug()<<"UDP_server::run_message";

  const QByteArray type(message.mid(0, message.indexOf(FREE_SPASE)));
  const QByteArray content(message.mid(message.indexOf(FREE_SPASE) + 1));

  if(type.toInt() != Messages::MESSAGE_RECEIVED)
    send_data(Messages::MESSAGE_RECEIVED, u);

  switch(type.toInt())
  {
    case Messages::MESSAGE_RECEIVED:
      qDebug()<<"type == MESSAGE_RECEIVED";
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
      send_data(get_usr_info(u), u);
      break;
    case Messages::MY_INF:
      send_data(get_usr_info(u, false), u);
      break;
    case Messages::LOGIN:
      qDebug()<<"login!: "<<content;
      u._login = content;
      break;
    default:
      push_message_to_logic(type, content, u);
  }
  u.start_check_connect_timer();
}

void UDP_server::push_message_to_logic(const QByteArray &type, const QByteArray &content, User& u)
{
  qDebug()<<"UDP_server::push_message_to_logic";
  if(u.get_board_ind() == NO_OPPONENT)
    return;

  Desk *const board = _board[u.get_board_ind()];
  switch(type.toInt())
  {
    case Messages::MOVE:
    {
      qDebug()<<"MOVE";
      board->make_moves_from_str(content.toStdString());
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
      qDebug()<<"GO_TO_HISTORY"<<content.toInt();
      board->go_to_history_index(content.toInt());
      break;
    case Messages::FROM_FILE:
      qDebug()<<"FROM_FILE: "<<content.toInt();
      board->start_new_game();
      board->make_moves_from_str(content.toStdString());
      break;
    default:
      qDebug()<<"sheet message!";
      return;
  }
  send_board_state(u);
}

void UDP_server::send_board_state(User &u)
{
  qDebug()<<"UDP_server::send_board_state";

  QByteArray message;
  Desk *const board = _board[u.get_board_ind()];

  message.append(QString::fromStdString(board->get_board_mask()));
  message.append(";");

  message.append(QString::fromStdString(board->get_moves_history()));
  if(board->is_mate())
    message.append("#");
  message.append(";");

  message.append(QByteArray::number(board->get_move_num()));

  send_data(message, *_user[u._opponent_index]);
  send_data(message, u);
}

QByteArray UDP_server::get_usr_info(const User &u, bool is_opponent) const
{
  QByteArray inf;
  inf.append(QString::number(Messages::INF_REQUEST));
  inf.append(FREE_SPASE);

  if(is_opponent && u._opponent_index == NO_OPPONENT)
     inf.append("No opponent!");
  else
  {
    const User &user = is_opponent ? *_user[u._opponent_index] : u;
    inf.append("Login: " + user._login + "; Rating ELO: "
                + QByteArray::number(user._rating_ELO));
  }
  return inf;
}

void UDP_server::set_opponent(User &u)
{
  u._opponent_index = _user.indexOf(*std::find_if(_user.begin(), _user.end(),
                               [](auto const &i){return(i-> _opponent_index == NO_OPPONENT);}));

  qDebug()<<"u.opponent_index: "<<u._opponent_index<<" my: "<<u._my_index;

  if(u._opponent_index != NO_OPPONENT)
  {
    _user[u._opponent_index]->_opponent_index = u._my_index;
    _board.append(new Desk(u._my_index, u._opponent_index));
     send_board_state(u);
  }
}

void UDP_server::begin_wait_receive(User &u)
{
  qDebug()<<"UDP_server::begin_wait_receive";
  u._is_message_reach = false;
  u.start_response_timer();
}

QByteArray UDP_server::add_serial_num(const QByteArray &message, User &u, bool is_prev_serial_need)
{
  if(!is_prev_serial_need)
    ++u._send_serial_num;

  QByteArray m;
  m.setNum(u._send_serial_num);
  m.append(FREE_SPASE);
  m.append(message);
  return m;
}

int UDP_server::cut_serial_num(QByteArray &data) const
{
  QByteArray serial_num(data.mid(0, data.indexOf(FREE_SPASE)));
  data.remove(0, data.indexOf(FREE_SPASE) + 1);

  return serial_num.toInt();
}

bool UDP_server::is_message_reach(const QByteArray &message, User &u)
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

bool UDP_server::save_users_inf() const
{
  qDebug()<<"UDP_server::save_users_inf()";
  QFile save_file(QStringLiteral("save.json"));

  if (!save_file.open(QIODevice::WriteOnly)) {
    qWarning("Couldn't open file.");
    return false;
  }

  QJsonObject users_inf;
  write_inf(users_inf);

  QJsonDocument save_doc(users_inf);
  save_file.write(save_doc.toJson());

  return true;
}

void UDP_server::write_inf(QJsonObject &json) const
{
  qDebug()<<"UDP_server::write_inf";
  QJsonArray users;
  for(auto &i : _user)
    users.append(i->get_inf_json());

  json["users"] = users;
}

bool UDP_server::load_users_inf()//!!!
{
  qDebug()<<"UDP_server::load_users_inf()";
  QFile load_file("save.json");

  if (!load_file.open(QIODevice::ReadOnly)) {
    qWarning("Couldn't open file.");
    return false;
  }

  QJsonDocument load_doc(QJsonDocument::fromJson(load_file.readAll()));

  QJsonObject _1 = load_doc.object();

  read_inf(_1);

  return true;
}

void UDP_server::read_inf(QJsonObject &json)
{
    qDebug()<<"UDP_server::read_inf()";
  QJsonArray users_array = json["users"].toArray();
  for(auto i : users_array)
  {
    QJsonObject inf = i.toObject();
    _user.append(new User(this, this, 0, QHostAddress::LocalHost, 0, _user.size(), inf["name"].toString(), inf["ELO"].toInt()));
  }
}


UDP_server::User::User(QObject *parent, UDP_server *parent_class, const quint16 &port, const QHostAddress &ip,
                       const int received_serial_num, const int index, const QString &login, const int ELO)
                       : QObject(parent), _parent_class(parent_class), _port(port), _ip(ip), _my_index(index),
                         _received_serial_num(received_serial_num), _send_serial_num(0), _is_message_reach(true),
                         _login(login), _rating_ELO(ELO),
                         _response_timer(new QTimer), _check_connect_timer(new QTimer)
{
  connect(_response_timer, SIGNAL(timeout()), this, SLOT(response_timer_timeout()));
  connect(_check_connect_timer, SIGNAL(timeout()), this, SLOT(check_connect_timer_timeout()));
}

void UDP_server::User::start_check_connect_timer()
{
  _check_connect_timer->start(CHECK_CONNECT_TIME);
}

void UDP_server::User::start_response_timer()
{
  _response_timer->start(RESPONSE_WAIT_TIME);
}

int UDP_server::User::get_board_ind()
{
  auto it = std::find_if(_parent_class->_board.begin(), _parent_class->_board.end(),
                        [this](auto const &i){return(i->contain_player(_my_index) && i->contain_player(_opponent_index));});

  return _parent_class->_board.indexOf(*it);
}

void UDP_server::User::response_timer_timeout()
{
  qDebug()<<"User::time out";

  static int count = 0;
  if(!_is_message_reach)
  {
    ++count;
    if(_opponent_index != NO_OPPONENT && (_last_sent_message.toInt() == Messages::CLIENT_LOST || count > 5))
    {
      qDebug()<<"last message was client lost";
      _parent_class->send_data(Messages::OPPONENT_LOST, *_parent_class->_user[_opponent_index]);
    }
    start_response_timer();

    _parent_class->_socket->writeDatagram(_parent_class->add_serial_num(_last_sent_message, *this, true),
                                          _ip, _port);
  }
  else
  {
    qDebug()<<"timer stop";
    _response_timer->stop();
    count = 0;
  }
}

void UDP_server::User::check_connect_timer_timeout()
{
  qDebug()<<"User::timer_from_last_received_message_timeout "<<_my_index;
  _parent_class->send_data(Messages::CLIENT_LOST, *this);
}

QJsonObject UDP_server::User::get_inf_json() const //!!!
{
  QJsonObject json;
  json["name"] = _login;
  json["ELO"] = _rating_ELO;

  return json;
}
