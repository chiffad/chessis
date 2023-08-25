#include "logic/players_holder.hpp"

namespace logic {

players_holder_t::players_holder_t() = default;

player_t::uuid_t players_holder_t::add(const endpoint_t& addr)
{
  const auto uuid = uuid_generator_.new_uuid();
  emplace(std::piecewise_construct, std::forward_as_tuple(uuid), std::forward_as_tuple(addr, uuid));
  return uuid;
}

player_arr_t::iterator players_holder_t::find(const endpoint_t& addr)
{
  return std::find_if(begin(), end(), [&addr](const auto& cl) { return cl.second.address() == addr; });
}

player_arr_t::iterator players_holder_t::find(const std::string& login)
{
  return std::find_if(begin(), end(), [&login](const auto& cl) { return cl.second.credentials().login == login; });
}

} // namespace logic