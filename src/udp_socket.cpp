#include <QObject>
#include <QUdpSocket>
#include <QByteArray>
#include <QTimer>
#include "headers/udp_socket.h"
#include "headers/enums.h"

UDP_socket::UDP_socket(QObject *parent) : QObject(parent), _socket(new QUdpSocket(this)), _timer(new QTimer(this)),
                                        _timer_from_last_received_message(new QTimer(this)),
                                        _received_serial_num(0), _send_serial_num(0), _is_message_received(true),
                                        SERVER_PORT(12345), SERVER_IP(QHostAddress::LocalHost)
{
  connect(_timer, SIGNAL(timeout()), this, SLOT(is_message_received()));
  connect(_timer_from_last_received_message, SIGNAL(timeout()), this, SLOT(timer_from_last_received_message_timeout()));

  _socket->bind(SERVER_IP, SERVER_PORT);
  connect(_socket, SIGNAL(readyRead()), this, SLOT(read_data()));

  send_data(Messages::HELLO_SERVER);
}

void UDP_socket::send_data(QByteArray message, bool is_prev_serial_need)
{
  if(!is_message_received())
  {
    qDebug()<<"can't send, prev message not reach";
    _send_message_stack.push_back(message);
    return;
  }

  add_serial_num(message, is_prev_serial_need);

  qDebug()<<"====Sending data to server"<<message;
  _socket->writeDatagram(message, SERVER_IP, SERVER_PORT);
  begin_wait_receive(message);
}

void UDP_socket::send_data(const Messages::MESSAGE r_mes, bool is_prev_serial_need)
{
  QByteArray message;
  message.setNum(r_mes);

  if(r_mes != Messages::MESSAGE_RECEIVED)
    if(!is_message_received())
    {
      qDebug()<<"can't send, last message not reach";
      _send_message_stack.push_back(message);
      return;
    }

  add_serial_num(message, is_prev_serial_need);

  qDebug()<<"====Sending data to server"<<message;
  _socket->writeDatagram(message, SERVER_IP, SERVER_PORT);
  if(r_mes != Messages::MESSAGE_RECEIVED)
    begin_wait_receive(message);
}

void UDP_socket::read_data()
{
  QHostAddress sender_IP;
  quint16 sender_port;

  QByteArray message;
  message.resize(_socket->pendingDatagramSize());
  _socket->readDatagram(message.data(), message.size(), &sender_IP, &sender_port);

  qDebug()<<"====socket read"<<message;

  if(sender_IP != SERVER_IP || sender_port != SERVER_PORT || _last_send_message == message)
  {
    qDebug()<<"wrong sender!";
    return;
  }

  QByteArray serial_num = cut_serial_num(message);
  if(serial_num.toInt() != ++_received_serial_num)
  {
    --_received_serial_num;
    if(serial_num.toInt() == _received_serial_num && message.toInt() != Messages::MESSAGE_RECEIVED)
    {
      _timer_from_last_received_message->start(TEN_SEC);
      send_data(Messages::MESSAGE_RECEIVED, true);
      qDebug()<<"prev serial num. Resent message";
    }
    else qDebug()<<"serial_num is wrong";
    return;
  }

  if(message.toInt() != Messages::MESSAGE_RECEIVED)
  {
    send_data(Messages::MESSAGE_RECEIVED);
    _received_message_stack.push_back(message);
  }
  else _is_message_received = true;

  _timer_from_last_received_message->start(TEN_SEC);
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
    _socket->writeDatagram(_last_send_message, SERVER_IP, SERVER_PORT);
    if(_last_send_message.toInt() == Messages::IS_SERVER_LOST || num_of_restarts > 5)
      _received_message_stack.push_back(QByteArray::number(Messages::SERVER_LOST));

    ++num_of_restarts;
  }
  return _is_message_received;
}

void UDP_socket::add_serial_num(QByteArray &data, bool is_prev_serial_need)
{
  if(!is_prev_serial_need)
     ++_send_serial_num;

  QByteArray serial_num;
  serial_num.setNum(_send_serial_num);
  serial_num.append(FREE_SPASE);

  data.prepend(serial_num);
}

QByteArray UDP_socket::cut_serial_num(QByteArray &data) const
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
