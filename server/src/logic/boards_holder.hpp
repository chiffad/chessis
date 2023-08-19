#pragma once

#include "logic/board_logic.hpp"

#include <list>

namespace logic {

using desks_arr_t = std::list<board_logic_t>;

class boards_holder_t : private desks_arr_t
{
public:
  explicit boards_holder_t();
  board_logic_t& add();
};

} // namespace logic