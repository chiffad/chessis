#pragma once

#include "logic/player.hpp"

#include <map>
#include <memory>

namespace chess::logic {

using player_arr_t = std::map<client_uuid_t, player_t>;

class players_holder_t : private player_arr_t
{
public:
  explicit players_holder_t();
  std::pair<player_arr_t::iterator, bool> add(const client_uuid_t& addr);

  using player_arr_t::at;
  using player_arr_t::begin;
  using player_arr_t::count;
  using player_arr_t::end;
};

} // namespace chess::logic