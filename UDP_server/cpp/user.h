#ifndef __USER_H__BYUVGFTYADVWYUBBAWDTYCFWDTYAVWDUB
#define __USER_H__BYUVGFTYADVWYUBBAWDTYCFWDTYAVWDUB

#include <QObject>
#include <QTimer>
#include <QByteArray>
#include <QHostAddress>
#include <memory>

#include "desk.h"
#include "messages.h"


namespace sr
{

class user : public QObject
{
  Q_OBJECT

public:
  user(const QHostAddress &ip, const quint16 &port, const QString &login = "guest");
  ~user();
  void push(const messages::MESSAGE type, const QByteArray& message);
  bool is_message_appear() const;
  QByteArray pull();
  void set_board(std::shared_ptr<logic::Desk> d);

private slots:
  void check_connection_timeout();

private:
  QTimer connect_timer;

  struct impl_t;
  std::unique_ptr<impl_t> impl;
};

} //namespace serv

#endif //__USER_H__BYUVGFTYADVWYUBBAWDTYCFWDTYAVWDUB