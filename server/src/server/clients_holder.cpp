#include "server/clients_holder.hpp"
#include <spdlog/spdlog.h>

namespace server {

clients_holder_t::clients_holder_t(io_service_t& io_service)
  : io_service_(io_service)
{}

void clients_holder_t::add(const endpoint_t& addr)
{
  emplace(std::piecewise_construct, std::forward_as_tuple(addr), std::forward_as_tuple(io_service_, addr));
}

void clients_holder_t::process(const datagram_t& datagram)
{
  const auto it = find(datagram.address);
  if (it == end())
  {
    SPDLOG_INFO("datagram={} from unknown address received!", datagram);

    // TODO:
    //  * send login or uuid in each message!
    //  * check is this datagram contains known uuid or login and if exists -> update client address and process as usual i.e. do not add new client
    add(datagram.address);
  }

  auto& cl = at(datagram.address);
  cl.message_received(datagram.message);
}

std::vector<datagram_t> clients_holder_t::datagrams_to_send()
{
  std::vector<datagram_t> res;
  for (auto& client : *this)
  {
    while (client.second.message_for_server_append())
    {
      res.push_back({client.second.address(), client.second.pull_for_server()});
    }
  }

  return res;
}

std::vector<datagram_t> clients_holder_t::datagrams_to_process()
{
  std::vector<datagram_t> res;
  for (auto& client : *this)
  {
    while (client.second.message_for_logic_append())
    {
      res.push_back({client.second.address(), client.second.pull_for_logic()});
    }
  }

  return res;
}

/*clients_arr_t::iterator clients_holder_t::find(const endpoint_t& addr)
{
  return find(addr);
}*/

/*clients_arr_t::iterator clients_holder_t::find(const std::string& login)
{
  return std::find_if(begin(), end(), [&login](const auto& cl) { return cl.second.credentials().login == login; });
}*/

} // namespace server