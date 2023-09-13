#include "server/authentication/message_handler.hpp"
#include "common/unique_id_generator.hpp"
#include "logic/credentials.hpp"
#include "messages/messages.hpp"
#include "server/authentication/client.hpp"

#include <boost/mpl/begin_end.hpp>
#include <map>
#include <spdlog/spdlog.h>

namespace server::authentication {

namespace {
inline bool valid_credentials(const logic::credentials_t& cred)
{
  return !cred.login.empty() && !cred.pwd.empty();
}

using authentication_messages_t = boost::mpl::vector<msg::hello_server_t, msg::login_t>;

template<typename T>
concept one_of_authentication_msg = msg::mpl_vector_has_type<authentication_messages_t, T>;

} // namespace

struct message_handler_t::impl_t
{
  impl_t(const endpoint_t logic_server_endpoint, const client_authenticated_callback_t client_authenticated_callback,
         const send_to_client_callback_t& send_to_client_callback)
    : logic_server_endpoint_{logic_server_endpoint}
    , client_authenticated_callback_{client_authenticated_callback}
    , send_to_client_{send_to_client_callback}
    , send_serial_num_{0}
  {}

  template<typename T>
  inline void send_to_client(T&& msg, const endpoint_t& sender) const
  {
    send_to_client_(msg::prepare_for_send(msg::incoming_datagramm_t{msg::prepare_for_send(std::forward<T>(msg)), ++send_serial_num_}), sender);
  }

  // TODO: rename some_datagramm_t -> some_datagram_t
  template<typename T>
  void process(const msg::some_datagramm_t& datagram, const endpoint_t& sender)
  {
    if constexpr (one_of_authentication_msg<typename T::type>)
    {
      if (datagram.type == msg::id_v<typename T::type>)
      {
        handle(msg::init<typename T::type>(datagram), sender);
        return;
      }
    }
    process<typename boost::mpl::next<T>::type>(datagram, sender);
  }

  // TODO: define, what to do with send_serial_num_ in case of receiving
  //  and do we need send_serial_num_ at all in authentication server
  void handle(const msg::hello_server_t& /*msg*/, const endpoint_t& sender)
  {
    SPDLOG_INFO("New client={} connection request received!", sender);
    send_to_client(msg::get_login_t{}, sender);
  }

  void handle(const msg::login_t& msg, const endpoint_t& sender)
  {
    const logic::credentials_t creds = {msg.login, msg.pwd};
    SPDLOG_INFO("Handle login requested from address={}; creds={}", sender, creds);

    if (!valid_credentials(creds))
    {
      SPDLOG_INFO("Not valid creds={} from addr={}", creds, sender);
      send_to_client(msg::incorrect_log_t{}, sender);
      return;
    }

    if (clients_.count(creds.login))
    {
      auto& cl = clients_.at(creds.login);
      if (cl.credentials == creds)
      {
        SPDLOG_DEBUG("Recconect client={} on addr={}", msg.login, sender);
        cl.address = sender;
        send_to_client(msg::login_response_t{boost::uuids::to_string(cl.uuid), logic_server_endpoint_.address().to_string(), logic_server_endpoint_.port()},
                       sender);
      }
      return;
    }

    add_client(creds, sender);
  }

  void add_client(const logic::credentials_t& creds, const endpoint_t& endpoint)
  {
    const auto client_uuid = clients_uuid_generator_.new_uuid();
    const client_t cl = {client_uuid, endpoint, creds};
    SPDLOG_INFO("Add new client={}!", cl);

    send_to_client(msg::login_response_t{boost::uuids::to_string(cl.uuid), logic_server_endpoint_.address().to_string(), logic_server_endpoint_.port()},
                   endpoint);

    clients_[creds.login] = std::move(cl);
    client_authenticated_callback_(client_uuid);
  }

  using login_t = std::string;
  std::map<login_t, client_t> clients_;
  common::uuid_generator_t clients_uuid_generator_;
  const endpoint_t logic_server_endpoint_;
  const client_authenticated_callback_t client_authenticated_callback_;
  const send_to_client_callback_t send_to_client_;
  int send_serial_num_;
};

template<>
void message_handler_t::impl_t::process<boost::mpl::end<authentication_messages_t>::type>(const msg::some_datagramm_t& datagram, const endpoint_t& sender)
{
  SPDLOG_ERROR("No type found for datagram type={}; sender={};", datagram.type, sender);
}

message_handler_t::message_handler_t(const endpoint_t& logic_server_endpoint, const client_authenticated_callback_t& client_authenticated_callback,
                                     const send_to_client_callback_t& send_to_client)
  : impl_(std::make_unique<impl_t>(logic_server_endpoint, client_authenticated_callback, send_to_client))
{}

message_handler_t::~message_handler_t() = default;

void message_handler_t::handle(const endpoint_t& addr, const std::string& message)
{
  SPDLOG_INFO("Received data={}", message);
  const auto incoming_datagramm = msg::init<msg::incoming_datagramm_t>(message);
  // authentification server do not care about incoming serial number

  const auto datagram = msg::init<msg::some_datagramm_t>(incoming_datagramm.data);
  if (datagram.type == msg::id_v<msg::message_received_t>) return;

  impl_->send_to_client(msg::message_received_t{}, addr);

  SPDLOG_TRACE("Begin processing of the datagram.type={} from addr={}", datagram.type, addr);
  impl_->process<boost::mpl::begin<authentication_messages_t>::type>(datagram, addr);
}

} // namespace server::authentication