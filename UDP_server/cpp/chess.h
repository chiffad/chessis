#ifndef _VILVIVYVKCKYVLVUYFDKYUFYTF
#define _VILVIVYVKCKYVLVUYFDKYUFYTF
#include <iostream>
#include <string>
#include <vector>

class Board
{
public:
   enum COLOR {NONE, WHITE, BLACK};
   enum FIGURE {B_QUEEN = 'Q', B_KING = 'K', B_ROOK = 'R', B_ELEPHANT = 'E', B_HORSE = 'H', B_PAWN = 'P',
                 W_QUEEN = 'q', W_KING = 'k', W_ROOK = 'r', W_ELEPHANT = 'e', W_HORSE = 'h', W_PAWN = 'p',
                 FREE_FIELD = '.'};
   enum COLORLESS_FIG {NOT_FIGURE = FREE_FIELD, QUEEN = B_QUEEN + W_QUEEN, KING = W_KING + B_KING,
                       ROOK = W_ROOK + B_ROOK, PAWN = W_PAWN + B_PAWN, ELEPHANT = W_ELEPHANT + B_ELEPHANT,
                       HORSE = W_HORSE + B_HORSE};
   enum {BOARD_SIDE = 8, FIGURES_NUMBER = 32, BOARD_SIZE = BOARD_SIDE * BOARD_SIDE};

public:
   struct Coord
   {
     Coord(const unsigned X = 0, const unsigned Y = 0);
     bool operator==(const Coord& rhs) const;
     unsigned x;
     unsigned y;
   };

public:
   Board();

   bool move(const Coord &from, const Coord &to);
   bool back_move();
   void start_new_game();
   void make_moves_from_str(const std::string &str);
   void go_to_history_index(const unsigned index);
   bool is_mate();

   const std::string get_moves_history() const;
   void load_moves_from_file(const std::string &path);
   void write_moves_to_file(const std::string &path) const;

   unsigned get_move_num() const;
   unsigned get_move_num_from_0() const;
   const std::string get_board_mask() const;
   COLOR get_move_color() const;
   COLOR get_color(const Coord &c) const;
   FIGURE get_figure(const Coord &c) const;
   FIGURE get_figure(const unsigned x, const unsigned y) const;
   COLORLESS_FIG get_colorless_fig(const Coord &c) const;

public:
   Board(const Board&) = delete;
   Board& operator=(const Board&) = delete;

private:
   void if_pawn_cross_beat();
   unsigned get_field_index(const Coord &c) const;
   void if_castling(const Coord &fr, const Coord &to);
   void finish_move(const Coord &from, const Coord &to);
   void next_move(const Coord &from = Coord(), const Coord &to = Coord());
   void set_field(const Coord &lhs, const Coord &rhs, const FIGURE &fig = FREE_FIELD);

   bool is_check(const COLOR color) const;
   bool is_castling(const Coord &from, const Coord &to) const;
   bool is_can_move(const Coord &from, const Coord &to) const;
   bool is_pawn_cross_beat(const Coord &fr, const Coord &to) const;

   int diff(const int _1, const int _2) const;

private:
   struct Moves
   {
     Coord from;
     Coord to;
     FIGURE fig_on_captured_field;
   }m_actual_move;
   std::vector<Moves> m_moves;
   std::vector<Moves> m_moves_copy;
   std::vector<FIGURE> m_field;
   bool m_is_go_to_history_in_progress;

private:
 enum{a_LETTER = 'a', EIGHT_ch = '8'};
};
#endif //_VILVIVYVKCKYVLVUYFDKYUFYTF
