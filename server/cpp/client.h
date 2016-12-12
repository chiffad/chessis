#ifndef __CLIENT_H__GOVLIYVFTYUVFTYFDIKVGLUIHL
#define __CLIENT_H__GOVLIYVFTYUVFTYFDIKVGLUIHL

#include <QByteArray>
#include <QHostAddress>
#include <QVector>
#include <memory>


namespace inet
{

class client_t
{
public:
  client_t(const int port, const QHostAddress& ip);
  ~client_t();
  void push(QByteArray message);
  QVector<QByteArray> pull_for_server();
  QVector<QByteArray> pull_for_logic();
  int get_port() const;
  QHostAddress get_ip() const;

public:
  client_t(const client_t&) = delete;
  client_t& operator=(const client_t&) = delete;

private:
  struct impl_t;
  std::unique_ptr<impl_t> impl;
};

} // namespace inet

#endif // __CLIENT_H__GOVLIYVFTYUVFTYFDIKVGLUIHL

