#ifndef _VILVIVYVKCKYVLVUYFDKYUFYTF
#define _VILVIVYVKCKYVLVUYFDKYUFYTF
#include <iostream>
#include <vector>

class Board
{
public:
   enum COLOR {NONE = 0, W_FIG = int('a'), B_FIG = int('Z')};
   enum FIGURES {B_QUEEN = 'Q', B_KING = 'K', B_ROOK = 'R', B_ELEPHANT = 'E', B_HORSE = 'H', B_PAWN = 'P', W_QUEEN = 'q',
                W_KING = 'k', W_ROOK = 'r', W_ELEPHANT = 'e', W_HORSE = 'h', W_PAWN = 'p', FREE_FIELD = '.'};
   enum COLORLESS_FIG {QUEEN = B_QUEEN + W_QUEEN, KING = W_KING + B_KING, ROOK = W_ROOK + B_ROOK,
         ELEPHANT = W_ELEPHANT + B_ELEPHANT, HORSE = W_HORSE + B_HORSE, PAWN = W_PAWN + B_PAWN, NOT_FIGURE = 92};
   enum {X_SIZE = 8, Y_SIZE = 8, FIGURES_NUMBER = 32};

public:
   struct Coord
   {
     int x;
     int y;
     bool operator ==(const Coord& rhs);
   };

   Board();
   bool move(const Coord& fr, const Coord& t);
   bool back_move();
   bool is_mate(COLOR color);
   bool go_to_history_index(const unsigned index);
   unsigned get_current_move() const;
   FIGURES get_figure(const Coord& c) const;
   FIGURES get_figure(const int x, const int y) const;
   COLORLESS_FIG get_colorless_figure(const Coord& c) const;
   COLOR get_color(const Coord& c) const;
   COLOR get_move_color_i_from_end(const unsigned i) const;

   const Coord& get_prev_from_coord() const;
   const Coord& get_prev_to_coord() const;

private:
   void next_move(const Coord& fr = Coord(), const Coord& t = Coord());
   void add_move_to_history_copy(const Coord& coord_from, const Coord& coord_to);
   void field_change(const Coord& fr, const Coord& t);
   void if_castling(const Coord& fr, const Coord& t);
   bool right_move_turn(const Coord& coord) const;
   bool is_check(COLOR color) const;
   bool step_ver(const Coord& f, const Coord& t) const;
   bool is_castling(const Coord& from, const Coord& to) const;
   void set_field(const Coord& lhs, const Coord& rhs);
   void set_field(const Coord& coord, FIGURES fig);
   void make_moves_from_str(const std::string& str);

private:
   struct Moves
   {
     Coord _from;
     Coord _to;
     COLOR _color;
     FIGURES _fig_on_captured_field;
   }m;
   std::vector<Moves> moves;
   int  _move_num;
   FIGURES _field[X_SIZE][Y_SIZE];
   bool _is_go_to_history_in_progress;

   struct Fool_move_coord
   {
     Coord _from;
     Coord _to;
   };
   std::vector<Fool_move_coord> history_copy;
};
#endif //_VILVIVYVKCKYVLVUYFDKYUFYTF
