#include <QObject>
#include <QUdpSocket>
#include <QByteArray>
#include <QTimer>
#include "headers/udp_client.h"

UDP_client::UDP_client(QObject *parent) : QObject(parent), _received_serial_num(0), _send_serial_num(0),
                                          _is_message_received(true), SERVER_PORT(1234), SERVER_IP(QHostAddress::LocalHost)
{
  _timer = new QTimer(this);
  _timer_from_last_received_message = new QTimer(this);
  connect(_timer, SIGNAL(timeout()), this, SLOT(checked_is_message_received()));
  connect(_timer_from_last_received_message, SIGNAL(timeout()), this, SLOT(timer_from_last_received_message_timeout()));

  _socket = new QUdpSocket(this);
  _socket->bind(SERVER_IP, SERVER_PORT);
  connect(_socket, SIGNAL(readyRead()), this, SLOT(read_data()));

  send_data(HELLO_SERVER);
}

void UDP_client::send_data(QByteArray& message, bool is_prev_serial_need)
{
  /*if(!checked_is_message_received())
  {
    qDebug()<<"can't send, prev message not reach";
    _message_stack.push_back(message);
    return;
  }

  add_serial_num(message, is_prev_serial_need);

  qDebug()<<"====Sending data to server"<<message;
  _socket->writeDatagram(message, SERVER_IP, SERVER_PORT);
  begin_wait_receive(message);*/
}

void UDP_client::send_data(REQUEST_MESSAGES r_mes, bool is_prev_serial_need)
{
  /*QByteArray message;
  message.setNum(r_mes);

  if(r_mes != MESSAGE_RECEIVED)
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
    begin_wait_receive(message);*/
}

void UDP_client::read_data()
{
  qDebug()<<"=====socket read"<<_data;
  QHostAddress sender_IP;
  quint16 sender_port;

  _data.resize(_socket->pendingDatagramSize());
  _socket->readDatagram(_data.data(), _data.size(), &sender_IP, &sender_port);

  if(sender_IP != SERVER_IP || sender_port != SERVER_PORT || _last_send_message == _data)
  {
    qDebug()<<"wrong sender!";
    return;
  }

  QByteArray serial_num = cut_serial_num(_data);

  if(serial_num.toInt() != ++_received_serial_num)
  {
    --_received_serial_num;
    if(serial_num.toInt() == _received_serial_num && _data.toInt() != MESSAGE_RECEIVED)
    {
      _timer_from_last_received_message->start(TEN_SEC);
      send_data(MESSAGE_RECEIVED, true);
      qDebug()<<"prev serial num. Resent message";
    }
    else qDebug()<<"serial_num is wrong";
    return;
  }

  qDebug()<<"received: "<<_data;

  QByteArray message_type;
  for(int i = 0; QChar(_data[i]) != FREE_SPASE && _data.size() < i; ++i)
    message_type.append(_data[i]);

  qDebug()<<"message_type: "<<message_type.toInt();

  switch(message_type.toInt())
  {
    case CLIENT_LOST:
      break;
    case OPPONENT_LOST_FROM_SERVER:
      set_data_and_emit_to_chess(OPPONENT_LOST);
      break;
    case GO_TO_HISTORY:
      emit some_data_came();
      break;
    default:
      if(_data.size() == NEED_SIMBOLS_TO_MOVE || (_data.toInt() >= MOVE && _data.toInt() <= NEW_GAME))
      {
        qDebug()<<"emit_data_came";
        emit some_data_came();
      }
      else
      {
        if(message_type.toInt() == MESSAGE_RECEIVED)
        {
          qDebug()<<"Message received";
          _is_message_received = true;
          if(_message_stack.size())
          {
            qDebug()<<"message stack not empty: "<<_message_stack[0];
            send_data(_message_stack[0]);
            _message_stack.remove(0);
          }
        }
        else qDebug()<<"wrong message in read_data_from_udp()";
        return;
      }
  }
  send_data(MESSAGE_RECEIVED);
  _timer_from_last_received_message->start(TEN_SEC);
}

void UDP_client::set_data_and_emit_to_chess(CHESS_MESSAGE_TYPE message)
{
  _data.clear();
  _data.setNum(message);
  emit some_data_came();
}

void UDP_client::begin_wait_receive(const QByteArray& message)
{
  qDebug()<<"====begin waight";
  _is_message_received = false;
  _last_send_message = message;
  _timer->start(SECOND);
}

void UDP_client::timer_from_last_received_message_timeout()
{
  qDebug()<<"is server lost?";
  send_data(IS_SERVER_LOST);
}

bool UDP_client::checked_is_message_received()
{
  static int num_of_restarts = 0;
  qDebug()<<"===checked_message_received";
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
    if(_last_send_message.toInt() == IS_SERVER_LOST || num_of_restarts > 5)
      set_data_and_emit_to_chess(SERVER_LOST);

    ++num_of_restarts;
  }
  return _is_message_received;
}

void UDP_client::export_readed_data_to_chess(QString& move) const
{
  move.clear();
  move.append(_data);
}

void UDP_client::add_serial_num(QByteArray& data, bool is_prev_serial_need)
{
  if(!is_prev_serial_need)
     ++_send_serial_num;

  QByteArray serial_num;
  serial_num.setNum(_send_serial_num);
  serial_num.append(FREE_SPASE);

  data.prepend(serial_num);
}

QByteArray UDP_client::cut_serial_num(QByteArray& data) const
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
