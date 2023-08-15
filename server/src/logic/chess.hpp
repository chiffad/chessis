#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "logic/coord.hpp"

namespace logic {

class board_t
{
public:
  explicit board_t();
  virtual ~board_t();
  bool move(const coord_t& from, const coord_t& to);
  bool back_move();
  void start_new_game();
  void go_to_history(size_t i);
  bool mate() const;

  std::string get_moves_history() const;
  unsigned get_move_num() const;
  std::string get_board_mask() const;

public:
  board_t(const board_t&) = delete;
  board_t& operator=(const board_t&) = delete;

private:
  struct impl_t;
  std::unique_ptr<impl_t> impl_;
};

} // namespace logic
