#include "server/logic/clients_holder.hpp"
#include <spdlog/spdlog.h>

namespace chess::server::logic {

clients_holder_t::clients_holder_t(io_service_t& io_service, const connection_status_signal_t::slot_type& subscriber)
  : io_service_(io_service)
  , subscriber_{subscriber}
{}

client_t& clients_holder_t::add(const client_uuid_t& uuid, const endpoint_t& addr)
{
  // TODO: do we need address_to_cl_?
  if (address_to_cl_.count(addr))
  {
    SPDLOG_INFO("Client on address={} already known!", addr);
    return *address_to_cl_.at(addr);
  }
  if (uuid_to_cl_.count(uuid))
  {
    SPDLOG_INFO("Client uuid={} already known!", uuid);
    auto& cl = uuid_to_cl_.at(uuid);
    if (!address_to_cl_.count(addr))
    {
      SPDLOG_INFO("New address={} for already known client uuid={}!", addr, uuid);
      address_to_cl_.erase(cl->address());
      address_to_cl_[addr] = cl;
    }
    return *cl;
  }

  SPDLOG_INFO("Add new client on address={}; uuid={}!", addr, uuid);
  auto cl = std::make_shared<client_t>(io_service_, uuid, addr);
  cl->connect_connection_status_changed([cl, this](const bool online) { subscriber_(*cl, online); });
  address_to_cl_[addr] = cl;
  uuid_to_cl_[uuid] = cl;
  return *cl;
}

client_t& clients_holder_t::at(const client_uuid_t& uuid)
{
  return *uuid_to_cl_.at(uuid);
}

const client_t& clients_holder_t::at(const client_uuid_t& uuid) const
{
  return *uuid_to_cl_.at(uuid);
}

const client_t& clients_holder_t::at(const endpoint_t& addr) const
{
  return *address_to_cl_.at(addr);
}

client_t& clients_holder_t::at(const endpoint_t& addr)
{
  return *address_to_cl_.at(addr);
}

std::vector<datagram_t<std::string>> clients_holder_t::datagrams_to_send()
{
  std::vector<datagram_t<std::string>> res;
  for (auto& [addr, client] : address_to_cl_)
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