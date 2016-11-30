#include "server_user.h"

#include <QVector>
#include <QTimer>
#include <QDebug>
#include <exception>

#include "messages.h"
#include "log.h"


using namespace sr;

struct server_user_t::impl_t
{
  impl_t(const int port, const QHostAddress& address);
  bool is_me(const int _port, const QHostAddress& _ip) const;
  int get_port() const;
  QHostAddress get_ip() const;

  void push_for_send(const QByteArray& m);
  QByteArray pull_message_for_send();
  bool is_no_message_for_send() const;

  void push_received_mess(const QByteArray& m);
  bool is_no_received_mess() const;
  QByteArray pull_received_mess();

  void receiving_timeout();
  int cut_serial_num(QByteArray& m);

  const int port;
  const QHostAddress ip;
  int send_serial_num = 0;
  int receiv_serial_num = 1;

  QVector<QByteArray> mess_for_send;
  QByteArray last_sent_mess;
  QByteArray repeate_message_receive;
  QVector<QByteArray> received_mess;

  bool is_last_mess_reach = true;
};

server_user_t::server_user_t(const int port, const QHostAddress& ip)
    : impl(new impl_t(port, ip))
{
}

server_user_t::~server_user_t()
{
}

bool server_user_t::is_me(const int port, const QHostAddress& ip) const
{
  return impl->is_me(port, ip);
}
int server_user_t::get_port() const
{
  return impl->get_port();
}

QHostAddress server_user_t::get_ip() const
{
  return impl->get_ip();
}

void server_user_t::push_for_send(const QByteArray& m)
{
  impl->push_for_send(m);
}

bool server_user_t::is_no_message_for_send() const
{
  return impl->is_no_message_for_send();
}

QByteArray server_user_t::pull_message_for_send()
{
  return impl->pull_message_for_send();
}

void server_user_t::push_received_mess(const QByteArray& m)
{
  impl->push_received_mess(m);
}

bool server_user_t::is_no_received_mess() const
{
  return impl->is_no_received_mess();
}

QByteArray server_user_t::pull_received_mess()
{
  return impl->pull_received_mess();
}

server_user_t::impl_t::impl_t(const int p, const QHostAddress& address)
    : port(p), ip(address)
{
}

bool server_user_t::impl_t::is_me(const int _port, const QHostAddress& _ip) const
{
  return (port == _port && ip == _ip);
}

int server_user_t::impl_t::get_port() const
{
  return port;
}

QHostAddress server_user_t::impl_t::get_ip() const
{
  return ip;
}

void server_user_t::impl_t::push_for_send(const QByteArray& m)
{
  qDebug()<<"============";
  qDebug()<<"server_user_t::impl_t::push_for_send"<<m;
  mess_for_send.append(m);
}

QByteArray server_user_t::impl_t::pull_message_for_send() //need correct
{
  qDebug()<<"===========";
  qDebug()<<"server_user_t::impl_t::pull_message_for_send()";
  if(!repeate_message_receive.isEmpty())
  {
    const auto m = repeate_message_receive;
    repeate_message_receive.clear();
    return m;
  }

  if(mess_for_send.isEmpty())
    { sr::throw_exception("mess_for_send empty!"); }

  const auto m = mess_for_send.first();
  mess_for_send.removeFirst();

  if(m.toInt() != messages::MESSAGE_RECEIVED)
  {
    const int RECEIV_CHECK_TIME = 5000;
    QTimer::singleShot(RECEIV_CHECK_TIME, [&](){receiving_timeout();});
  }

  last_sent_mess = QByteArray::number(++send_serial_num) + " " + m;

  return last_sent_mess;
}

void server_user_t::impl_t::push_received_mess(const QByteArray& message)
{
  qDebug()<<"===========";
  qDebug()<<"server_user_t::impl_t::push_received_mess:"<<message;
  QByteArray m = message;
  const int ser_num = cut_serial_num(m);

  if(receiv_serial_num != ser_num)
    { qDebug()<<"Wrong ser num!!"<<ser_num<<"must be:"<<receiv_serial_num; }

  else if(receiv_serial_num - 1 == ser_num)
  {
    qDebug()<<"Previous ser num!!"<<ser_num;
    if(m.toInt() == messages::MESSAGE_RECEIVED)
      { return; }

    repeate_message_receive = QByteArray::number(send_serial_num) + " " + QByteArray::number(messages::MESSAGE_RECEIVED);
  }
  else
  {
    qDebug()<<"correct ser num!!!"<<m;
    ++receiv_serial_num;

    if(m.toInt() == messages::MESSAGE_RECEIVED)
    {
      is_last_mess_reach = true;
      qDebug()<<"m.toInt() == messages::MESSAGE_RECEIVED";
    }
    else
    {
      mess_for_send.push_front(QByteArray::number(messages::MESSAGE_RECEIVED));

      if(m.toInt() == messages::IS_SERVER_LOST)
      {
        qDebug()<<"m.toInt() == messages::IS_SERVER_LOST";
        return;
      }

      received_mess.append(m);
    }
  }
}

int server_user_t::impl_t::cut_serial_num(QByteArray& m)
{
  auto serial_num = m.mid(0, m.indexOf(" "));
  m.remove(0, m.indexOf(" ") + 1);

  return serial_num.toInt();
}

bool server_user_t::impl_t::is_no_received_mess() const
{
  return received_mess.isEmpty();
}

QByteArray server_user_t::impl_t::pull_received_mess()
{
  if(received_mess.isEmpty())
    { sr::throw_exception("received_mess empty!"); }

  const auto m = received_mess.first();
  received_mess.removeFirst();

  return m;
}

bool server_user_t::impl_t::is_no_message_for_send() const
{
  return (mess_for_send.isEmpty() && is_last_mess_reach);// || repeate_message_receive.isEmpty();
}

void server_user_t::impl_t::receiving_timeout()
{
  if(is_last_mess_reach)
    { return; }

  mess_for_send.push_front(last_sent_mess);
}

