#ifndef __MY_BOARD_H__VILVIVYVKCKYVLVUYFDKYUFYTF
#define __MY_BOARD_H__VILVIVYVKCKYVLVUYFDKYUFYTF

#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include "coord.h"


namespace logic
{

class Board
{
public:
  Board();
  ~Board();
  bool move(const Coord &from, const Coord &to);
  bool back_move();
  void start_new_game();
  void make_moves_from_str(const std::string &str);
  void go_to_history_index(const unsigned index);
  bool is_mate();

  std::string get_moves_history() const;

  unsigned get_move_num() const;
  std::string get_board_mask() const;

public:
  Board(const Board&) = delete;
  Board& operator=(const Board&) = delete;


private:
  struct impl_t;
  std::unique_ptr<impl_t> impl;
};

} //namespace logic

#endif // __MY_BOARD_H__VILVIVYVKCKYVLVUYFDKYUFYTF
