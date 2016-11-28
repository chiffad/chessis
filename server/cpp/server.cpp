#include "server.h"

#include <QVector>
#include <QByteArray>
#include <QObject>


using namespace sr;

struct server_t::impl_t
{
  impl_t();
  void send(const QByteArray& message, const int port, const QHostAddress& ip);
  bool is_message_append() const;
  QByteArray pull();
  void read();

  enum {FIRST_PORT = 49152, LAST_PORT = 49500};
  const QHostAddress _SERVER_IP = QHostAddress::LocalHost;

  QUdpSocket socket;
  QVector<QByteArray> received_mes;



};

server_t::server_t()
    : impl(new impl_t())
{
}

server_t::~server_t()
{
}

void server_t::send(const QByteArray& message, const int port, const QHostAddress& ip)
{
  impl->send(message, port, ip);
}

bool server_t::is_message_append() const
{
  return impl->is_message_append();
}

QByteArray server_t::pull()
{
  return impl->pull();
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

void server_t::impl_t::send(const QByteArray& message, const int port, const QHostAddress& ip)
{
  socket.writeDatagram(message, ip, port);
}

bool server_t::impl_t::is_message_append() const
{
  return !received_mes.isEmpty();
}

QByteArray server_t::impl_t::pull()
{
  const auto m = received_mes.first();
  received_mes.removeFirst();

  return m;
}

void server_t::impl_t::read()
{
  QHostAddress ip;
  quint16 port;
  QByteArray m;

  m.resize(socket.pendingDatagramSize());
  socket.readDatagram(m.data(), m.size(), &ip, &port);

  received_mes.append(m);
}

