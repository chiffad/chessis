#ifndef _VILVIVYVKCKYVLVUYFDKYUFYTF
#define _VILVIVYVKCKYVLVUYFDKYUFYTF
#include<iostream>
#include<vector>

enum COLOR {W_FIG = int('a'), B_FIG = int('Z'), NONE = 0};
enum FIGURES {B_QUEEN = 'Q', B_KING = 'K', B_ROOK = 'R', B_ELEPHANT = 'E', B_HORSE = 'H', B_PAWN = 'P', W_QUEEN = 'q', 
             W_KING = 'k', W_ROOK = 'r', W_ELEPHANT = 'e', W_HORSE = 'h', W_PAWN = 'p', FREE_FIELD = '.'}; 
enum {X_SIZE = 8, Y_SIZE = 8, FIGURES_NUMBER = 32, QUEEN = B_QUEEN + W_QUEEN, KING = W_KING + B_KING, ROOK = W_ROOK + B_ROOK,
     ELEPHANT = W_ELEPHANT + B_ELEPHANT, HORSE = W_HORSE + B_HORSE, PAWN = W_PAWN + B_PAWN};

class Board
{
public:
   struct Coord
   {
     int x;
     int y;
     bool operator ==(Coord const& rhv);
   };

   Board();
   bool move(Coord const& fr, Coord const& t);
   bool back_move();
   bool is_mate(COLOR color);
   unsigned get_current_move() const;
   FIGURES get_figure(Coord const& c) const;
   FIGURES get_figure(const int x, const int y) const;
   int get_colorless_figure(Coord const& c) const;
   COLOR get_color(Coord const& c) const;
   COLOR get_move_color_i_from_end(const unsigned i) const;

   Coord const& get_i_from_coord_from_end(const unsigned i = 0 ) const;
   Coord const& get_i_to_coord_from_end(const unsigned i = 0) const;

private:
   struct Moves
   {
     Coord _from;
     Coord _to;
     COLOR _color;
     FIGURES _fig_on_captured_field;
   }m;
   std::vector<Moves>moves;
   Coord _f;
   Coord _t;
   int  _move_num;
   FIGURES _field[X_SIZE][Y_SIZE];

   void next_move();
   void field_change();
   bool right_move_turn() const;
   bool is_check(COLOR color) const;
   bool step_ver(Coord const& f, Coord const& t) const;
   bool is_can_castling(COLOR color, const int x) const;
   void set_field(Coord const& rhv, Coord const& lhv);
   void set_field(Coord const& rhv, FIGURES fig);
};
#endif //_VILVIVYVKCKYVLVUYFDKYUFYTF
