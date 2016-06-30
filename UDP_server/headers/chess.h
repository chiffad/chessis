#ifndef _VILVIVYVKCKYVLVUYFDKYUFYTF
#define _VILVIVYVKCKYVLVUYFDKYUFYTF
#include <iostream>
#include <vector>

class Board
{
public:
   enum COLOR {NONE, W_FIG, B_FIG};
   enum FIGURES {B_QUEEN = 'Q', B_KING = 'K', B_ROOK = 'R', B_ELEPHANT = 'E', B_HORSE = 'H', B_PAWN = 'P', W_QUEEN = 'q',
                 W_KING = 'k', W_ROOK = 'r', W_ELEPHANT = 'e', W_HORSE = 'h', W_PAWN = 'p', FREE_FIELD = '.'};
   enum COLORLESS_FIG {NOT_FIGURE = FREE_FIELD, QUEEN = B_QUEEN + W_QUEEN, KING = W_KING + B_KING, ROOK = W_ROOK + B_ROOK,
                       PAWN = W_PAWN + B_PAWN, ELEPHANT = W_ELEPHANT + B_ELEPHANT, HORSE = W_HORSE + B_HORSE};
   enum {BOARD_SIZE = 8, FIGURES_NUMBER = 32 , a_LETTER = 'a', h_LETTER = 'h', ONE_ch = '1', EIGHT_ch = '8', FREE_SPACE = ' '};

public:
   struct Coord
   {
     Coord(const unsigned X = 0, const unsigned Y = 0);
     bool operator==(const Coord& rhs);
     unsigned x;
     unsigned y;
   };

public:
   Board();
   bool move(const Coord &from, const Coord &to);
   void back_move();
   void start_new_game();
   void make_moves_from_str(const std::string &str);
   void go_to_history_index(const unsigned index);
   bool is_mate();

   const std::string get_moves_history() const;
   void read_moves_from_file(const std::string &path);
   void write_moves_to_file(const std::string &path) const;

   unsigned get_actual_move() const;
   unsigned get_last_made_move() const;
   const std::string get_board_mask() const;
   COLOR get_move_color() const;
   COLOR get_color(const Coord &c) const;
   FIGURES get_figure(const Coord &c) const;
   FIGURES get_figure(const unsigned x, const unsigned y) const;
   COLORLESS_FIG get_colorless_figure(const Coord &c) const;

private:
   Board(const Board&) = delete;
   void operator=(const Board&) = delete;

   void field_change(const Coord &from, const Coord &to);
   void next_move(const Coord &from = Coord(), const Coord &to = Coord());
   void set_field(const Coord &c, const FIGURES &fig);
   void set_field(const Coord &lhs, const Coord &rhs);
   void if_castling(const Coord &fr, const Coord &to);
   unsigned get_field_index(const Coord &c) const;

   bool is_castling(const Coord &from, const Coord &to) const;
   bool is_can_move(const Coord &from, const Coord &to) const;
   bool is_right_move_turn(const Coord &c) const;
   bool is_check(const COLOR color) const;

private:
   struct Moves
   {
     Coord from;
     Coord to;
     FIGURES fig_on_captured_field;
   }m_actual_move;
   std::vector<Moves> m_moves;
   std::vector<Moves> m_history_copy;
   std::vector<FIGURES> m_field;
   bool m_is_go_to_history_in_progress;
};
#endif //_VILVIVYVKCKYVLVUYFDKYUFYTF
