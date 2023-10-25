#pragma once

#include "common/helper.hpp"
#include "logic/player.hpp"

#include <map>
#include <memory>

namespace chess::logic {

using player_arr_t = std::map<player_t::uuid_t, player_t>;

class players_holder_t : private player_arr_t
{
public:
  explicit players_holder_t();
  player_t::uuid_t add(const endpoint_t& addr);
  player_arr_t::iterator find(const endpoint_t& addr);
  player_arr_t::iterator find(const std::string& login);

  using player_arr_t::at;
  using player_arr_t::begin;
  using player_arr_t::end;

private:
  common::uuid_generator_t uuid_generator_;
};

} // namespace chess::logic