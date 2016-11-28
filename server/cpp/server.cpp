#include "server.h"

#include <QVector>
#include <QByteArray>
#include <QObject>
#include <algorithm>

#include "server_user.h"
#include "messages.h"


using namespace sr;

struct server_t::impl_t
{
  impl_t();
  void process_event();
  void push(const QByteArray& message, const int port, const QHostAddress& ip);
  bool is_message_append(const int port, const QHostAddress& ip) const;
  QByteArray pull(const int port, const QHostAddress& ip);
  void read();
  int cut_serial_num(QByteArray& m);
  std::shared_ptr<server_user_t> get_user(const int port, const QHostAddress& ip) const;
  void run_message(const QByteArray& m, const int port, const QHostAddress& ip);

  enum {FIRST_PORT = 49152, LAST_PORT = 49500};
  const QHostAddress _SERVER_IP = QHostAddress::LocalHost;

  QUdpSocket socket;
  QVector<QByteArray> received_mes;

  QVector<std::shared_ptr<server_user_t>> users;
};

server_t::server_t()
    : impl(new impl_t())
{
}

server_t::~server_t()
{
}

void server_t::process_event()
{
  impl->process_event();
}

void server_t::push(const QByteArray& message, const int port, const QHostAddress& ip)
{
  impl->push(message, port, ip);
}

bool server_t::is_message_append(const int port, const QHostAddress& ip) const
{
  return impl->is_message_append(port, ip);
}

QByteArray server_t::pull(const int port, const QHostAddress& ip)
{
  return impl->pull(port, ip);
}

server_t::impl_t::impl_t()
{
  QObject::connect(&socket, &QUdpSocket::readyRead, [&](){read();});

  for(int i = 0; i + FIRST_PORT < LAST_PORT; ++i)
  {
    if(socket.bind(_SERVER_IP, FIRST_PORT + i))
    {
      qDebug()<<"server::bind: "<<FIRST_PORT + i;
      break;
    }
    if(i + FIRST_PORT == LAST_PORT - 1)
      { i = -1; }
  }
}

void server_t::impl_t::process_event()
{
  for(auto& i: users)
  {
    if(!i->is_no_message_for_send())
        { socket.writeDatagram(i->pull_message_for_send(), i->get_ip(), i->get_port()); }
  }
}

void server_t::impl_t::push(const QByteArray& message, const int port, const QHostAddress& ip)
{
  get_user(port, ip)->push_for_send(message);
}

bool server_t::impl_t::is_message_append(const int port, const QHostAddress& ip) const
{
  return !(get_user(port, ip)->is_no_received_mess());
}

QByteArray server_t::impl_t::pull(const int port, const QHostAddress& ip)
{
  return get_user(port, ip)->pull_received_mess();
}

std::shared_ptr<server_user_t> server_t::impl_t::get_user(const int port, const QHostAddress& ip) const
{
  auto user = std::find_if(users.begin(), users.end(), [&](auto i){ return i->is_me(port, ip); });

  if(user == users.end())
   { qDebug()<<"server_t::impl_t::is_message_append: no such user!!!!"; }

  return (*user);
}

int server_t::impl_t::cut_serial_num(QByteArray& m)
{
  auto serial_num = m.mid(0, m.indexOf(" "));
  m.remove(0, m.indexOf(" ") + 1);

  return serial_num.toInt();
}

void server_t::impl_t::read()
{
  QHostAddress ip;
  quint16 port;
  QByteArray m;

  m.resize(socket.pendingDatagramSize());
  socket.readDatagram(m.data(), m.size(), &ip, &port);

  run_message(m, port, ip);
}

void server_t::impl_t::run_message(const QByteArray& m, const int port, const QHostAddress& ip)
{
  auto user = std::find_if(users.begin(), users.end(), [&](auto i){ return i->is_me(port, ip); });
  const int num = cut_serial_num(m);

  if(user == users.end())
  {
    if(num != 1)
    {
      qDebug()<<"server_t::impl_t::read(): For new users ser num must be == 1!!";
      return;
    }

    users.append(std::make_shared<server_user_t>(port, ip));
    users.last()->push_received_mess(m);
  }
  else
  {
    if(!(*user)->is_current_serial_num(num))
      { qDebug()<<"server_t::impl_t::read(): Wrong ser num!!"<<num; }

    else if((*user)->is_previous_serial_num(num))
    {
      if(m == QByteArray::number(messages::MESSAGE_RECEIVED))
        { return; }

      qDebug()<<"server_t::impl_t::read(): Wrong ser num!!"<<num;
      QByteArray m = QByteArray::number(num) + " " + QByteArray::number(messages::MESSAGE_RECEIVED);
      socket.writeDatagram(m, ip, port);
      ///sending Message_receive
    }
    else
    {
     // if message == message receive need to user->last_message_received();
      if(m == QByteArray::number(messages::MESSAGE_RECEIVED))
        { (*user)->last_message_received(); }
      else
      {
        (*user)->push_received_mess(m);
        (*user)->increase_receive_serial_num();
      }
    }
  }
}


