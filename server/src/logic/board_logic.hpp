#pragma once

#include "logic/coord.hpp"

#include <memory>
#include <string>

namespace logic {

class board_logic_t
{
public:
  explicit board_logic_t();
  virtual ~board_logic_t();
  bool move(const coord_t& from, const coord_t& to);
  bool back_move();
  void start_new_game();
  void go_to_history(size_t i);
  bool mate() const;

  std::string get_moves_history() const;
  unsigned get_move_num() const;
  std::string get_board_mask() const;

public:
  board_logic_t(const board_logic_t&) = delete;
  board_logic_t& operator=(const board_logic_t&) = delete;

private:
  struct impl_t;
  std::unique_ptr<impl_t> impl_;
};

} // namespace logic
