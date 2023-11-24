#include "server/authentication/server.hpp"

#include <array>
#include <boost/bind/bind.hpp>
#include <spdlog/spdlog.h>

namespace {
const int FIRST_PORT = 49152;
const int LAST_PORT = 49300;
} // namespace

namespace chess::server::authentication {

struct server_t::impl_t
{
  impl_t(io_service_t& io_serv, user::users_data_manager_t& users_data_manager, const endpoint_t& logic_server_endpoint, const server_t::client_authenticated_callback_t& callback)
    : socket_{io_serv}
    , message_handler_{users_data_manager, logic_server_endpoint, callback, [this](const std::string& message, const endpoint_t& destination) { send(message, destination); }}
  {
    while (!socket_.is_open())
    {
      socket_.open(boost::asio::ip::udp::v4());
    }
    for (int i = FIRST_PORT; i < LAST_PORT; i = i >= LAST_PORT ? FIRST_PORT : i + 1)
    {
      try
      {
        socket_.bind(endpoint_t(boost::asio::ip::address_v4::loopback(), i));
        break;
      }
      catch (const boost::system::system_error& ex)
      {
        SPDLOG_INFO("can not bind to port={}", i);
      }
    }

    SPDLOG_INFO("Authentication server started on endpoint={}", socket_.local_endpoint());
    start_receive();
  }

  ~impl_t() { socket_.close(); }

  void send(const std::string& message, const endpoint_t& destination)
  {
    SPDLOG_TRACE("send={}; to={}", message, destination);
    socket_.async_send_to(boost::asio::buffer(message), destination, [](auto, auto) {});
  }

  void handle_receive(const error_code_t& e, const size_t readed_size)
  {
    if (e && e != boost::asio::error::message_size)
    {
      SPDLOG_ERROR("hendle error!!");
      return;
    }

    const std::string mess(incoming_message_.begin(), incoming_message_.begin() + readed_size);
    SPDLOG_TRACE("read={}", mess);
    message_handler_.handle(last_mess_sender_, mess);
    start_receive();
  }

  void start_receive()
  {
    socket_.async_receive_from(boost::asio::buffer(incoming_message_), last_mess_sender_,
                               boost::bind(&server_t::impl_t::handle_receive, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
  }

  boost::asio::ip::udp::socket socket_;
  std::array<char, 1024> incoming_message_;
  endpoint_t last_mess_sender_;

  message_handler_t message_handler_;
};

server_t::server_t(io_service_t& io_serv, user::users_data_manager_t& users_data_manager, const endpoint_t& logic_server_endpoint, const client_authenticated_callback_t& callback)
  : impl_(std::make_unique<impl_t>(io_serv, users_data_manager, logic_server_endpoint, callback))
{}

server_t::~server_t() = default;

} // namespace chess::server::authentication