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
  void checked_is_message_received();

private:
  enum REQUEST_MESSAGES{HELLO_SERVER = 1, MESSAGE_RECEIVED};
  enum {NO_OPPONENT = -1, SECOND = 1000};
  const QChar FREE_SPASE = ' ';
  struct User
  {
    quint16 port;
    QHostAddress ip;
    QByteArray last_sent_message;
    int last_received_serial_num;
    int send_serial_num;
    int opponent_index;
    bool is_message_reach;
  };

private:
  void send_data(QByteArray& message, const int index, bool is_prev_serial_need = false);
  void send_data(REQUEST_MESSAGES r_mes, const int index, bool is_prev_serial_need = false);
  void add_serial_num(QByteArray& message, const int index, bool is_prev_serial_need = false);
  QByteArray cut_serial_num_from_data(QByteArray& message);
  void begin_wait_receive(const int index);
  void set_opponent(User& u);

private:
  const quint16 _SERVER_PORT;
  const QHostAddress _SERVER_IP;

  QUdpSocket *_socket;
  QTimer *_timer;

  QVector<User> _user;
};
#endif // UDP_SERVER_H
