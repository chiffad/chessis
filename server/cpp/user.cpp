#include "user.h"

#include <QObject>
#include <exception>

#include "log.h"


using namespace sr;

struct user_t::impl_t
{
  impl_t(const QHostAddress& ip, const int& port);
  void push(const messages::MESSAGE type, const QByteArray& message);
  void set_board(std::shared_ptr<logic::desk_t> d);
  void check_connection_timeout();
  QByteArray get_board_state() const;
  QByteArray get_info() const;
  int get_port() const;
  QHostAddress get_ip() const;
  bool is_need_check_connection() const;
  bool is_game_active() const;
  const logic::desk_t* get_board() const;


  QTimer connect_timer;
  QHostAddress ip;
  int port;
  QByteArray login;
  int rating = 1200;
  std::shared_ptr<logic::desk_t> board;

  bool need_check_connection = false;
};

user_t::user_t(const QHostAddress& ip, const int port)
    : impl(new impl_t(ip, port))
{
}

user_t::~user_t()
{
}

void user_t::push(const messages::MESSAGE type, const QByteArray& message)
{
  impl->push(type, message);
}

void user_t::set_board(std::shared_ptr<logic::desk_t> d)
{
  impl->set_board(d);
}

QByteArray user_t::get_board_state() const
{
  return impl->get_board_state();
}

QByteArray user_t::get_info() const
{
  return impl->get_info();
}

int user_t::get_port() const
{
  return impl->get_port();
}

QHostAddress user_t::get_ip() const
{
  return impl->get_ip();
}

bool user_t::is_need_check_connection() const
{
  return impl->is_need_check_connection();
}

bool user_t::is_game_active() const
{
  return impl->is_game_active();
}

const logic::desk_t* user_t::get_board() const
{
  return impl->get_board();
}

user_t::impl_t::impl_t(const QHostAddress& _ip, const int& _port)
    : ip(_ip), port(_port)
{
  QObject::connect(&connect_timer, &QTimer::timeout, [&](){check_connection_timeout();});
  const int CHECK_CONNECT_TIME = 10000;
  connect_timer.setInterval(CHECK_CONNECT_TIME);
  connect_timer.start();
}

void user_t::impl_t::set_board(std::shared_ptr<logic::desk_t> d)
{
  board = d;
}

void user_t::impl_t::push(const messages::MESSAGE type, const QByteArray& content)
{
  qDebug()<<"===========";
  qDebug()<<"user_t::impl_t::push: "<<type<<" "<<content;

  if(type == messages::HELLO_SERVER)
  {
    login == content;
    return;
  }

  if(!board)
    { return; }

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

  connect_timer.start();
  need_check_connection = false;
}

QByteArray user_t::impl_t::get_board_state() const
{
  if(!board)
    { sr::throw_exception("board not exist!"); }

  QByteArray m;
  m.setNum(messages::GAME_INF);
  m.append(" " + QByteArray::fromStdString(board->get_board_mask()) + ";");

  m.append(QByteArray::fromStdString(board->get_moves_history()));
  if(board->is_mate())
    { m.append("#"); }
  m.append(";");

  m.append(QByteArray::number(board->get_move_num()));

  return m;
}

QByteArray user_t::impl_t::get_info() const
{
  return (QByteArray::number(messages::INF_REQUEST) + " Login: " + login
          + "; Rating ELO: " + QByteArray::number(rating));
}

void user_t::impl_t::check_connection_timeout()
{
  need_check_connection = true;
}

int user_t::impl_t::get_port() const
{
  return port;
}

QHostAddress user_t::impl_t::get_ip() const
{
  return ip;
}

bool user_t::impl_t::is_game_active() const
{
  return board.operator bool();
}

bool user_t::impl_t::is_need_check_connection() const
{
  return need_check_connection;
}

const logic::desk_t* user_t::impl_t::get_board() const
{
  return board.get();
}

