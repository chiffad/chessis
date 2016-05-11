#include <QObject>
#include <QUdpSocket>
#include <QByteArray>
#include <QTimer>
#include "headers/udp_client.h"

UDP_client::UDP_client(QObject *parent) : QObject(parent), _last_received_serial_num(0), _serial_num(0),
                                          _is_message_received(true), SERVER_PORT(1234), SERVER_IP(QHostAddress::LocalHost)
{
  _timer = new QTimer(this);
  connect(_timer, SIGNAL(timeout()), this, SLOT(checked_is_message_received()));

  _socket = new QUdpSocket(this);
  _socket->bind(SERVER_IP, SERVER_PORT);
  connect(_socket, SIGNAL(readyRead()), this, SLOT(read_data()));

  send_data(HELLO_SERVER);
}

void UDP_client::send_data(QByteArray& message)
{
  if(!checked_is_message_received())
  {
    qDebug()<<"can't send, prev message not reach";
    _message_stack.push_back(message);
    return;
  }

  add_serial_num(message);

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
    qDebug()<<"can't send, last message not reach";
    _message_stack.push_back(message);
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
  qDebug()<<"====begin waight";
  _is_message_received = false;
  _last_send_message = message;
  _timer->start(SECOND);
}

bool UDP_client::checked_is_message_received()
{
  static unsigned repeat_message_counter = 0;
  qDebug()<<"===checked_message_received";
  if(_is_message_received)
  {
    _timer->stop();
    repeat_message_counter = 0;
  }
  else
  {
    if(repeat_message_counter > 5)
    {
      qDebug()<<"_repeat_message_counter > 5, connection lost?";
    }

    qDebug()<<"timer restart";
    _timer->start(SECOND);
    _socket->writeDatagram(_last_send_message, SERVER_IP, SERVER_PORT);
    ++repeat_message_counter;
  }
  return _is_message_received;
}

void UDP_client::read_data()
{
  QHostAddress sender_IP;
  quint16 sender_port;

  _data.resize(_socket->pendingDatagramSize());
  _socket->readDatagram(_data.data(), _data.size(), &sender_IP, &sender_port);

  if(sender_IP != SERVER_IP || sender_port != SERVER_PORT)
  {
    qDebug()<<"wrong sender!";
    return;
  }

  qDebug()<<"=====socket read"<<_data;
  QByteArray serial_num = cut_serial_num(_data);

  if(serial_num.toInt() != ++_last_received_serial_num)
  {
    --_last_received_serial_num;
    if(serial_num.toInt() == _last_received_serial_num && _data.toInt() != MESSAGE_RECEIVED)
    {
      send_data(MESSAGE_RECEIVED, true);
      qDebug()<<"prev serial num. Resent message";
    }
    else qDebug()<<"serial_num is wrong";
    return;
  }

  qDebug()<<"received: "<<_data;

  switch (_data.toInt())
  {
    case MESSAGE_RECEIVED:
      qDebug()<<"Message received";
      _is_message_received = true;
      if(_message_stack.size())
      {
        qDebug()<<"message stack not empty: "<<_message_stack[0];
        send_data(_message_stack[0]);
        _message_stack.remove(0);
      }
      break;

    default:
      if(_data.size() == NEED_SIMBOLS_TO_MOVE || (_data.toInt() >= MOVE && _data.toInt() <= NEW_GAME))
      {
        send_data(MESSAGE_RECEIVED);
        emit some_data_came();
      }
      else qDebug()<<"wrong message in read_data_from_udp()";
  }
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
  QChar first_data_simbol = QChar(data[0]);
  while(data.size() > 0 && first_data_simbol.isNumber())
  {
    serial_num.append(data[0]);
    data.remove(0,1);
    first_data_simbol = QChar(data[0]);
  }

  while(QChar(data[0]) == FREE_SPASE)
    data.remove(0,1);

  return serial_num;
}
