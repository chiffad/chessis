#include "server/logic/server.hpp"
#include "server/logic/clients_holder.hpp"

#include <boost/bind/bind.hpp>
#include <spdlog/spdlog.h>

namespace server::logic {
struct server_t::impl_t
{
  impl_t(io_service_t& io_serv, const clients_holder_t::connection_status_signal_t::slot_type& subscriber)
    : socket_{io_serv}
    , clients_holder_{io_serv, subscriber}
  {
    while (!socket_.is_open())
    {
      socket_.open(boost::asio::ip::udp::v4());
    }

    socket_.bind(endpoint_t{boost::asio::ip::address_v4::loopback(), 0});
    SPDLOG_INFO("Logic server started on endpoint={}", socket_.local_endpoint());
    start_receive();
  }

  ~impl_t() { socket_.close(); }

  // void send(const std::string& message, const endpoint_t& destination)
  //{
  //   SPDLOG_INFO("send={}; to={}", message, destination.address().to_string());
  //   socket_.async_send_to(boost::asio::buffer(message), destination, [](auto /*_1*/, auto /*_2*/) {});
  // }

  void handle_receive(const error_code_t& e, const size_t readed_size)
  {
    if (e && e != boost::asio::error::message_size)
    {
      SPDLOG_ERROR("hendle error!!");
      return;
    }

    const std::string mess(incoming_message_.begin(), incoming_message_.begin() + readed_size);
    SPDLOG_INFO("read={}", mess);
    clients_holder_.process(datagram_t<std::string>{last_mess_sender_, mess});
    start_receive();
  }

  void start_receive()
  {
    SPDLOG_TRACE("start_receive()");
    socket_.async_receive_from(boost::asio::buffer(incoming_message_), last_mess_sender_,
                               boost::bind(&server_t::impl_t::handle_receive, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
  }

  boost::asio::ip::udp::socket socket_;
  clients_holder_t clients_holder_;

  std::array<char, 1024> incoming_message_;
  endpoint_t last_mess_sender_;
};

server_t::server_t(io_service_t& io_serv, const clients_holder_t::connection_status_signal_t::slot_type& subscriber)
  : impl_(std::make_unique<impl_t>(io_serv, subscriber))
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
    SPDLOG_INFO("send datagram={}", datagram);
    impl_->socket_.async_send_to(boost::asio::buffer(datagram.message), datagram.address, [](auto /*_1*/, auto /*_2*/) {});

    // impl_->send(datagram.message, datagram.address);
  }
}

std::vector<datagram_t<msg::some_datagram_t>> server_t::read()
{
  return impl_->clients_holder_.datagrams_to_process();
}

endpoint_t server_t::address() const
{
  return impl_->socket_.local_endpoint();
}

} // namespace server