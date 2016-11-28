#include "user.h"

#include <QVector>

#include "desk.h"


using namespace sr;

struct user::impl_t
{
  impl_t(const QHostAddress &ip, const int &port, const QString &login, QTimer& connection);
  void push(const messages::MESSAGE type, const QByteArray& message);
  void push_to_logic(const messages::MESSAGE type, const QByteArray& content);
  bool is_message_appear() const;
  QByteArray pull();
  void set_board(std::shared_ptr<logic::Desk> d);
  void check_connection_timeout();
  void add_to_messages(const messages::MESSAGE m);
  void add_to_messages(const QByteArray& m);
  QByteArray get_board_state() const;


  QTimer& connect_t;
  QHostAddress ip;
  int port;
  QString login;
  int received_serial_num = 1;
  int send_serial_num = 0;
  bool is_message_reach = true;
  std::shared_ptr<logic::Desk> board;

  QVector<QByteArray> messages;
};

user::user(const QHostAddress &ip, const quint16 &port, const QString &login)
    : QObject(nullptr), impl(new impl_t(ip, port, login, connect_timer))
{
  connect(&connect_timer, SIGNAL(timeout()), this, SLOT(check_connection_timeout()));
}

user::~user()
{
}

void user::push(const messages::MESSAGE type, const QByteArray& message)
{
  impl->push(type, message);
}

void user::set_board(std::shared_ptr<logic::Desk> d)
{
  impl->set_board(d);
}

bool user::is_message_appear() const
{
  return impl->is_message_appear();
}

QByteArray user::pull()
{
  return impl->pull();
}

void user::check_connection_timeout()
{
  impl->check_connection_timeout();
}

user::impl_t::impl_t(const QHostAddress &_ip, const int &_port, const QString &log, QTimer& connection)
    : connect_t(connection), ip(_ip), port(_port), login(log)
{
  board = std::make_shared<logic::Desk>(0,0);
  const int CHECK_CONNECT_TIME = 10000;
  connect_t.setInterval(CHECK_CONNECT_TIME);
}

void user::impl_t::set_board(std::shared_ptr<logic::Desk> d)
{
  board = d;
}

void user::impl_t::push(const messages::MESSAGE type, const QByteArray& message)
{
  if(type != messages::MESSAGE_RECEIVED)
    { add_to_messages(messages::MESSAGE_RECEIVED); }

  switch(type)
  {
    case messages::MESSAGE_RECEIVED:
      is_message_reach = true;
      break;
    case messages::IS_SERVER_LOST:
      break;
    case messages::OPPONENT_INF:
      //send_data(get_user_info(*(*u)), *(*u)); //!!!
      break;
    case messages::MY_INF:
//      send_data(get_user_info(*(*u), false), *(*u)) //!!!;
      break;
    default:
      push_to_logic(type, message);
  }
}

void user::impl_t::push_to_logic(const messages::MESSAGE type, const QByteArray& content)
{
//  if(_opponent_index == NO_OPPONENT)
//    { return; }

  switch(type)
  {
    case messages::MOVE:
      board->make_moves_from_str(content.toStdString());
      break;
    case messages::BACK_MOVE:
      board->back_move();
      break;
    case messages::NEW_GAME:
      board->start_new_game();
      break;
    case messages::GO_TO_HISTORY:
      board->go_to_history_index(content.toInt());
      break;
    case messages::FROM_FILE:
      board->start_new_game();
      board->make_moves_from_str(content.toStdString());
      break;
    default: qDebug()<<"UDP_server::push_message_to_logic: Unknown message type";
  }
  add_to_messages(get_board_state());
}

QByteArray user::impl_t::get_board_state() const
{
  QByteArray m;
  m.setNum(messages::GAME_INF);
  m.append(" ");
  m.append(QString::fromStdString(board->get_board_mask()));
  m.append(";");

  m.append(QString::fromStdString(board->get_moves_history()));
  if(board->is_mate())
    { m.append("#"); }
  m.append(";");

  m.append(QByteArray::number(board->get_move_num()));

  return m;
}

void user::impl_t::add_to_messages(const messages::MESSAGE m)
{
  messages.append(QByteArray::number(m));
}

void user::impl_t::add_to_messages(const QByteArray& m)
{
  messages.append(m);
}

bool user::impl_t::is_message_appear() const
{
  return !messages.isEmpty();
}

QByteArray user::impl_t::pull()
{
  const auto m = messages.first();
  messages.removeFirst();

  return m;
}

void user::impl_t::check_connection_timeout()
{}
