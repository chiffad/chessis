#include "server/clients_holder.hpp"
#include <spdlog/spdlog.h>

namespace server {

clients_holder_t::clients_holder_t(io_service_t& io_service, const connection_status_signal_t::slot_type& subscriber)
  : io_service_(io_service)
  , subscriber_{subscriber}
{}

void clients_holder_t::add(const endpoint_t& addr)
{
  emplace(std::piecewise_construct, std::forward_as_tuple(addr), std::forward_as_tuple(io_service_, addr));
  auto& cl = at(addr);
  at(addr).connect_connection_status_changed([&](const bool online) { subscriber_(cl, online); });
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

  // TODO now!:
  //  * add client login to each message
  //  * think about 2 types of client: logged in client and not logged in
  //  * when cleint with unkown address is connetted -> create "not logged in client" -> ask login -> check received login/pwd
  //    -> if valid -> create new registered client(or reinit existing one)
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

} // namespace server