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
  void export_readed_data_to_chess(QString& move);

private:
  enum REQUEST_MESSAGES{HELLO_SERVER = 1, HELLO_CLIENT,
                        IS_HAVE_OPPONENT, HAVE_OPPONENT, HAVENT_OPPONENT,
                        IS_SERVER_WORKING};
  const QString FREE_SPASE = " ";
  enum{NEED_SIMBOLS_TO_MOVE = 7};

public slots:
  void read_data();
  void send_data(QByteArray message);
  void send_data(REQUEST_MESSAGES r_mes);

signals:
  void some_data_came();

private:
  QUdpSocket* _socket;
  QByteArray _data;

  const quint16 SERVER_PORT;
  const QHostAddress SERVER_IP;
};
#endif // UDP_CLIENT_H
