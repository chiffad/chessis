#ifndef UDP_CLIENT_H
#define UDP_CLIENT_H

#include <QObject>
#include <QUdpSocket>
#include <QString>

class UDP_client : public QObject
{
  Q_OBJECT
public:
  explicit UDP_client(QObject *parent = 0);
  void export_readed_data_to_chess(QString& move);
  bool is_server_connected();

private:
  enum REQUEST_MESSAGES{HELLO_SERVER, HELLO_CLIENT,
                        IS_HAVE_OPPONENT, HAVE_OPPONENT, HAVENT_OPPONENT,
                        IS_SERVER_WORKING};
public slots:
  void read_data();
  void send_data(const QByteArray& message);
  void send_data(REQUEST_MESSAGES r_mes);

signals:
  void some_data_came();

private:
  QUdpSocket* _socket;
  bool _is_server_connected;
  bool _is_have_opponent;
  const quint16 _SERVER_PORT;
  const QHostAddress _SERVER_IP;
  QByteArray _data;
};
#endif // UDP_CLIENT_H
