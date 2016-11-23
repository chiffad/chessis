#include "udp_server.h"

#include <algorithm>
#include <cmath>
#include <QIODevice>
#include <QFile>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonDocument>

#include "chess.h"
#include "enums.h"


UDP_server::UDP_server(QObject *parent) 
    : QObject(parent), _SERVER_IP(QHostAddress::LocalHost),
      _socket(std::make_shared<QUdpSocket>(this))
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

  connect(_socket.get(), SIGNAL(readyRead()), this, SLOT(read_data()));

  load_users_inf();
}

UDP_server::~UDP_server()
{
  save_users_inf();
}

void UDP_server::send_data(const QByteArray &message, User &u)
{
  if(!is_message_reach(message, u))
    return;

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

  _socket->writeDatagram(add_serial_num(message, u, is_prev_serial_need), u._ip, u._port);
}

void UDP_server::read_data()
{
  QHostAddress sender_IP;
  quint16 sender_port;
  QByteArray message;

  message.resize(_socket->pendingDatagramSize());
  _socket->readDatagram(message.data(), message.size(), &sender_IP, &sender_port);

  run_message(message, sender_IP, sender_port);
}

void UDP_server::run_message(QByteArray &message, const QHostAddress &ip, const quint16 port)
{
  const int serial_num = cut_serial_num(message);
  const auto type = Messages::MESSAGE(message.mid(0, message.indexOf(FREE_SPASE)).toInt());
  const QByteArray content = message.mid(message.indexOf(FREE_SPASE) + 1);

  auto u = std::find_if(_user.begin(), _user.end(),
                             [&port, &ip](auto const &i){return(i->_port == port && i->_ip == ip);});

  if(type == Messages::HELLO_SERVER)
  {
    create_new_user(ip, port, content);
    return;
  }

  if(u ==_user.end())
    qDebug()<<"UDP_server::run_message: Unknown user";

  else if(!check_serial_num(serial_num, type, *(*u)))
    qDebug()<<"UDP_server::run_message: Wrong Serial num!";

  else
  {
    if(type != Messages::MESSAGE_RECEIVED)
      send_data(Messages::MESSAGE_RECEIVED, *(*u));

    switch(type)
    {
      case Messages::MESSAGE_RECEIVED:
        (*u)->_is_message_reach = true;
        if(!(*u)->_message_stack.isEmpty())
        {
          send_data((*u)->_message_stack[0], *(*u));
          (*u)->_message_stack.removeFirst();
        }
        break;
      case Messages::IS_SERVER_LOST:
        break;
      case Messages::OPPONENT_INF:
        send_data(get_usr_info(*(*u)), *(*u));
        break;
      case Messages::MY_INF:
        send_data(get_usr_info(*(*u), false), *(*u));
        break;
      default:
        push_message_to_logic(type, content, *(*u));
    }
    (*u)->start_check_connect_timer();
  }
}

bool UDP_server::check_serial_num(const int num, const Messages::MESSAGE type, User &u)
{
  if(num != ++u._received_serial_num)
  {
    --u._received_serial_num;

    if(num == u._received_serial_num && type != Messages::MESSAGE_RECEIVED)
    {
      u.start_check_connect_timer();
      send_data(Messages::MESSAGE_RECEIVED, u, true);
    }
    return false;
  }
  return true;
}

void UDP_server::create_new_user(const QHostAddress &ip, const quint16 port, const QByteArray &login)
{
  auto sender = std::find_if(_user.begin(), _user.end(),
                             [&port, &ip](auto const &i){return(i->_port == port && i->_ip == ip);});
  if(sender != _user.end())
    return;

  auto u = std::find_if(_user.begin(), _user.end(), [&login](auto const &i){return(i->_login == login);});
  if(u != _user.end())
    (*u)->reconnect(port, ip);
  else
  {
    _user.append(std::make_shared<User>(this, this, port, ip, _user.size(), login));
    _user.last()->start_check_connect_timer();

    send_data(Messages::MESSAGE_RECEIVED, *_user.last());
    set_opponent(*_user.last());
  }
}

void UDP_server::push_message_to_logic(const Messages::MESSAGE type, const QByteArray &content, User& u)
{
  if(u.get_board_ind() == NO_OPPONENT)
    return;

  auto& board = _board[u.get_board_ind()];

  switch(type)
  {
    case Messages::MOVE:
      board->make_moves_from_str(content.toStdString());
      break;
    case Messages::BACK_MOVE:
      board->back_move();
      break;
    case Messages::NEW_GAME:
      board->start_new_game();
      break;
    case Messages::GO_TO_HISTORY:
      board->go_to_history_index(content.toInt());
      break;
    case Messages::FROM_FILE:
      board->start_new_game();
      board->make_moves_from_str(content.toStdString());
      break;
    default: qDebug()<<"UDP_server::push_message_to_logic: Unknown message type";
  }
  send_board_state(u);
}

void UDP_server::send_board_state(User &u)
{
  auto& board = _board[u.get_board_ind()];

  QByteArray m;
  m.setNum(Messages::GAME_INF);
  m.append(FREE_SPASE);
  m.append(QString::fromStdString(board->get_board_mask()));
  m.append(";");

  m.append(QString::fromStdString(board->get_moves_history()));
  if(board->is_mate())
    m.append("#");
  m.append(";");

  m.append(QByteArray::number(board->get_move_num()));

  send_data(m, *_user[u._opponent_index]);
  send_data(m, u);
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

  if(u._opponent_index != NO_OPPONENT)
  {
    _user[u._opponent_index]->_opponent_index = u._my_index;
    _board.append(std::make_shared<logic::Desk>(u._my_index, u._opponent_index));
     send_board_state(u);
  }
}

void UDP_server::begin_wait_receive(User &u)
{
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
  QByteArray serial_num = data.mid(0, data.indexOf(FREE_SPASE));
  data.remove(0, data.indexOf(FREE_SPASE) + 1);

  return serial_num.toInt();
}

bool UDP_server::is_message_reach(const QByteArray &message, User &u)
{
  if(!u._is_message_reach)
  {
    u._message_stack.push_back(message);
    return false;
  }

  u._last_sent_message = message;
  return true;
}

void UDP_server::save_users_inf() const
{
  QFile save_file(QStringLiteral("save.json"));

  if (!save_file.open(QIODevice::WriteOnly))
  {
    qDebug()<<"Warning! in UDP_server::save_users_inf: Couldn't open file.";
    return;
  }

  QJsonObject users_inf;
  write_inf(users_inf);

  QJsonDocument save_doc(users_inf);
  save_file.write(save_doc.toJson());
}

void UDP_server::write_inf(QJsonObject &json) const
{
  QJsonArray users;
  for(auto &i : _user)
    users.append(i->get_inf_json());

  json["users"] = users;
}

void UDP_server::load_users_inf()
{
  QFile load_file("save.json");

  if (!load_file.open(QIODevice::ReadOnly))
  {
    qDebug()<<"UDP_server::load_users_inf: Couldn't open file.";
    return;
  }

  QJsonDocument load_doc(QJsonDocument::fromJson(load_file.readAll()));
  QJsonObject _1 = load_doc.object();
  read_inf(_1);
}

void UDP_server::read_inf(QJsonObject &json)
{
  QJsonArray users_array = json["users"].toArray();
  for(auto i : users_array)
  {
    QJsonObject inf = i.toObject();
    _user.append(std::make_shared<User>(this, this, 0, QHostAddress::LocalHost, _user.size(),
                                        inf["name"].toString(), inf["ELO"].toInt()));
  }
}

UDP_server::User::User(QObject *parent, UDP_server *parent_class, const quint16 &port, const QHostAddress &ip,
                       const int index, const QString &login, const int ELO)
                       : QObject(parent), _parent_class(parent_class), _port(port), _ip(ip), _my_index(index),
                         _received_serial_num(1), _send_serial_num(0), _is_message_reach(true),
                         _login(login), _rating_ELO(ELO),
                         _response_timer(std::make_shared<QTimer>()), _check_connect_timer(std::make_shared<QTimer>())
{
  connect(_response_timer.get(), SIGNAL(timeout()), this, SLOT(response_timer_timeout()));
  connect(_check_connect_timer.get(), SIGNAL(timeout()), this, SLOT(check_connect_timer_timeout()));
}

UDP_server::User::~User()
{
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
  static int count = 0;
  if(!_is_message_reach)
  {
    ++count;
    if(_opponent_index != NO_OPPONENT && (_last_sent_message.toInt() == Messages::CLIENT_LOST || count > 5))
      _parent_class->send_data(Messages::OPPONENT_LOST, *_parent_class->_user[_opponent_index]);

    start_response_timer();

    _parent_class->_socket->writeDatagram(_parent_class->add_serial_num(_last_sent_message, *this, true),
                                          _ip, _port);
  }
  else
  {
    _response_timer->stop();
    count = 0;
  }
}

void UDP_server::User::check_connect_timer_timeout()
{
  _parent_class->send_data(Messages::CLIENT_LOST, *this);
}

QJsonObject UDP_server::User::get_inf_json() const
{
  QJsonObject json;
  json["name"] = _login;
  json["ELO"] = _rating_ELO;

  return json;
}

void UDP_server::User::reconnect(const quint16 port, const QHostAddress &ip)
{
  _port = port;
  _ip = ip;
  _received_serial_num = 1;
  _send_serial_num = 0;
}
