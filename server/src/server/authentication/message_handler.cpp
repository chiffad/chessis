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

template<typename T>
concept one_of_authentication_msg = msg::one_of<T, msg::hello_server_t, msg::login_t>;
} // namespace

struct message_handler_t::impl_t
{
  impl_t(const endpoint_t logic_server_endpoint, const client_authenticated_callback_t client_authenticated_callback,
         const send_to_client_callback_t& send_to_client)
    : logic_server_endpoint_{logic_server_endpoint}
    , client_authenticated_callback_{client_authenticated_callback}
    , send_to_client_{send_to_client}
  {}

  template<typename T>
  inline void send_to_client(T&& msg, const endpoint_t& sender) const
  {
    send_to_client_(msg::prepare_for_send(std::forward<T>(msg)), sender);
  }

  // TODO: rename some_datagramm_t -> some_datagram_t
  template<typename T>
  void process(const msg::some_datagramm_t& datagramm, const endpoint_t& sender)
  {
    if constexpr (one_of_authentication_msg<typename T::type>)
    {
      // TODO: use enum values for msg::id_v
      if (datagramm.type == msg::id_v<typename T::type>)
      {
        handle(msg::init<typename T::type>(datagramm), sender);
        return;
      }
    }
    process<typename boost::mpl::next<T>::type>(datagramm, sender);
  }

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
        // TODO
        // send_to_client(msg::login_responce_t{cl.uuid, logic_server_endpoint_});
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

    // TODO
    //  send_to_client(msg::login_responce_t{cl.uuid, logic_server_endpoint_}, endpoint);
    clients_[creds.login] = std::move(cl);
    client_authenticated_callback_(client_uuid);
  }

  using login_t = std::string;
  std::map<login_t, client_t> clients_;
  common::uuid_generator_t clients_uuid_generator_;
  const endpoint_t logic_server_endpoint_;
  const client_authenticated_callback_t client_authenticated_callback_;
  const send_to_client_callback_t send_to_client_;
};

template<>
void message_handler_t::impl_t::process<boost::mpl::end<msg::messages_t>::type>(const msg::some_datagramm_t& datagramm, const endpoint_t& sender)
{
  SPDLOG_ERROR("No type found for datagramm type={}; sender={};", datagramm.type, sender);
}

message_handler_t::message_handler_t(const endpoint_t& logic_server_endpoint, const client_authenticated_callback_t& client_authenticated_callback,
                                     const send_to_client_callback_t& send_to_client)
  : impl_(std::make_unique<impl_t>(logic_server_endpoint, client_authenticated_callback, send_to_client))
{}

message_handler_t::~message_handler_t() = default;

void message_handler_t::handle(const endpoint_t& addr, const std::string& message)
{
  const msg::some_datagramm_t datagramm = msg::init<msg::some_datagramm_t>(message);
  SPDLOG_TRACE("Begin processing of the datagramm.type={} from addr={}", datagramm.type, addr);
  impl_->process<boost::mpl::begin<msg::messages_t>::type>(datagramm, addr);
}

} // namespace server::authentication