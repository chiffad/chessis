#include "logic/boards_holder.hpp"

namespace chess::logic {

boards_holder_t::boards_holder_t() = default;

board_logic_t::uuid_t boards_holder_t::add()
{
  const auto uuid = uuid_generator_.new_uuid();
  emplace(std::piecewise_construct, std::forward_as_tuple(uuid), std::forward_as_tuple(uuid));
  return uuid;
}

} // namespace chess::logic