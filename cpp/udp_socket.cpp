#include <QObject>
#include <QUdpSocket>
#include <QByteArray>
#include <QTimer>
#include "udp_socket.h"
#include "enums.h"

UDP_socket::UDP_socket(QObject *parent) : QObject(parent), _socket(std::make_shared<QUdpSocket>(this)),
                                          _timer(std::make_shared<QTimer>(this)),
                                          _timer_from_last_received_message(std::make_shared<QTimer>(this)),
                                          _received_serial_num(0), _send_serial_num(0), _is_message_received(true),
                                          _server_port(FIRST_PORT), SERVER_IP(QHostAddress::LocalHost)
{
  for(unsigned i = 0; i + FIRST_PORT < LAST_PORT; ++i)
  {
    if(_socket->bind(SERVER_IP, FIRST_PORT + i))
    {
      _my_port = FIRST_PORT + i;
      qDebug()<<"UDP_socket::bind: "<<FIRST_PORT + i;
      break;
    }
    if(i + FIRST_PORT == LAST_PORT - 1)
     i = 0;
  }
  connect(_socket.get(), SIGNAL(readyRead()), this, SLOT(read_data()));
  connect(_timer.get(), SIGNAL(timeout()), this, SLOT(is_message_received()));
  connect(_timer_from_last_received_message.get(), SIGNAL(timeout()), this, SLOT(timer_from_last_received_message_timeout()));
}

UDP_socket::~UDP_socket()
{
}

void UDP_socket::send_data(const QByteArray &message, bool is_prev_serial_need)
{
  if(!is_message_received())
  {
    _send_message_stack.push_back(message);
    return;
  }

  _socket->writeDatagram(add_serial_num(message, is_prev_serial_need), SERVER_IP, _server_port);
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
      _send_message_stack.push_back(message);
      return;
    }
    begin_wait_receive(message);
  }

  _socket->writeDatagram(add_serial_num(message, is_prev_serial_need), SERVER_IP, _server_port);
}

void UDP_socket::read_data()
{
  QHostAddress sender_IP;
  quint16 sender_port;

  QByteArray message;
  message.resize(_socket->pendingDatagramSize());
  _socket->readDatagram(message.data(), message.size(), &sender_IP, &sender_port);

  const int serial_num = cut_serial_num(message);

  if(sender_IP != SERVER_IP || sender_port != _server_port || sender_port == _my_port)
  {
    qDebug()<<"Warning! in UDP_socket::read_data: Wrong sender!";
    return;
  }

  if(serial_num != ++_received_serial_num)
  {
    --_received_serial_num;
    if(serial_num == _received_serial_num && message.toInt() != Messages::MESSAGE_RECEIVED)
    {
      _timer_from_last_received_message->start(CHECK_CONNECT_TIME);
      send_data(Messages::MESSAGE_RECEIVED, true);
    }
    qDebug()<<"Warning! in UDP_socket::read_data: Wrong serial number!";
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
    const QString t = _last_send_message.mid(0, _last_send_message.indexOf(FREE_SPASE));
    if(t.toInt() == Messages::HELLO_SERVER)
      _server_port = sender_port;

      _is_message_received = true;
      _received_message_stack.push_back(QByteArray::number(Messages::SERVER_HERE));
  }
    _timer_from_last_received_message->start(CHECK_CONNECT_TIME);
}

QByteArray UDP_socket::pull_received_message()
{
  QByteArray mess_copy(_received_message_stack.first());
  _received_message_stack.removeFirst();
  return mess_copy;
}

bool UDP_socket::is_new_message_received() const
{
  return !_received_message_stack.isEmpty();
}

void UDP_socket::begin_wait_receive(const QByteArray& message)
{
  _is_message_received = false;
  _last_send_message = message;
  _timer->start(RESPONSE_WAIT_TIME);
}

void UDP_socket::timer_from_last_received_message_timeout()
{
  send_data(Messages::IS_SERVER_LOST);
}

bool UDP_socket::is_message_received()
{
  static int num_of_restarts = 0;
  if(_is_message_received)
  {
    _timer->stop();
    num_of_restarts = 0;
  }
  else
  {
    _timer->start(RESPONSE_WAIT_TIME);

    const QString t = _last_send_message.mid(0, _last_send_message.indexOf(FREE_SPASE));
    if(t.toInt() == Messages::HELLO_SERVER)
    {
      ++_server_port;
      if(_server_port == LAST_PORT)
        _server_port = FIRST_PORT;

      _timer->start(RESPONSE_WAIT_TIME/10);
    }

    _socket->writeDatagram(add_serial_num(_last_send_message, true), SERVER_IP, _server_port);

    if(_last_send_message.toInt() == Messages::IS_SERVER_LOST || num_of_restarts == 5)
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

int UDP_socket::cut_serial_num(QByteArray &data) const
{
  QByteArray serial_num(data.mid(0, data.indexOf(FREE_SPASE)));
  data.remove(0, data.indexOf(FREE_SPASE) + 1);
  return serial_num.toInt();
}
