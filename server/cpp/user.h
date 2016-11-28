#ifndef __USER_H__BYUVGFTYADVWYUBBAWDTYCFWDTYAVWDUB
#define __USER_H__BYUVGFTYADVWYUBBAWDTYCFWDTYAVWDUB

#include <QTimer>
#include <QByteArray>
#include <QHostAddress>
#include <memory>

#include "desk.h"
#include "messages.h"


namespace sr
{

class user_t
{
public:
  user_t(const QHostAddress &ip, const quint16 &port, const QString &login = "guest");
  ~user_t();
  void push(const messages::MESSAGE type, const QByteArray& message);
  bool is_message_appear() const;
  QByteArray pull();
  void set_board(std::shared_ptr<logic::desk_t> d);

public:
  user_t(const user_t&) = delete;
  user_t& operator=(const user_t&) = delete;

private:
  struct impl_t;
  std::unique_ptr<impl_t> impl;
};

} //namespace sr

#endif //__USER_H__BYUVGFTYADVWYUBBAWDTYCFWDTYAVWDUB