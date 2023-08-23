#include "server/clients_holder.hpp"

namespace server {

clients_holder_t::clients_holder_t(io_service_t& io_service)
  : io_service_(io_service)
{}

client_t::uuid_t clients_holder_t::add(const endpoint_t& addr)
{
  const auto uuid = uuid_generator_.new_uuid();

  emplace(std::piecewise_construct, std::forward_as_tuple(uuid), std::forward_as_tuple(io_service_, addr, uuid));

  return uuid;
}

clients_arr_t::iterator clients_holder_t::find(const endpoint_t& addr)
{
  return std::find_if(begin(), end(), [&addr](const auto& cl) { return cl.second.address() == addr; });
}

clients_arr_t::iterator clients_holder_t::find(const std::string& login)
{
  return std::find_if(begin(), end(), [&login](const auto& cl) { return cl.second.credentials().login == login; });
}

} // namespace server