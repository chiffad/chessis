#pragma once

#include "common/helper.hpp"

#include "spdlog/fmt/ostr.h"

namespace chess::logic {

struct player_t
{
  player_t(const client_uuid_t& uuid);

  bool playing_white;
  const client_uuid_t uuid;
};

std::ostream& operator<<(std::ostream& os, const player_t& c);
bool operator==(const player_t& lhs, const player_t& rhs);

} // namespace chess::logic
