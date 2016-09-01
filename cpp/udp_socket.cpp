#include <QObject>
#include <QUdpSocket>
#include <QByteArray>
#include <QTimer>
#include "udp_socket.h"
#include "enums.h"

UDP_socket::UDP_socket(QObject *parent) : QObject(parent), _socket(new QUdpSocket(this)), _timer(new QTimer(this)),
                                          _timer_from_last_received_message(new QTimer(this)),
                                          _received_serial_num(0), _send_serial_num(0), _is_message_received(true),
                                          SERVER_PORT(49238), SERVER_IP(QHostAddress::LocalHost)
{
  connect(_timer, SIGNAL(timeout()), this, SLOT(is_message_received()));
  connect(_timer_from_last_received_message, SIGNAL(timeout()), this, SLOT(timer_from_last_received_message_timeout()));

  qDebug()<<"is bind:"<<_socket->bind();
  connect(_socket, SIGNAL(readyRead()), this, SLOT(read_data()));

  send_data(Messages::HELLO_SERVER);
}

UDP_socket::~UDP_socket()
{
  delete _socket;
  delete _timer;
  delete _timer_from_last_received_message;
}

void UDP_socket::send_data(const QByteArray &message, bool is_prev_serial_need)
{
  if(!is_message_received())
  {
    qDebug()<<"can't send, prev message not reach";
    _send_message_stack.push_back(message);
    return;
  }

  qDebug()<<"====Sending data to server"<<message;
  _socket->writeDatagram(add_serial_num(message, is_prev_serial_need), SERVER_IP, SERVER_PORT);
  begin_wait_receive(message);
}

void UDP_socket::send_data(const Messages::MESSAGE r_mes, bool is_prev_serial_need)
{
  QByteArray message;
  message.setNum(r_mes);

  if(r_mes != Messages::MESSAGE_RECEIVED)
  {
    if(!is_message_received())
    {
      qDebug()<<"can't send, last message not reach";
      _send_message_stack.push_back(message);
      return;
    }
    begin_wait_receive(message);
  }

  qDebug()<<"====Sending data to server"<<message;
  _socket->writeDatagram(add_serial_num(message, is_prev_serial_need), SERVER_IP, SERVER_PORT);
}

void UDP_socket::read_data()
{
  QHostAddress sender_IP;
  quint16 sender_port;

  QByteArray message;
  message.resize(_socket->pendingDatagramSize());
  _socket->readDatagram(message.data(), message.size(), &sender_IP, &sender_port);

  qDebug()<<"====socket read"<<message;

  const QByteArray serial_num = cut_serial_num(message);

 /* if(sender_IP != SERVER_IP || sender_port != SERVER_PORT || _last_send_message == message)
  {
    qDebug()<<"wrong sender!"<< (sender_port != SERVER_PORT)<<" "<<(sender_IP != SERVER_IP);
    qDebug()<<SERVER_IP<<""<<sender_IP;
    return;
  }*/

 // qDebug()<<"!serial_num"<<serial_num;

  if(serial_num.toInt() != ++_received_serial_num)
  {
    --_received_serial_num;
    if(serial_num.toInt() == _received_serial_num && message.toInt() != Messages::MESSAGE_RECEIVED)
    {
      _timer_from_last_received_message->start(FIVE_SEC);
      send_data(Messages::MESSAGE_RECEIVED, true);
      qDebug()<<"prev serial num. Resent message";
    }
    else qDebug()<<"serial_num is wrong";
    return;
  }

  if(message.toInt() != Messages::MESSAGE_RECEIVED)
  {
    send_data(Messages::MESSAGE_RECEIVED);
    if(message.toInt() != Messages::CLIENT_LOST)
      _received_message_stack.push_back(message);
  }
  else
  {
    _is_message_received = true;
    _received_message_stack.push_back(QByteArray::number(Messages::SERVER_HERE));
  }

  _timer_from_last_received_message->start(FIVE_SEC);
}

QByteArray UDP_socket::pull_received_message()
{
  qDebug()<<"===pull_received_message()";
  QByteArray mess_copy(_received_message_stack.first());
  _received_message_stack.removeFirst();
  return mess_copy;
}

bool UDP_socket::is_new_message_received() const
{
  //qDebug()<<"=====is_new_message_received:"<<!_received_message_stack.isEmpty();
  return !_received_message_stack.isEmpty();
}

void UDP_socket::begin_wait_receive(const QByteArray& message)
{
  qDebug()<<"====begin waight";
  _is_message_received = false;
  _last_send_message = message;
  _timer->start(SECOND);
}

void UDP_socket::timer_from_last_received_message_timeout()
{
  qDebug()<<"is server lost?";
  send_data(Messages::IS_SERVER_LOST);
}

bool UDP_socket::is_message_received()
{
  static int num_of_restarts = 0;
  qDebug()<<"===checked_message_received:"<<_is_message_received;
  if(_is_message_received)
  {
    _timer->stop();
    num_of_restarts = 0;
  }
  else
  {
    qDebug()<<"timer restart";
    _timer->start(SECOND);
    _socket->writeDatagram(add_serial_num(_last_send_message, true), SERVER_IP, SERVER_PORT);
    if(_last_send_message.toInt() == Messages::IS_SERVER_LOST || num_of_restarts > 5)
      _received_message_stack.push_back(QByteArray::number(Messages::SERVER_LOST));

    ++num_of_restarts;
  }
  return _is_message_received;
}

QByteArray UDP_socket::add_serial_num(const QByteArray &data, bool is_prev_serial_need)
{
  if(!is_prev_serial_need)
     ++_send_serial_num;

  QByteArray message;
  message.setNum(_send_serial_num);
  message.append(FREE_SPASE);
  message.append(data);

  return message;
}

QByteArray UDP_socket::cut_serial_num(QByteArray &data) const
{
  QByteArray serial_num(data.mid(0, data.indexOf(FREE_SPASE)));
  data.remove(0, data.indexOf(FREE_SPASE) + 1);
  return serial_num;
}