#pragma once

#include "logic/board_logic.hpp"

#include <map>

namespace logic {

using desks_arr_t = std::map<board_logic_t::uuid_t, board_logic_t>;

class boards_holder_t : private desks_arr_t
{
public:
  explicit boards_holder_t();
  board_logic_t::uuid_t add();
  using desks_arr_t::at;
  using desks_arr_t::count;
  using desks_arr_t::erase;

private:
  common::uuid_generator_t uuid_generator_;
};

} // namespace logic