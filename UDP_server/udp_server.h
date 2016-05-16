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
  ~UDP_server(){delete _socket;}

public slots:
  void read_data();
  void timer_timeout();
  void timer_last_received_message_timeout();

private:
  enum REQUEST_MESSAGES{HELLO_SERVER = 1, MESSAGE_RECEIVED, SERVER_LOST, CLIENT_LOST};
  enum {NO_OPPONENT = -1, SECOND = 1000, TEN_SEC = 10000};
  const QChar FREE_SPASE = ' ';

  struct User
  {
    ~User(){delete timer; delete timer_last_received_message;}
    QTimer *timer;
    QTimer *timer_last_received_message;

    quint16 port;
    QHostAddress ip;
    int received_serial_num;
    int send_serial_num;
    int opponent_index;
    bool is_message_reach;
    QByteArray last_sent_message;
    QVector<QByteArray> message_stack;
  };

private:
  void send_data(QByteArray& message, User& u);
  void send_data(REQUEST_MESSAGES r_mes, User& u, bool is_prev_serial_need = false);
  bool is_message_reach(QByteArray& message, User& u);
  void add_serial_num(QByteArray& message, User& u, bool is_prev_serial_need = false);
  QByteArray cut_serial_num_from_data(QByteArray& message) const;
  void begin_wait_receive(User& u);
  void set_opponent(User& u);

private:
  const quint16 _SERVER_PORT;
  const QHostAddress _SERVER_IP;

  QUdpSocket *_socket;

  QVector<User> _user;
};
#endif // UDP_SERVER_H
