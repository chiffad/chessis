#include "server/logic/clients_holder.hpp"
#include <spdlog/spdlog.h>

namespace chess::server::logic {

clients_holder_t::clients_holder_t(io_service_t& io_service, const connection_status_signal_t::slot_type& subscriber)
  : io_service_(io_service)
  , subscriber_{subscriber}
{}

client_t& clients_holder_t::add(const client_uuid_t& uuid, const endpoint_t& addr)
{
  if (uuid_to_cl_.count(uuid))
  {
    SPDLOG_INFO("Client uuid={} already known!", uuid);
    auto& cl = uuid_to_cl_.at(uuid);
    return *cl;
  }

  SPDLOG_INFO("Add new client on address={}; uuid={}!", addr, uuid);

  auto cl = add_client(uuid, addr);
  cl->connect_connection_status_changed([uuid, this](const bool online) { client_connection_changed(uuid, online); });
  return *cl;
}

std::shared_ptr<client_t> clients_holder_t::add_client(const client_uuid_t& uuid, const endpoint_t& e)
{
  auto cl = std::make_shared<client_t>(io_service_, uuid, e);
  uuid_to_cl_[uuid] = cl;
  return cl;
}

void clients_holder_t::client_connection_changed(const client_uuid_t& uuid, const bool online)
{
  subscriber_(uuid, online);

  if (online) return;

  SPDLOG_INFO("Remove client {} as it is disconnected", uuid);
  uuid_to_cl_.erase(uuid);
}

void clients_holder_t::message_received(const endpoint_t& e, const client_uuid_t& uuid, msg::incoming_datagram_t message)
{
  auto cl_it = uuid_to_cl_.find(uuid);
  if (cl_it != uuid_to_cl_.end())
  {
    cl_it->second->message_received(e, std::move(message));
    return;
  }

  add_client(uuid, e)->message_received(e, std::move(message));
}

client_t& clients_holder_t::at(const client_uuid_t& uuid)
{
  return *uuid_to_cl_.at(uuid);
}

const client_t& clients_holder_t::at(const client_uuid_t& uuid) const
{
  return *uuid_to_cl_.at(uuid);
}

std::vector<datagram_t<std::string>> clients_holder_t::datagrams_to_send()
{
  std::vector<datagram_t<std::string>> res;
  for (auto& [uuid, client] : uuid_to_cl_)
  {
    while (client->message_for_send_append())
    {
      res.push_back({client->address(), client->pull_for_send()});
    }
  }

  return res;
}

std::map<client_uuid_t, std::vector<msg::some_datagram_t>> clients_holder_t::datagrams_to_process()
{
  std::map<client_uuid_t, std::vector<msg::some_datagram_t>> res;
  for (auto& [uuid, client] : uuid_to_cl_)
  {
    while (client->message_for_logic_append())
    {
      res[uuid].push_back(client->pull_for_logic());
    }
  }

  return res;
}

} // namespace chess::server::logic