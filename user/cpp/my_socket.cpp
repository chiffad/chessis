#include "my_socket.h"

#include <QObject>
#include <QUdpSocket>
#include <QByteArray>
#include <QTimer>

#include "enums.h"


using namespace inet;

struct my_socket::impl_t
{
  impl_t(QUdpSocket& s, QTimer& t, QTimer& connection_timer);
  void send(const QByteArray& message, bool is_prev_serial_need = false);
  void send(const Messages::MESSAGE r_mes, bool is_prev_serial_need = false);
  QByteArray pull();
  void read();
  bool is_message_received();
  void connection_checker_timer_timeout();
  bool is_message_append() const;
  QByteArray add_serial_num(const QByteArray& data, bool is_prev_serial_need = false);
  int cut_serial_num(QByteArray& data) const;
  void begin_wait_receive(const QByteArray& message);

  QUdpSocket& socket;
  QTimer& timer;
  QTimer& connection_checker_timer;

  QByteArray last_send_message;
  QVector<QByteArray> send_message_stack;
  QVector<QByteArray> received_message_stack;
  int received_serial_num;
  int send_serial_num;
  bool is_received;

  quint16 my_port;
  quint16 server_port;
  const QHostAddress SERVER_IP;

  enum { RESPONSE_WAIT_TIME = 1000, CHECK_CONNECT_TIME = 5000, FIRST_PORT = 49152, LAST_PORT = 49500 };
  const QChar FREE_SPASE = ' ';
};


my_socket::my_socket()
    : QObject(nullptr), impl(new impl_t(socket, timer, connection_checker_timer))
{
  connect(&socket, SIGNAL(readyRead()), this, SLOT(read()));
  connect(&timer, SIGNAL(timeout()), this, SLOT(is_message_received()));
  connect(&connection_checker_timer, SIGNAL(timeout()), this, SLOT(connection_checker_timer_timeout()));
}

my_socket::~my_socket()
{
}

void my_socket::send(const QByteArray &message, bool is_prev_serial_need)
{
  impl->send(message, is_prev_serial_need);
}

QByteArray my_socket::pull()
{
  return impl->pull();
}

bool my_socket::is_message_append() const
{
  return impl->is_message_append();
}

void my_socket::read()
{
  impl->read();
}

bool my_socket::is_message_received()
{
  return impl->is_message_received();
}

void my_socket::connection_checker_timer_timeout()
{
  impl->connection_checker_timer_timeout();
}

my_socket::impl_t::impl_t(QUdpSocket& s, QTimer& t, QTimer& connection_timer)
    : socket(s), timer(t), connection_checker_timer(connection_timer), received_serial_num(0),
      send_serial_num(0), is_received(true), server_port(FIRST_PORT), SERVER_IP(QHostAddress::LocalHost)
{
  for(unsigned i = 0; i + FIRST_PORT < LAST_PORT; ++i)
  {
    if(socket.bind(SERVER_IP, FIRST_PORT + i))
    {
      my_port = FIRST_PORT + i;
      qDebug()<<"UDP_socket::bind: "<<FIRST_PORT + i;
      break;
    }
    if(i + FIRST_PORT == LAST_PORT - 1)
     { i = 0; }
  }
}

void my_socket::impl_t::send(const QByteArray& message, bool is_prev_serial_need)
{
  if(!is_message_received())
  {
    send_message_stack.push_back(message);
    return;
  }

  qDebug()<<"sending:"<<message;

  socket.writeDatagram(add_serial_num(message, is_prev_serial_need), SERVER_IP, server_port);
  begin_wait_receive(message);
}

void my_socket::impl_t::send(const Messages::MESSAGE r_mes, bool is_prev_serial_need)
{
  QByteArray message;
  message.setNum(r_mes);

  if(r_mes != Messages::MESSAGE_RECEIVED)
  {
    if(!is_message_received())
    {
      send_message_stack.push_back(message);
      return;
    }
    begin_wait_receive(message);
  }

  qDebug()<<"sending:"<<message;

  socket.writeDatagram(add_serial_num(message, is_prev_serial_need), SERVER_IP, server_port);
}

void my_socket::impl_t::read()
{
  QHostAddress sender_IP;
  quint16 sender_port;

  QByteArray message;
  message.resize(socket.pendingDatagramSize());
  socket.readDatagram(message.data(), message.size(), &sender_IP, &sender_port);

qDebug()<<"read!!"<<message;

  const int serial_num = cut_serial_num(message);

  if(sender_IP != SERVER_IP || sender_port != server_port || sender_port == my_port)
  {
    qDebug()<<"Warning! in UDP_socket::read_data: Wrong sender!"<<message;
    return;
  }

  if(serial_num != ++received_serial_num)
  {
    --received_serial_num;
    if(serial_num == received_serial_num && message.toInt() != Messages::MESSAGE_RECEIVED)
    {
      connection_checker_timer.start(CHECK_CONNECT_TIME);
      send(Messages::MESSAGE_RECEIVED, true);
    }
    qDebug()<<"Warning! in UDP_socket::read_data: Wrong serial number!";
    return;
  }

  if(message.toInt() != Messages::MESSAGE_RECEIVED)
  {
    send(Messages::MESSAGE_RECEIVED);
    if(message.toInt() != Messages::CLIENT_LOST)
      { received_message_stack.push_back(message); }
  }
  else
  {
    const QString t = last_send_message.mid(0, last_send_message.indexOf(FREE_SPASE));
    if(t.toInt() == Messages::HELLO_SERVER)
      { server_port = sender_port; }

    is_received = true;
    received_message_stack.push_back(QByteArray::number(Messages::SERVER_HERE));
  }
  connection_checker_timer.start(CHECK_CONNECT_TIME);
}

QByteArray my_socket::impl_t::pull()
{
  QByteArray m(received_message_stack.first());
  received_message_stack.removeFirst();

  return m;
}

bool my_socket::impl_t::is_message_append() const
{
  return !received_message_stack.isEmpty();
}

void my_socket::impl_t::begin_wait_receive(const QByteArray& message)
{
  is_received = false;
  last_send_message = message;
  timer.start(RESPONSE_WAIT_TIME);
}

void my_socket::impl_t::connection_checker_timer_timeout()
{
  send(Messages::IS_SERVER_LOST);
}

bool my_socket::impl_t::is_message_received()
{
  static int num_of_restarts = 0;
  if(is_received)
  {
    timer.stop();
    num_of_restarts = 0;
  }
  else
  {
    timer.start(RESPONSE_WAIT_TIME);

    const QString t = last_send_message.mid(0, last_send_message.indexOf(FREE_SPASE));
    if(t.toInt() == Messages::HELLO_SERVER)
    {
      ++server_port;
      if(server_port == LAST_PORT)
        { server_port = FIRST_PORT; }

      timer.start(RESPONSE_WAIT_TIME/10);
    }

    socket.writeDatagram(add_serial_num(last_send_message, true), SERVER_IP, server_port);

    if(last_send_message.toInt() == Messages::IS_SERVER_LOST || num_of_restarts == 5)
      { received_message_stack.push_back(QByteArray::number(Messages::SERVER_LOST)); }

    ++num_of_restarts;
  }
  return is_received;
}

QByteArray my_socket::impl_t::add_serial_num(const QByteArray &data, bool is_prev_serial_need)
{
  if(!is_prev_serial_need)
    { ++send_serial_num; }

  QByteArray message;
  message.setNum(send_serial_num);
  message.append(FREE_SPASE);
  message.append(data);

  return message;
}

int my_socket::impl_t::cut_serial_num(QByteArray &data) const
{
  QByteArray serial_num(data.mid(0, data.indexOf(FREE_SPASE)));
  data.remove(0, data.indexOf(FREE_SPASE) + 1);

  return serial_num.toInt();
}
