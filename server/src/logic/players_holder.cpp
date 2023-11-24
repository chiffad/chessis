#include "logic/players_holder.hpp"

#include <spdlog/spdlog.h>

namespace chess::logic {

players_holder_t::players_holder_t() = default;

player_t& players_holder_t::add(const client_uuid_t& uuid)
{
  if (count(uuid)) SPDLOG_WARN("Player with uuid={} recconected!", uuid);
  else SPDLOG_INFO("Add new player uuid={}", uuid);

  return emplace(std::piecewise_construct, std::forward_as_tuple(uuid), std::forward_as_tuple(uuid)).first->second;
}

} // namespace chess::logic
