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

public slots:
  void read_data();

private:
  void send_data(const quint16 port, const QHostAddress& addres, const QByteArray& message);

private:
  quint16 _server_port;
  QHostAddress _server_IP;

  QUdpSocket *_socket;
  QVector<quint16> _user_port;
  QVector<QHostAddress> _user_addres;
};
#endif // UDP_SERVER_H
