#include "client/socket.hpp"

#include <QObject>
#include <spdlog/spdlog.h>

namespace chess::cl {

udp_socket_t::udp_socket_t(const std::function<void()>& ready_read)
{
  QObject::connect(&socket_, &QUdpSocket::readyRead, [=]() { ready_read(); });
}

bool udp_socket_t::bind()
{
  while (!socket_.bind())
  {}
  SPDLOG_DEBUG("Bind socket to port={}", socket_.localPort());
  return true;
}

endpoint_t udp_socket_t::endpoint() const
{
  return {socket_.localAddress().toString().toStdString(), socket_.localPort()};
}

data_t udp_socket_t::read()
{
  QHostAddress address;
  data_t r;
  r.data.resize(socket_.pendingDatagramSize());
  socket_.readDatagram(r.data.data(), r.data.size(), &address, &r.endpoint.port);

  if (address.protocol() == QAbstractSocket::IPv6Protocol)
  {
    bool ok = false;
    QHostAddress ip4_address{address.toIPv4Address(&ok)};
    if (ok)
    {
      r.endpoint.host = ip4_address.toString().toStdString();
    }
  }
  else r.endpoint.host = address.toString().toStdString();
  return r;
}

void udp_socket_t::write(const data_t& data)
{
  socket_.writeDatagram(data.data.c_str(), QHostAddress{QString::fromStdString(data.endpoint.host)}, data.endpoint.port);
}

bool operator==(const endpoint_t& lhs, const endpoint_t& rhs)
{
  return lhs.host == rhs.host && lhs.port == rhs.port;
}

std::ostream& operator<<(std::ostream& os, const endpoint_t& e)
{
  return os << e.host << ":" << e.port;
}

std::ostream& operator<<(std::ostream& os, const data_t& d)
{
  return os << "data_t{ data=" << d.data << "; endpoint=" << d.endpoint << "; }";
}

} // namespace chess::cl