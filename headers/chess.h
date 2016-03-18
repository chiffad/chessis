#ifndef _VILVIVYVKCKYVLVUYFDKYUFYTF
#define _VILVIVYVKCKYVLVUYFDKYUFYTF
#include<iostream>
#include<vector>

enum COLOR {W_FIG = int('a'), B_FIG = int('Z'), NONE = 0};
enum FIGURES {B_QUEEN = 'Q', B_KING = 'K', B_ROOK = 'R', B_ELEPHANT = 'E', B_HORSE = 'H', B_PAWN = 'P', W_QUEEN = 'q', 
             W_KING = 'k', W_ROOK = 'r', W_ELEPHANT = 'e', W_HORSE = 'h', W_PAWN = 'p', FREE_FIELD = '.'}; 
enum {X_SIZE = 8, Y_SIZE = 8, QUEEN = B_QUEEN + W_QUEEN, KING = W_KING + B_KING, ROOK = W_ROOK + B_ROOK, 
     ELEPHANT = W_ELEPHANT + B_ELEPHANT, HORSE = W_HORSE + B_HORSE, PAWN = W_PAWN + B_PAWN};

class Board
{
public:
   struct Coord
   {
     int x;
     int y;
   }from, to;
   Board();
   bool move(Coord const& fr, Coord const& t);
   void back_move();
   bool is_mate(COLOR color);
   int  get_figure(Coord const& c) const;
   COLOR get_color(Coord const& c) const;
   unsigned int get_current_move() const {return _move_num;}
   FIGURES get_field(Coord const& c) const {return FIGURES(_field[c.x][c.y]);}
   COLOR get_prev_color() const {return moves[_move_num - 1]._color;}
   const char* get_board() const {return _field};

private:
   struct Moves
   {
     Coord _prev_from;
     Coord _prev_to;
     COLOR _color;
     char _fig_on_field;
     bool _w_king_m;
     bool _b_king_m;
     bool _w_l_rook_m;
     bool _w_r_rook_m;
     bool _b_l_rook_m;
     bool _b_r_rook_m;
   }m;
   std::vector<Moves>moves;
   Coord _f;
   Coord _t;
   int  _move_num;
   char _field[X_SIZE][Y_SIZE];

   void next_move();
   void field_change();
   bool right_move_turn() const;
   bool is_check(COLOR color) const;
   bool step_ver(Coord const& f, Coord const& t) const;
   bool step_ver_2(Coord const& f, Coord const& t) const;
};
#endif //_VILVIVYVKCKYVLVUYFDKYUFYTF
