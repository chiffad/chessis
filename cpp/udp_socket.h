#ifndef UDP_SOCET
#define UDP_SOCET

#include <QObject>
#include <QUdpSocket>
#include <QString>
#include <QByteArray>
#include <QTimer>
#include "enums.h"

class UDP_socket : public QObject
{
  Q_OBJECT
public:
  explicit UDP_socket(QObject *parent = nullptr);
  ~UDP_socket();

public:
  void send_data(const QByteArray &message, bool is_prev_serial_need = false);
  void send_data(const Messages::MESSAGE r_mes, bool is_prev_serial_need = false);
  QByteArray pull_received_message();
  bool is_new_message_received() const;

public slots:
  void read_data();
  bool is_message_received();
  void timer_from_last_received_message_timeout();

private:
  enum {RESPONSE_WAIT_TIME = 1000, CHECK_CONNECT_TIME = 5000, FIRST_PORT = 49152, LAST_PORT = 49500};
  const QChar FREE_SPASE = ' ';

private:
  UDP_socket(const UDP_socket&) = delete;
  void operator=(const UDP_socket&) = delete;

  QByteArray add_serial_num(const QByteArray &data, bool is_prev_serial_need = false);
  QByteArray cut_serial_num(QByteArray &data) const;
  void begin_wait_receive(const QByteArray &message);

private:
  QUdpSocket *_socket;
  QTimer *_timer;
  QTimer *_timer_from_last_received_message;

  QByteArray _last_send_message;
  QVector<QByteArray> _send_message_stack;
  QVector<QByteArray> _received_message_stack;
  int _received_serial_num;
  int _send_serial_num;
  bool _is_message_received;

  quint16 _my_port;
  quint16 _server_port;
  const QHostAddress SERVER_IP;
};
#endif // UDP_SOCET

