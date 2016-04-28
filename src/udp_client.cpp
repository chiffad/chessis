#include <QObject>
#include <QUdpSocket>
#include "headers/udp_client.h"

UDP_client::UDP_client(QObject *parent) : QObject(parent), _SERVER_PORT(1234), _SERVER_IP(QHostAddress::LocalHost)
{
  _socket = new QUdpSocket(this);
  _socket->bind(_SERVER_IP, _SERVER_PORT);
  connect(_socket, SIGNAL(readyRead()), this, SLOT(read_data()));

  send_data(HELLO_SERVER);
}

void UDP_client::send_data(const QByteArray& message)
{
  qDebug()<<"====Sending data to server====";
  _socket->writeDatagram(message, _SERVER_IP, _SERVER_PORT);
}

void UDP_client::send_data(REQUEST_MESSAGES r_mes)
{
  QByteArray message;
  message.setNum(r_mes);
  _socket->writeDatagram(message, _SERVER_IP, _SERVER_PORT);
}

bool UDP_client::is_server_connected()
{
  send_data(IS_HAVE_OPPONENT);
  send_data(IS_SERVER_WORKING);
  return _is_server_connected && _is_have_opponent;
}

void UDP_client::read_data()
{
  _data.resize(_socket->pendingDatagramSize());

  QHostAddress sender;
  quint16 senderPort;

  _socket->readDatagram(_data.data(), _data.size(), &sender, &senderPort);

  switch(_data.toInt())
  {
    case HELLO_CLIENT:
      qDebug()<<"server_connected";
      _is_server_connected = true;
      break;
    case HAVE_OPPONENT:
      qDebug()<<"have_opponent";
      _is_have_opponent = true;
      break;
    case HAVENT_OPPONENT:
      qDebug()<<"havent_opponent";
      _is_have_opponent = false;
      break;
    default:
      emit some_data_came();
  }
}

void UDP_client::export_readed_data_to_chess(QString& message)
{
  message.clear();
  message.append(_data);
}
