#include "server.h"

#include <QObject>
#include <QUdpSocket>

#include "log.h"

#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

#include <iostream>

using namespace sr;



struct server_t::impl_t
{
  impl_t(boost::asio::io_service& io_serv);                                                   //boost
  ~impl_t(); //boost
//  void send(const QByteArray& message, const int port, const QHostAddress& ip);
//  void send(const std::string& message, const boost::asio::ip::udp::endpoint& destination);      //boost
 void start_receive();                                                                            //boost
  std::vector<datagram_t> pull();
  //void read();

 void handle_receive(const boost::system::error_code& error,// sheet
      std::size_t type/*bytes_transferred*/);

//  QUdpSocket socket;
  boost::asio::ip::udp::socket socket;                                                            //boost
  std::vector<datagram_t> messages;

  boost::array<char, 1> recv_buffer;
    boost::asio::ip::udp::endpoint sender;

};

server_t::server_t(boost::asio::io_service& io_serv)                                             //boost
    : impl(std::make_unique<impl_t>(io_serv))
{
}

server_t::~server_t()
{
}

/*void server_t::send(const QByteArray& message, const int port, const QHostAddress& ip)
{
  impl->send(message, port, ip);
}*/

/*void server_t::send(const std::string& message, const boost::asio::ip::udp::endpoint& destination); //boost
{
  impl->send(message, destination);
}*/

std::vector<server_t::datagram_t> server_t::pull()
{
  return impl->pull();
}

server_t::impl_t::impl_t(boost::asio::io_service& io_serv)                                         //boost
    : socket(io_serv)
{
  /*QObject::connect(&socket, &QUdpSocket::readyRead, [&](){read();});

  enum { FIRST_PORT = 49152, LAST_PORT = 49500 };

  const QHostAddress SERVER_IP = QHostAddress::LocalHost;

  for(int i = 0; i + FIRST_PORT < LAST_PORT; ++i)
  {
    if(socket.bind(SERVER_IP, FIRST_PORT + i))
    {
      log("server bind: ", FIRST_PORT + i);
      break;
    }
    if(i + FIRST_PORT == LAST_PORT)
      { i = -1; }
  }*/

  enum { FIRST_PORT = 49152, LAST_PORT = 49500 };

  socket.open(boost::asio::ip::udp::v4());                                                                   //boost
  const auto MY_IP = boost::asio::ip::address::from_string("127.0.0.1");

  for(int i = 0; i + FIRST_PORT < LAST_PORT; ++i)
  {
    try
    {
      socket.bind(boost::asio::ip::udp::endpoint(MY_IP, FIRST_PORT + i));
    }
    catch(const boost::system::system_error& ex)
    {
      log("can not bind to: ", FIRST_PORT + i);
      if(i + FIRST_PORT == LAST_PORT)
        { i = -1; }

      continue;
    }
    log("server bind: ", FIRST_PORT + i);
    break;
  }
  start_receive();
}

server_t::impl_t::~impl_t() //boost
{
  socket.close();
}

/*void server_t::impl_t::send(const std::string& message, const boost::asio::ip::udp::endpoint& destination); //boost
{
  log("send: ", message);
  socket.async_send_to(boost::asio::buffer(message), destination);
}*/

void server_t::impl_t::start_receive() //boost
{

sr::log("!!!!1");

 socket.async_receive_from(
        boost::asio::buffer(recv_buffer), sender,
        boost::bind(&server_t::impl_t::start_receive, this));
//,          boost::asio::placeholders::error,
//          boost::asio::placeholders::bytes_transferred));

std::string str(recv_buffer.begin(), recv_buffer.end());
std::cout<<str<<std::endl;
//  sr::log(datagram.message);
}

/*void server_t::impl_t::send(const QByteArray& message, const int port, const QHostAddress& ip)
{
  log("send: ", message);
  socket.writeDatagram(message, ip, port);
}*/

std::vector<server_t::datagram_t> server_t::impl_t::pull()
{
  auto _1 = messages;
  messages.clear();

  return _1;
}

/*void server_t::impl_t::read()
{
  QHostAddress ip;
  quint16 port;
  QByteArray m;

  m.resize(socket.pendingDatagramSize());
  socket.readDatagram(m.data(), m.size(), &ip, &port);

  log("read:", m);

  datagram_t d;
  d.ip = ip;
  d.port = port;
  d.message = m;

  messages.push_back(d);
}*/

