#include "server/server.hpp"

#include "helper.h"

#include <boost/array.hpp>
#include <boost/bind.hpp>

#include <spdlog/spdlog.h>

namespace server {
struct server_t::impl_t
{
  impl_t(io_service_t& io_serv);
  ~impl_t();
  void start_receive();
  void send(const std::string& message, const endpoint_t& destination);
  std::vector<datagram_t> pull();
  void handle_receive(const error_code_t& e, const size_t readed_size);

  boost::asio::ip::udp::socket socket_;
  std::vector<datagram_t> messages_;

  std::array<char, 1024> incoming_message_;
  endpoint_t last_mess_sender_;
};

server_t::datagram_t::datagram_t(const endpoint_t& addr, const std::string& mess)
  : address(addr)
  , message(mess)
{}

server_t::server_t(io_service_t& io_serv)
  : impl_(std::make_unique<impl_t>(io_serv))
{}

server_t::~server_t()
{}

void server_t::send(const std::string& message, const endpoint_t& destination)
{
  impl_->send(message, destination);
}

std::vector<server_t::datagram_t> server_t::pull()
{
  return impl_->pull();
}

server_t::impl_t::impl_t(io_service_t& io_serv)
  : socket_(io_serv)
{
  enum
  {
    FIRST_PORT = 49152,
    LAST_PORT = 49300
  };

  while (!socket_.is_open())
  {
    socket_.open(boost::asio::ip::udp::v4());
  }
  for (int i = FIRST_PORT; i < LAST_PORT; i = i >= LAST_PORT ? FIRST_PORT : i + 1)
  {
    try
    {
      socket_.bind(endpoint_t(boost::asio::ip::address::from_string("127.0.0.1"), i));
      SPDLOG_INFO("server bind to port={}", i);
      break;
    }
    catch (const boost::system::system_error& ex)
    {
      SPDLOG_INFO("can not bind to port={}", i);
    }
  }
  start_receive();
}

server_t::impl_t::~impl_t()
{
  socket_.close();
}

void server_t::impl_t::send(const std::string& message, const endpoint_t& destination)
{
  SPDLOG_INFO("send={}; to={}", message, destination.address().to_string());
  socket_.async_send_to(boost::asio::buffer(message), destination, [](auto /*_1*/, auto /*_2*/) {});
}

void server_t::impl_t::handle_receive(const error_code_t& e, const size_t readed_size)
{
  if (!e || e == boost::asio::error::message_size)
  {
    std::string mess(incoming_message_.begin(), incoming_message_.begin() + readed_size);
    SPDLOG_INFO("read={}", mess);

    messages_.push_back(datagram_t(last_mess_sender_, mess));
    start_receive();
  }
  else
  {
    SPDLOG_ERROR("hendle error!!");
  }
}

void server_t::impl_t::start_receive()
{
  SPDLOG_INFO("start_receive()");
  socket_.async_receive_from(
    boost::asio::buffer(incoming_message_), last_mess_sender_,
    boost::bind(&server_t::impl_t::handle_receive, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

std::vector<server_t::datagram_t> server_t::impl_t::pull()
{
  auto _1 = messages_;
  messages_.clear();

  return _1;
}

} // namespace server