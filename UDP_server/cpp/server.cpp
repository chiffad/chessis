#include "server.h"

#include <QVector>
#include <QByteArray>


using namespace sr;

server::impl_t
{
  impl_t(QUdpSocket& s);
  void send(const QByteArray& message, const int port, const QHostAddress& ip);
  bool is_message_appear() const;
  QByteArray pull();
  void read();

  enum {FIRST_PORT = 49152, LAST_PORT = 49500};
  const QHostAddress _SERVER_IP = QHostAddress::LocalHost;

  QUdpSocket& socket;
  QVector<QByteArray> messages;

};

server::server()
    : QObject(nullptr), impl(new impl_t(socket))
{
  connect(&_socket, SIGNAL(readyRead()), this, SLOT(read_data()));
}

void server::send(const QByteArray& message, const int port, const QHostAddress& ip)
{
  impl->send(message, port, ip);
}

bool server::is_message_appear() const
{
  return impl->is_message_appear();
}

QByteArray server::pull()
{
  return impl->pull();
}

void server::read()
{
  impl->read();
}

server::impl_t::impl_t(QUdpSocket& s)
    : socket(s)
{
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

void server::impl_t::send(const QByteArray& message, const int port, const QHostAddress& ip)
{
  socket.writeDatagram(message, ip, port);
}

bool server::impl_t::is_message_appear() const
{
  return !messages.isEmpty();
}

QByteArray server::impl_t::pull()
{
  const auto m = messages.first();
  messages.removeFirst();

  return m;
}

void server::impl_t::read()
{
  QHostAddress ip;
  quint16 port;
  QByteArray m;

  m.resize(_socket.pendingDatagramSize());
  socket.readDatagram(m.data(), message.size(), &ip, &port);

  

  messages.append(m);
}

