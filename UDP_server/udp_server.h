#ifndef UDP_SERVER_H
#define UDP_SERVER_H
#include <QObject>
#include <QUdpSocket>
#include <QVector>
#include <QString>

class UDP_server : public QObject
{
  Q_OBJECT
public:
  explicit UDP_server(QObject *parent = 0);
  ~UDP_server(){delete _socket;}

public slots:
  void read_data();

private:
  enum REQUEST_MESSAGES{HELLO_SERVER = 1, HELLO_CLIENT,
                        IS_HAVE_OPPONENT, HAVE_OPPONENT, HAVENT_OPPONENT,
                        IS_SERVER_WORKING};
private:
  void send_data(const quint16 port, const QHostAddress& addres, const QByteArray& message);
  void send_data(const quint16 port, const QHostAddress& addres, REQUEST_MESSAGES r_mes);

private:
  const quint16 _SERVER_PORT;
  const QHostAddress _SERVER_IP;

  QUdpSocket *_socket;
  QVector<quint16> _user_port;
  QVector<QHostAddress> _user_addres;
};
#endif // UDP_SERVER_H
