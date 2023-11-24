#include "server/authentication/message_handler.hpp"
#include "messages/messages.hpp"
#include "user_data/credentials.hpp"
#include "user_data/session_token_generator.hpp"

#include <boost/mpl/begin_end.hpp>
#include <spdlog/spdlog.h>

namespace chess::server::authentication {

namespace {

using authentication_messages_t = boost::mpl::vector<msg::hello_server_t, msg::login_t>;

template<typename T>
concept one_of_authentication_msg = msg::mpl_vector_has_type<authentication_messages_t, T>;

} // namespace

struct message_handler_t::impl_t
{
  impl_t(user_data::users_data_manager_t& users_data_manager, const endpoint_t logic_server_endpoint, const client_authenticated_callback_t client_authenticated_callback,
         const send_to_client_callback_t& send_to_client_callback)
    : users_data_manager_{users_data_manager}
    , logic_server_endpoint_{logic_server_endpoint}
    , client_authenticated_callback_{client_authenticated_callback}
    , send_to_client_{send_to_client_callback}
  {}

  template<typename T>
  inline void send_to_client(T&& msg, const endpoint_t& sender, uint64_t send_serial_num) const
  {
    send_to_client_(msg::prepare_for_send(msg::incoming_datagram_t{msg::to_some_datagram(std::forward<T>(msg)), send_serial_num, 0}), sender);
  }

  template<typename T>
  void process(const msg::some_datagram_t& datagram, const endpoint_t& sender, const uint64_t ser_num)
  {
    if constexpr (one_of_authentication_msg<typename T::type>)
    {
      if (datagram.type == msg::id_v<typename T::type>)
      {
        handle(msg::init<typename T::type>(datagram), sender, ser_num);
        return;
      }
    }
    process<typename boost::mpl::next<T>::type>(datagram, sender, ser_num);
  }

  void handle(const msg::hello_server_t& msg, const endpoint_t& sender, const uint64_t ser_num)
  {
    SPDLOG_INFO("New client={} connection request received!", sender);
    send_to_client(msg::get_login_t{}, sender, ser_num);
  }

  void handle(const msg::login_t& msg, const endpoint_t& sender, const uint64_t ser_num)
  {
    const user_data::credentials_t creds = {msg.login, msg.pwd};
    SPDLOG_INFO("Handle login requested from address={}; creds={}", sender, creds);

    switch (users_data_manager_.known(creds))
    {
      case user_data::users_data_manager_t::known_user_res_t::known: client_reconnected(creds, sender, ser_num); break;
      case user_data::users_data_manager_t::known_user_res_t::unknown: new_client(creds, sender, ser_num); break;
      default:
      {
        SPDLOG_INFO("Not valid creds={} from addr={}", creds, sender);
        send_to_client(msg::incorrect_log_t{}, sender, ser_num);
      }
    }
  }

  void send_login_reponse(const client_uuid_t& uuid, const endpoint_t& endpoint, const uint64_t ser_num)
  {
    send_to_client(msg::login_response_t{session_token_generator_.gen(uuid), logic_server_endpoint_.address().to_string(), logic_server_endpoint_.port()}, endpoint, ser_num);
  }

  void client_reconnected(const user_data::credentials_t& creds, const endpoint_t& endpoint, const uint64_t ser_num)
  {
    SPDLOG_INFO("Recconect client={} on addr={}", creds, endpoint);
    const auto uuid = users_data_manager_.user(creds.login).uuid();
    send_login_reponse(uuid, endpoint, ser_num);
    client_authenticated_callback_(endpoint, uuid);
  }

  void new_client(const user_data::credentials_t& creds, const endpoint_t& endpoint, const uint64_t ser_num)
  {
    const auto client_uuid = users_data_manager_.add_user(creds).uuid();
    SPDLOG_INFO("Add new client! client_uuid={}; endpoint={}; creds={}", client_uuid, endpoint, creds);

    send_login_reponse(client_uuid, endpoint, ser_num);
    client_authenticated_callback_(endpoint, client_uuid);
  }

  chess::server::user_data::session_token_generator_t session_token_generator_;
  user_data::users_data_manager_t& users_data_manager_;

  const endpoint_t logic_server_endpoint_;
  const client_authenticated_callback_t client_authenticated_callback_;
  const send_to_client_callback_t send_to_client_;
};

template<>
void message_handler_t::impl_t::process<boost::mpl::end<authentication_messages_t>::type>(const msg::some_datagram_t& datagram, const endpoint_t& sender, const uint64_t ser_num)
{
  SPDLOG_ERROR("No type found for datagram type={}; sender={};", datagram.type, sender);
}

message_handler_t::message_handler_t(user_data::users_data_manager_t& users_data_manager, const endpoint_t& logic_server_endpoint,
                                     const client_authenticated_callback_t& client_authenticated_callback, const send_to_client_callback_t& send_to_client)
  : impl_(std::make_unique<impl_t>(users_data_manager, logic_server_endpoint, client_authenticated_callback, send_to_client))
{}

message_handler_t::~message_handler_t() = default;

void message_handler_t::handle(const endpoint_t& addr, const std::string& message)
try
{
  SPDLOG_TRACE("Received data={} from {}", message, addr);
  const msg::incoming_datagram_t incoming_datagram = msg::init<msg::incoming_datagram_t>(message);
  if (incoming_datagram.data.type == msg::id_v<msg::message_received_t>) return;

  impl_->send_to_client(msg::message_received_t{}, addr, incoming_datagram.response_ser_num);

  SPDLOG_TRACE("Begin processing of the datagram.type={} from addr={}", incoming_datagram.data.type, addr);
  impl_->process<boost::mpl::begin<authentication_messages_t>::type>(incoming_datagram.data, addr, incoming_datagram.response_ser_num + 1);
}
catch (const std::exception& ex)
{
  SPDLOG_CRITICAL("Failed to handle msg={} from {}", message, addr);
  throw;
}

} // namespace chess::server::authentication
