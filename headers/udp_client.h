#ifndef UDP_CLIENT_H
#define UDP_CLIENT_H

#include <QObject>
#include <QUdpSocket>
#include <QString>
#include <QByteArray>

class UDP_client : public QObject
{
  Q_OBJECT
public:
  explicit UDP_client(QObject *parent = 0);
  ~UDP_client(){delete _socket;}

  void export_readed_data_to_chess(QString& move);

private:
  enum REQUEST_MESSAGES{HELLO_SERVER = 1, HELLO_CLIENT,
                        IS_HAVE_OPPONENT, HAVE_OPPONENT, HAVENT_OPPONENT,
                        IS_SERVER_WORKING,SERVER_WORKING,
                        MESSAGE_RECEIVED};
  enum{NEED_SIMBOLS_TO_MOVE = 7};
  const QChar FREE_SPASE = ' ';

public slots:
  void read_data();
  void send_data(QByteArray message);
  void send_data(REQUEST_MESSAGES r_mes);

signals:
  void some_data_came();

private:
  void add_serial_num_and_size(QByteArray& message);
  QByteArray cut_data_to_free_spase(QByteArray& message);

private:
  QUdpSocket* _socket;
  QByteArray _data;
  int _last_received_serial_num;
  int _serial_num;

  const quint16 SERVER_PORT;
  const QHostAddress SERVER_IP;
};
#endif // UDP_CLIENT_H
