#ifndef __MY_BOARD_H__VILVIVYVKCKYVLVUYFDKYUFYTF
#define __MY_BOARD_H__VILVIVYVKCKYVLVUYFDKYUFYTF

#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include "coord.h"


namespace logic
{

class board_t
{
public:
  board_t();
  ~board_t();
  bool move(const coord_t &from, const coord_t &to);
  bool back_move();
  void start_new_game();
  void go_to_history_index(const unsigned index);
  bool is_mate();

  std::string get_moves_history() const;
  unsigned get_move_num() const;
  std::string get_board_mask() const;

public:
  board_t(const board_t&) = delete;
  board_t& operator=(const board_t&) = delete;


private:
  struct impl_t;
  std::unique_ptr<impl_t> impl;
};

} //namespace logic

#endif // __MY_BOARD_H__VILVIVYVKCKYVLVUYFDKYUFYTF
