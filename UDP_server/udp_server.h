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
                        IS_SERVER_WORKING, SERVER_WORKING,
                        MESSAGE_RECEIVED};
  const QChar FREE_SPASE = ' ';

private:
  void send_data(QByteArray message, const int index);
  void send_data(REQUEST_MESSAGES r_mes, const int index);
  //void whait_for_an_ansver(const QByteArray& message, const int index) const;
  void add_serial_num_and_size(QByteArray& message, const int index);
  QByteArray cut_data_to_free_spase(QByteArray& message);

private:
  const quint16 _SERVER_PORT;
  const QHostAddress _SERVER_IP;
  //bool _is_message_received;

  QUdpSocket *_socket;
  struct User
  {
    quint16 port;
    QHostAddress ip;
    int last_received_serial_num;
    int send_serial_num;
  };
  QVector<User> _user;
};
#endif // UDP_SERVER_H
