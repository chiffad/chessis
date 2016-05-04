#ifndef UDP_SERVER_H
#define UDP_SERVER_H
#include <QObject>
#include <QUdpSocket>
#include <QVector>
#include <QString>
#include <QTimer>

class UDP_server : public QObject
{
  Q_OBJECT
public:
  explicit UDP_server(QObject *parent = 0);
  ~UDP_server(){delete _socket; delete _timer;}

public slots:
  void read_data();
  void repeat_message();

private:
  enum REQUEST_MESSAGES{HELLO_SERVER = 1, MESSAGE_RECEIVED};
  enum {SECOND = 1000};
  const QChar FREE_SPASE = ' ';

private:
  void send_data(QByteArray message, const int i);
  void send_data(REQUEST_MESSAGES r_mes, const int i);
  void add_serial_num(QByteArray& message, const int i);
  QByteArray cut_serial_num_from_data(QByteArray& message);
  void begin_wait_confirm(const int i);

private:
  const quint16 _SERVER_PORT;
  const QHostAddress _SERVER_IP;

  QUdpSocket *_socket;
  QTimer *_timer;

  struct User
  {
    quint16 port;
    QHostAddress ip;
    QByteArray last_sent_message;
    int last_received_serial_num;
    int send_serial_num;
    bool is_message_reach;
  };
  QVector<User> _user;
};
#endif // UDP_SERVER_H
