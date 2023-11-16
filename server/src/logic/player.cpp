#include "logic/player.hpp"

namespace chess::logic {

player_t::player_t(const client_uuid_t& client_uuid)
  : playing_white{true}
  , uuid{client_uuid}
{}

std::ostream& operator<<(std::ostream& os, const player_t& c)
{
  return os << "Player{ uuid=" << c.uuid << " }";
}

bool operator==(const player_t& lhs, const player_t& rhs)
{
  return lhs.uuid == rhs.uuid;
}

} // namespace chess::logic