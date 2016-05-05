#include <QObject>
#include <QUdpSocket>
#include <QByteArray>
#include <QTimer>
#include "headers/udp_client.h"

UDP_client::UDP_client(QObject *parent) : QObject(parent), SERVER_PORT(1234), SERVER_IP(QHostAddress::LocalHost)
{
  _last_received_serial_num = 0;
  _serial_num = 0;

  _timer = new QTimer(this);

  _socket = new QUdpSocket(this);
  _socket->bind(SERVER_IP, SERVER_PORT);
  connect(_socket, SIGNAL(readyRead()), this, SLOT(read_data()));

  send_data(HELLO_SERVER);
}

void UDP_client::send_data(QByteArray message, bool is_prev_serial_need)
{
  if(!checked_is_message_received())
  {
    qDebug()<<"cant send, prev message not reach";
    return;
  }

  add_serial_num(message, is_prev_serial_need);

  qDebug()<<"====Sending data to server"<<message;
  _socket->writeDatagram(message, SERVER_IP, SERVER_PORT);
  begin_wait_receive(message);
}

void UDP_client::send_data(REQUEST_MESSAGES r_mes, bool is_prev_serial_need)
{
  QByteArray message;
  message.setNum(r_mes);

  if(!checked_is_message_received())
  {
    qDebug()<<"cant send, last message not reach";
    return;
  }
  add_serial_num(message, is_prev_serial_need);

  qDebug()<<"====Sending data to server"<<message;
  _socket->writeDatagram(message, SERVER_IP, SERVER_PORT);
  if(r_mes != MESSAGE_RECEIVED)
    begin_wait_receive(message);
}

void UDP_client::begin_wait_receive(const QByteArray& message)
{
  _is_message_received = false;
  _last_send_message = message;
  _timer->start(SECOND);
}

bool UDP_client::checked_is_message_received() // test wariant
{
  qDebug()<<"===checked_message_received";
  if(_is_message_received)
    _timer->stop();
  else
  {
    qDebug()<<"timer restart";
    _timer->start();
    _socket->writeDatagram(_last_send_message, SERVER_IP, SERVER_PORT);
  }
  return _is_message_received;
}

void UDP_client::read_data()
{
  _data.resize(_socket->pendingDatagramSize());
  _socket->readDatagram(_data.data(), _data.size());

  qDebug()<<"=====socket read"<<_data;
  QByteArray serial_num = cut_serial_num(_data);

  if(serial_num.toInt() != ++_last_received_serial_num)
  {
    --_last_received_serial_num;
    qDebug()<<"serial_num is wrong";
    if(serial_num.toInt() == _last_received_serial_num && _data.toInt() != MESSAGE_RECEIVED)
    {
      send_data(MESSAGE_RECEIVED, true);
      qDebug()<<"prev serial num. Resent message";
    }
    return;
  }

  qDebug()<<"received: "<<_data;

  if(_data.size() == NEED_SIMBOLS_TO_MOVE || (_data.toInt() >= MOVE && _data.toInt() <= NEW_GAME))
    emit some_data_came();
  else qDebug()<<"wrong message in read_data_from_udp()";
}

void UDP_client::export_readed_data_to_chess(QString& data)
{
  data.clear();
  data.append(_data);
}

void UDP_client::add_serial_num(QByteArray& data, bool is_prev_serial_need)
{
  if(!is_prev_serial_need)
     ++_serial_num;

  QByteArray serial_num;
  serial_num.setNum(_serial_num);
  serial_num.append(FREE_SPASE);

  data.prepend(serial_num);
}

QByteArray UDP_client::cut_serial_num(QByteArray& data)
{
  QByteArray serial_num;
  while(data.size() > 0 && QChar(data[0]) != FREE_SPASE)
  {
    serial_num.append(data[0]);
    data.remove(0,1);
  }

  while(QChar(data[0]) == FREE_SPASE)
    data.remove(0,1);

  return serial_num;
}
