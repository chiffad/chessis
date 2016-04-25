#ifndef UDP_CLIENT_H
#define UDP_CLIENT_H

#include <QObject>
#include <QUdpSocket>

class UDP_client : public QObject
{
  Q_OBJECT
public:
  explicit UDP_client(QObject *parent = 0);
  void export_readed_data_to_chess(QString& move);
  bool is_server_ready();

public slots:
  void read_data();
  void send_data(const QByteArray& message);

signals:
  void some_data_came();

private:
  QUdpSocket* _socket;
  quint16 _port;
  QByteArray _data;

  const quint16 _SERVER_PORT = 1234;
  const QHostAddress _SERVER_IP = QHostAddress::LocalHost;
};
#endif // UDP_CLIENT_H
