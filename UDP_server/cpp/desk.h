#ifndef DESK
#define DESK

#include "chess.h"

class Desk : public Board
{
public:
  Desk(const unsigned first_p, const unsigned second_p)
                        : Board(), _first_player_ind(first_p), _second_player_ind(second_p)
  {}

  bool contain_player(const int ind) const
  { return (ind == _first_player_ind || ind == _second_player_ind); }

public:
   Desk(const Desk&) = delete;
   Desk& operator=(const Desk&) = delete;

private:
   const int _first_player_ind;
   const int _second_player_ind;
};
#endif // DESK

