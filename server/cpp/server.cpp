#include "server.h"

#include <QObject>
#include <QUdpSocket>


using namespace sr;

struct server_t::impl_t
{
  impl_t();
  void send(const QByteArray& message, const int port, const QHostAddress& ip);
  QVector<datagram_t> pull();
  void read()

  QUdpSocket socket;
  QVector<datagram_t> messages;
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

QVector<server_t::datagram_t> server_t::pull()
{
  return impl->pull();
}

server_t::impl_t::impl_t()
{
  QObject::connect(&socket, &QUdpSocket::readyRead, [&](){read();});

  enum(FIRST_PORT = 49152, LAST_PORT = 49500)
  const QHostAddress SERVER_IP = QHostAddress::LocalHost;

  for(int i = 0; i + FIRST_PORT < LAST_PORT; ++i)
  {
    if(socket.bind(SERVER_IP, FIRST_PORT + i))
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

QVector<server_t::datagramm_t> server_t::impl_t::pull()
{
  auto _1 = messages;
  messages.clear();

  return _1;
}

void server_t::impl_t::read()
{
  QHostAddress ip;
  quint16 port;
  QByteArray m;

  m.resize(socket.pendingDatagramSize());
  socket.readDatagram(m.data(), m.size(), &ip, &port);

qDebug()<<"read!:"<<m;

  datagramm_t d;
  d.ip = ip;
  d.port = port;
  d.message = m;

  messages.append(d);
}

