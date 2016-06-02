#ifndef UDP_CLIENT_H
#define UDP_CLIENT_H

#include <QObject>
#include <QUdpSocket>
#include <QString>
#include <QByteArray>
#include <QTimer>

class UDP_client : public QObject
{
  Q_OBJECT
public:
  explicit UDP_client(QObject *parent = 0);
  ~UDP_client(){delete _socket; delete _timer; delete _timer_from_last_received_message;}

public:
  enum REQUEST_MESSAGES{HELLO_SERVER = 1, MESSAGE_RECEIVED, IS_SERVER_LOST, CLIENT_LOST, OPPONENT_LOST_FROM_SERVER};
  enum CHESS_MESSAGE_TYPE{MOVE = 10, BACK_MOVE, GO_TO_HISTORY, NEW_GAME, SERVER_LOST, OPPONENT_LOST, SHOW_OPPONENT_INF};
  enum{NEED_SIMBOLS_TO_MOVE = 7, SECOND = 1000, TEN_SEC = 10000};
  const QChar FREE_SPASE = ' ';

public:
  void send_data(QByteArray& message, bool is_prev_serial_need = false);
  void send_data(REQUEST_MESSAGES r_mes, bool is_prev_serial_need = false);
  void export_readed_data_to_chess(QString& move) const;

public slots:
  void read_data();
  bool checked_is_message_received();
  void timer_from_last_received_message_timeout();

signals:
  void some_data_came();

private:
  void add_serial_num(QByteArray& data, bool is_prev_serial_need = false);
  QByteArray cut_serial_num(QByteArray& data) const;
  void begin_wait_receive(const QByteArray& message);
  void set_data_and_emit_to_chess(CHESS_MESSAGE_TYPE message);

private:
  QUdpSocket *_socket;
  QTimer *_timer;
  QTimer *_timer_from_last_received_message;
  
  QByteArray _data;
  QByteArray _last_send_message;
  QVector<QByteArray> _message_stack;
  int _received_serial_num;
  int _send_serial_num;
  bool _is_message_received;

  const quint16 SERVER_PORT;
  const QHostAddress SERVER_IP;
};
#endif // UDP_CLIENT_H
