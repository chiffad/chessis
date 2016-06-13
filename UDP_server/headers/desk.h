#ifndef DESK
#define DESK

#include "headers/chess.h"

class Desk : public Board
{
public:
  Desk(const unsigned first_p, const unsigned second_p)
                        : Board(), _first_player_ind(first_p), _second_player_ind(second_p)
  {}
  ~Desk(){}

  int _first_player_ind;
  int _second_player_ind;
};
#endif // DESK

