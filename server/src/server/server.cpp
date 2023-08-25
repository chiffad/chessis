#include "server/server.hpp"
#include "server/clients_holder.hpp"

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
  void handle_receive(const error_code_t& e, const size_t readed_size);

  boost::asio::ip::udp::socket socket_;
  clients_holder_t clients_holder_;

  std::array<char, 1024> incoming_message_;
  endpoint_t last_mess_sender_;
};

server_t::server_t(io_service_t& io_serv)
  : impl_(std::make_unique<impl_t>(io_serv))
{}

server_t::~server_t() = default;

void server_t::send(const std::string& message, const endpoint_t& destination)
{
  const auto cl_it = impl_->clients_holder_.find(destination);
  if (cl_it == impl_->clients_holder_.end())
  {
    SPDLOG_ERROR("Unable to send message to destination={}; No client found! message={}", message, destination);
    return;
  }

  cl_it->second.push_for_send(message);
}

void server_t::process()
{
  for (const auto& datagram : impl_->clients_holder_.datagrams_to_send())
  {
    impl_->send(datagram.message, datagram.address);
  }
}

std::vector<datagram_t> server_t::read()
{
  return impl_->clients_holder_.datagrams_to_process();
}

server_t::impl_t::impl_t(io_service_t& io_serv)
  : socket_{io_serv}
  , clients_holder_{io_serv}
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
  if (e && e != boost::asio::error::message_size)
  {
    SPDLOG_ERROR("hendle error!!");
    return;
  }

  const std::string mess(incoming_message_.begin(), incoming_message_.begin() + readed_size);
  SPDLOG_INFO("read={}", mess);
  clients_holder_.process(datagram_t{last_mess_sender_, mess});
  start_receive();
}

void server_t::impl_t::start_receive()
{
  SPDLOG_TRACE("start_receive()");
  socket_.async_receive_from(
    boost::asio::buffer(incoming_message_), last_mess_sender_,
    boost::bind(&server_t::impl_t::handle_receive, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

} // namespace server