#include "logic/boards_holder.hpp"

namespace logic {

boards_holder_t::boards_holder_t() = default;

board_logic_t& boards_holder_t::add()
{
  push_back(board_logic_t{});
}

} // namespace logic