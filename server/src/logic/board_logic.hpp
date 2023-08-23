#pragma once

#include "common/unique_id_generator.hpp"
#include "logic/coord.hpp"

#include <memory>
#include <string>

namespace logic {

class board_logic_t
{
public:
  using uuid_t = common::uuid_t;

  explicit board_logic_t(const uuid_t& uuid);
  virtual ~board_logic_t();
  bool move(const coord_t& from, const coord_t& to);
  bool back_move();
  void start_new_game();
  void go_to_history(size_t i);

  const uuid_t& uuid() const;
  bool mate() const;
  std::string get_moves_history() const;
  unsigned get_move_num() const;
  std::string get_board_mask() const;

public:
  board_logic_t(const board_logic_t&) = delete;
  board_logic_t& operator=(const board_logic_t&) = delete;
  board_logic_t(board_logic_t&&) = default;
  board_logic_t& operator=(board_logic_t&&) = default;

private:
  struct impl_t;
  std::unique_ptr<impl_t> impl_;
};

void make_moves_from_str(const std::string& str, board_logic_t& desk);
void load_moves_from_file(const std::string& path, board_logic_t& desk);
void write_moves_to_file(const std::string& path, board_logic_t& desk);

} // namespace logic
