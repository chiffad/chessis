#ifndef UDP_SERVER_H
#define UDP_SERVER_H
#include <QObject>
#include <QUdpSocket>
#include <QVector>
#include <QString>
#include <QTimer>
#include <QJsonObject>
#include "desk.h"
#include "enums.h"

class UDP_server : public QObject
{
  Q_OBJECT

public:
  explicit UDP_server(QObject *parent = nullptr);
  ~UDP_server();

public slots:
  void read_data();

private:
  class User;

public:
  void send_data(const QByteArray &message, User &u);
  void send_data(const Messages::MESSAGE r_mes, User &u, bool is_prev_serial_need = false);

private:
  enum {NO_OPPONENT = -1, FIRST_PORT = 49152, LAST_PORT = 49500};
  const QChar FREE_SPASE = ' ';

private:
  UDP_server(const UDP_server&) = delete;
  UDP_server& operator=(const UDP_server&) = delete;

  void set_opponent(User &u);
  void send_board_state(User &u);
  void begin_wait_receive(User &u);
  int cut_serial_num(QByteArray &message) const;
  void run_message(const QByteArray &message, User &u);
  bool is_message_reach(const QByteArray &message, User &u);
  QByteArray get_usr_info(const User &u, bool is_opponent = true) const;
  void push_message_to_logic(const QByteArray &type, const QByteArray &content, User &u);
  QByteArray add_serial_num(const QByteArray &message, User &u, bool is_prev_serial_need = false);

  bool load_users_inf();
  bool save_users_inf() const;
  void write_inf(QJsonObject &json) const;
  void read_inf(QJsonObject &json);

private:
  const QHostAddress _SERVER_IP;

  QUdpSocket *_socket;
  QVector<User*> _user;
  QVector<Desk*> _board;
};

class UDP_server::User : public QObject
{
  Q_OBJECT

public:
  User(QObject *parent = nullptr, UDP_server *parent_class = nullptr, const quint16 &port = 0,
       const QHostAddress &ip = QHostAddress::LocalHost, const int index = 0,
       const QString &login = "guest", const int ELO = 1200);
  ~User() {delete _response_timer; delete _check_connect_timer;}

public:
  int get_board_ind();
  QJsonObject get_inf_json() const;
  void start_response_timer();
  void start_check_connect_timer();
  void reconnect(const quint16 port, const QHostAddress &ip);

public slots:
  void response_timer_timeout();
  void check_connect_timer_timeout();

public:
  UDP_server *const _parent_class;
  quint16 _port;
  QHostAddress _ip;
  const int _my_index;
  int _received_serial_num;
  int _send_serial_num;
  int _opponent_index;
  bool _is_message_reach;
  QByteArray _last_sent_message;
  QVector<QByteArray> _message_stack;

  QString _login;
  int _rating_ELO;

private:
   enum {RESPONSE_WAIT_TIME = 1000, CHECK_CONNECT_TIME = 10000};

private:
   User(const User&) = delete;
   User& operator=(const User&) = delete;

   QTimer *_response_timer;
   QTimer *_check_connect_timer;
};
#endif // UDP_SERVER_H
