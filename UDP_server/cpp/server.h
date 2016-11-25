#ifndef __SERVER_H__BVGHVJHBDCWFDVAKYUVLWBALIGDVGSADW
#define __SERVER_H__BVGHVJHBDCWFDVAKYUVLWBALIGDVGSADW


#include <QObject>
#include <QByteArray>
#include <QHostAddress>
#include <QUdpSocket>
#include <memory>

namespace sr
{

class server : public QObject
{
  Q_OBJECT

public:
  server();
  ~server();

public:
  void send(const QByteArray& message, const int port, const QHostAddress& ip);
  bool is_message_appear() const;
  QByteArray pull();

private slots:
  void read();

public:
  server(const server&) = delete;
  server& operator=(const server&) = delete;

private:
  QUdpSocket socket;

  struct impl_t;
  std::unique_ptr<impl_t> impl;
};

} // namespace inet

#endif // __SERVER_H__BVGHVJHBDCWFDVAKYUVLWBALIGDVGSADW