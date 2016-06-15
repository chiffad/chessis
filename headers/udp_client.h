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
  ~UDP_client(){}

public:
  enum REQUEST_MESSAGES{HELLO_SERVER = 1, MESSAGE_RECEIVED, IS_SERVER_LOST, CLIENT_LOST, OPPONENT_LOST_FROM_SERVER, OPPONENT_INF, MY_INF};
  enum CHESS_MESSAGE_TYPE{MOVE = 10, BACK_MOVE, GO_TO_HISTORY, NEW_GAME, OPPONENT_INF_REQUEST, MY_INF_REQUEST, SERVER_HERE, SERVER_LOST, OPPONENT_LOST};
  enum{NEED_SIMBOLS_TO_MOVE = 7, SECOND = 1000, TEN_SEC = 10000};
  const QChar FREE_SPASE = ' ';

public:
  void pull_message_from
private:

private:
  QByteArray _data;
};
#endif // UDP_CLIENT_H
