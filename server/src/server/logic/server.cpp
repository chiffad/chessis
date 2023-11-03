#include "server/logic/server.hpp"
#include "server/logic/clients_holder.hpp"

#include <boost/bind/bind.hpp>
#include <messages/messages_io.hpp>
#include <spdlog/spdlog.h>

namespace chess::server::logic {

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

  void handle_receive(const error_code_t& e, const size_t readed_size)
  {
    if (e && e != boost::asio::error::message_size)
    {
      SPDLOG_ERROR("hendle error!!");
      return;
    }

    const std::string mess(incoming_message_.begin(), incoming_message_.begin() + readed_size);
    SPDLOG_INFO("read={}", mess);

    try
    {
      auto tokenized_msg = msg::init<msg::tokenized_msg_t>(mess);
      clients_holder_.at(chess::common::from_msg_token(tokenized_msg.token)).message_received(last_mess_sender_, std::move(tokenized_msg.data));
    }
    catch (const std::exception& ex)
    {
      SPDLOG_ERROR("Failed to process mess={} from address={}; ex={}!!!", mess, last_mess_sender_, ex.what());
    }
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

void server_t::add_client(const client_uuid_t& uuid, const endpoint_t& addr)
{
  impl_->clients_holder_.add(uuid, addr);
}

void server_t::send(const msg::some_datagram_t& message, const client_uuid_t& client_uuid)
try
{
  impl_->clients_holder_.at(client_uuid).push_for_send(message);
}
catch (...)
{
  SPDLOG_ERROR("Unable to send message to client={}; No client found! message={}", message, client_uuid);
}

void server_t::process()
{
  for (const auto& datagram : impl_->clients_holder_.datagrams_to_send())
  {
    SPDLOG_INFO("send datagram={}", datagram);
    impl_->socket_.async_send_to(boost::asio::buffer(datagram.message), datagram.address, [](auto /*_1*/, auto /*_2*/) {});
  }
}

std::map<client_uuid_t, std::vector<msg::some_datagram_t>> server_t::read()
{
  return impl_->clients_holder_.datagrams_to_process();
}

endpoint_t server_t::address() const
{
  return impl_->socket_.local_endpoint();
}

} // namespace chess::server::logic