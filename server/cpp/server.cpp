#include "server.h"

#include <QObject>
#include <QUdpSocket>

#include "log.h"


using namespace sr;

struct server_t::impl_t
{
  impl_t();
  void send(const QByteArray& message, const int port, const QHostAddress& ip);
  std::vector<datagram_t> pull();
  void read();

  QUdpSocket socket;
  std::vector<datagram_t> messages;
};

server_t::server_t()
    : impl(std::make_unique<impl_t>())
{
}

server_t::~server_t()
{
}

void server_t::send(const QByteArray& message, const int port, const QHostAddress& ip)
{
  impl->send(message, port, ip);
}

std::vector<server_t::datagram_t> server_t::pull()
{
  return impl->pull();
}

server_t::impl_t::impl_t()
{
  QObject::connect(&socket, &QUdpSocket::readyRead, [&](){read();});

  enum { FIRST_PORT = 49152, LAST_PORT = 49500 };

  const QHostAddress SERVER_IP = QHostAddress::LocalHost;

  for(int i = 0; i + FIRST_PORT < LAST_PORT; ++i)
  {
    if(socket.bind(SERVER_IP, FIRST_PORT + i))
    {
      log("server bind: ", FIRST_PORT + i);
      break;
    }
    if(i + FIRST_PORT == LAST_PORT)
      { i = -1; }
  }
}

void server_t::impl_t::send(const QByteArray& message, const int port, const QHostAddress& ip)
{
  log("send: ", message);
  socket.writeDatagram(message, ip, port);
}

std::vector<server_t::datagram_t> server_t::impl_t::pull()
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

  log("read:", m);

  datagram_t d;
  d.ip = ip;
  d.port = port;
  d.message = m;

  messages.push_back(d);
}

