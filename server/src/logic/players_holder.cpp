#include "logic/players_holder.hpp"

#include <spdlog/spdlog.h>

namespace chess::logic {

players_holder_t::players_holder_t() = default;

player_t& players_holder_t::add(const player_t::uuid_t& uuid)
{
  if (count(uuid))
  {
    SPDLOG_WARN("Cannot add new player with uuid={}; Already exist!", uuid);
  }
  SPDLOG_INFO("Add new player uuid={}", uuid);
  return emplace(std::piecewise_construct, std::forward_as_tuple(uuid), std::forward_as_tuple(uuid)).first->second;
}

} // namespace chess::logic
