#ifndef __USER_H__BYUVGFTYADVWYUBBAWDTYCFWDTYAVWDUB
#define __USER_H__BYUVGFTYADVWYUBBAWDTYCFWDTYAVWDUB

#include <QTimer>
#include <QByteArray>
#include <QHostAddress>
#include <QVector>
#include <memory>


#include "desk.h"
#include "messages.h"


namespace sr
{

class user_t
{
public:
  user_t(const QHostAddress& ip, const int port);
  ~user_t();
  void push(const messages::MESSAGE type, const QByteArray& message);
  void set_board(std::shared_ptr<logic::desk_t> d);
  QByteArray get_board_state() const;
  QByteArray get_info() const;
  int get_port() const;
  QHostAddress get_ip() const;
  bool is_need_check_connection() const;
  bool is_game_active() const;
  const logic::desk_t* get_board() const;


public:
  user_t(const user_t&) = delete;
  user_t& operator=(const user_t&) = delete;

private:
  struct impl_t;
  std::unique_ptr<impl_t> impl;
};

} //namespace sr

#endif //__USER_H__BYUVGFTYADVWYUBBAWDTYCFWDTYAVWDUB