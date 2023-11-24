#include "logic/players_holder.hpp"

#include <spdlog/spdlog.h>

namespace chess::logic {

players_holder_t::players_holder_t() = default;

std::pair<player_arr_t::iterator, bool> players_holder_t::add(const client_uuid_t& uuid)
{
  if (count(uuid)) SPDLOG_INFO("Player with uuid={} recconected!", uuid);
  else SPDLOG_INFO("Add new player uuid={}", uuid);

  return emplace(std::piecewise_construct, std::forward_as_tuple(uuid), std::forward_as_tuple(uuid));
}

} // namespace chess::logic
