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
  explicit UDP_server(QObject *parent = nullptr);
  ~UDP_server(){delete _socket;}

public slots:
  void read_data();

private:
  enum REQUEST_MESSAGES{HELLO_SERVER = 1, MESSAGE_RECEIVED, SERVER_LOST, CLIENT_LOST};
  enum {NO_OPPONENT = -1, SECOND = 1000, TEN_SEC = 10000};
  const QChar FREE_SPASE = ' ';
  class User;

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
  QVector<User*> _user;
};

class UDP_server::User : public QObject
{
  Q_OBJECT
public:
  explicit User(QObject *parent = nullptr, UDP_server *parent_class = nullptr, const quint16& port = 0,
                const QHostAddress& ip = QHostAddress::LocalHost, const int received_serial_num = 0);
  ~User() {delete _timer; delete _timer_last_received_message;}

public slots:
  void timer_timeout();
  void timer_last_received_message_timeout();

public:
  QTimer *_timer;
  QTimer *_timer_last_received_message;

  UDP_server* const _parent_class;
  quint16 _port;
  QHostAddress _ip;
  int _received_serial_num;
  int _send_serial_num;
  int _opponent_index;
  bool _is_message_reach;
  QByteArray _last_sent_message;
  QVector<QByteArray> _message_stack;
};
#endif // UDP_SERVER_H
