#include "server.h"

#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <iostream>

#include "log.h"


using namespace sr;

struct server_t::impl_t
{
  impl_t(boost::asio::io_service& io_serv);
  ~impl_t();
  void send(const std::string& message, const boost::asio::ip::udp::endpoint& destination);
  void start_receive();
  std::vector<datagram_t> pull();

  boost::asio::ip::udp::socket socket;
  std::vector<datagram_t> messages;
};

server_t::server_t(boost::asio::io_service& io_serv)
    : impl(std::make_unique<impl_t>(io_serv))
{
}

server_t::~server_t()
{
}

void server_t::send(const std::string& message, const boost::asio::ip::udp::endpoint& destination)
{
  impl->send(message, destination);
}

std::vector<server_t::datagram_t> server_t::pull()
{
  return impl->pull();
}

server_t::impl_t::impl_t(boost::asio::io_service& io_serv)
    : socket(io_serv)
{
  enum { FIRST_PORT = 49152, LAST_PORT = 49500 };

  socket.open(boost::asio::ip::udp::v4());
  for(int i = 0; i + FIRST_PORT < LAST_PORT; ++i)
  {
    try
      { socket.bind(boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), FIRST_PORT + i)); }
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

server_t::impl_t::~impl_t()
{
  socket.close();
}

void server_t::impl_t::send(const std::string& message, const boost::asio::ip::udp::endpoint& destination)
{
  log("send: ", message);
  socket.send_to(boost::asio::buffer(message), destination);
}

void server_t::impl_t::start_receive()
{
  sr::log("start_receive()");

  boost::array<char, 1> recv_buffer;
  datagram_t data;

  socket.receive_from(boost::asio::buffer(recv_buffer), data.sender);

  data.message = std::string(recv_buffer.begin(), recv_buffer.end());
  sr::log("received:" + data.message);
  messages.push_back(data);
  start_receive();
}

std::vector<server_t::datagram_t> server_t::impl_t::pull()
{
  auto _1 = messages;
  messages.clear();

  return _1;
}

