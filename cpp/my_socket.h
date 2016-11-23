#ifndef __UDP_SOCKET__GOVLIYVFTYUVFTYFDIKVGLUIHL
#define __UDP_SOCKET__GOVLIYVFTYUVFTYFDIKVGLUIHL

#include <QObject>
#include <QUdpSocket>
#include <QByteArray>
#include <QTimer>
#include <memory>
#include "enums.h"


namespace inet
{

class my_socket : public QObject
{
  Q_OBJECT

public:
  my_socket();
  ~my_socket();
  void send(const QByteArray &message, bool is_prev_serial_need = false);
  QByteArray pull();
  bool is_message_append() const;

public:
  my_socket(const my_socket&) = delete;
  my_socket& operator=(const my_socket&) = delete;

private slots:
  void read();
  bool is_message_received();
  void connection_checker_timer_timeout();

private:
  QUdpSocket socket;
  QTimer timer;
  QTimer connection_checker_timer;

  struct impl_t;
  std::unique_ptr<impl_t> impl;
};

} // namespace inet

#endif // __UDP_SOCKET__GOVLIYVFTYUVFTYFDIKVGLUIHL

