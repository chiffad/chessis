#include "chess.h"

#include <cstdlib>
#include <cctype>
#include <algorithm>
#include <iterator>
#include <cmath>
#include <stdlib.h>


using namespace logic;

struct board_t::impl_t
{
  impl_t();
  bool move(const coord_t &from, const coord_t &to);
  bool back_move();
  void start_new_game();
  void go_to_history_index(const unsigned index);
  bool is_mate();
  std::string get_moves_history() const;
  unsigned get_move_num() const;
  std::string get_board_mask() const;

  enum COLOR {NONE, WHITE, BLACK};
  enum FIGURE {B_QUEEN = 'Q', B_KING = 'K', B_ROOK = 'R', B_ELEPHANT = 'E', B_HORSE = 'H', B_PAWN = 'P',
               W_QUEEN = 'q', W_KING = 'k', W_ROOK = 'r', W_ELEPHANT = 'e', W_HORSE = 'h', W_PAWN = 'p',
               FREE_FIELD = '.'};
  enum COLORLESS_FIG {NOT_FIGURE = FREE_FIELD, QUEEN = B_QUEEN + W_QUEEN, KING = W_KING + B_KING,
                      ROOK = W_ROOK + B_ROOK, PAWN = W_PAWN + B_PAWN, ELEPHANT = W_ELEPHANT + B_ELEPHANT,
                      HORSE = W_HORSE + B_HORSE};
  enum {BOARD_SIDE = 8, FIGURES_NUMBER = 32, BOARD_SIZE = BOARD_SIDE * BOARD_SIDE};
  enum MOVE_TYPE{USUAL, CASTLING, EN_PASSANT, PAWN_TRANSFORM};

  COLOR get_color(const coord_t &c) const;
  COLOR get_move_color() const;
  FIGURE get_figure(const coord_t &c) const;
  FIGURE get_figure(const unsigned x, const unsigned y) const;
  COLORLESS_FIG get_colorless_fig(const coord_t &c) const;
  void en_passant();
  void re_en_passant();
  void pawn_re_transform();
  void pawn_transform(const coord_t &c);
  unsigned get_move_num_from_0() const;
  unsigned get_field_index(const coord_t &c) const;
  void if_castling(const coord_t &fr, const coord_t &to);
  void finish_move(const coord_t &from, const coord_t &to);
  void next_move(const coord_t &from = coord_t(), const coord_t &to = coord_t());
  void set_field(const coord_t &lhs, const coord_t &rhs, const FIGURE &fig = FREE_FIELD);

  bool is_check(const COLOR color) const;
  bool is_pawn_reach_other_side(const coord_t &c) const;
  bool is_castling(const coord_t &from, const coord_t &to) const;
  bool is_can_move(const coord_t &from, const coord_t &to) const;
  bool is_en_passant(const coord_t &fr, const coord_t &to) const;

  int diff(const int _1, const int _2) const;

  struct Moves
  {
   coord_t from;
   coord_t to;
   FIGURE fig_on_captured_field;
   MOVE_TYPE type;
  }m_actual_move;
  std::vector<Moves> m_moves;
  std::vector<Moves> m_moves_copy;
  std::vector<FIGURE> m_field;
  bool m_is_go_to_history_running;
};

board_t::board_t()
    : impl(new impl_t)
{
}

board_t::~board_t()
{
}

bool board_t::move(const coord_t &from, const coord_t &to)
{
  return impl->move(from, to);
}

bool board_t::back_move()
{
  return impl->back_move();
}

void board_t::start_new_game()
{
  impl->start_new_game();
}

void board_t::go_to_history_index(const unsigned index)
{
  impl->go_to_history_index(index);
}

bool board_t::is_mate()
{
  return impl->is_mate();
}

std::string board_t::get_moves_history() const
{
  return impl->get_moves_history();
}

unsigned board_t::get_move_num() const
{
  return impl->get_move_num();
}

std::string board_t::get_board_mask() const
{
  return impl->get_board_mask();
}

board_t::impl_t::impl_t() : m_is_go_to_history_running(false)
{
  m_field.resize(BOARD_SIZE);
  m_field = {B_ROOK,B_HORSE,B_ELEPHANT,B_QUEEN,B_KING,B_ELEPHANT,B_HORSE,B_ROOK};
  m_field.insert(m_field.end(), BOARD_SIDE, B_PAWN);
  m_field.insert(m_field.end(), BOARD_SIDE * 4, FREE_FIELD);
  m_field.insert(m_field.end(), BOARD_SIDE, W_PAWN);

  std::vector<FIGURE> eight_row = {W_ROOK,W_HORSE,W_ELEPHANT,W_QUEEN,W_KING,W_ELEPHANT,W_HORSE,W_ROOK};
  m_field.insert(m_field.end(), eight_row.begin(), eight_row.end());
}

bool board_t::impl_t::move(const coord_t &from, const coord_t &to)
{
  if(from == to)
    { return false; }

  if((get_color(from) == get_move_color()) && (is_can_move(from, to) || is_castling(from, to)))
  {
    m_actual_move.fig_on_captured_field = get_figure(to);
    set_field(to, from);
    if(!is_check(get_color(to)))
    {
      finish_move(from,to);
      return true;
    }
    else set_field(from, to);
  }
  return false;
}

void board_t::impl_t::finish_move(const coord_t &from, const coord_t &to)
{
  m_actual_move.type = USUAL;
  if(get_colorless_fig(to) == PAWN)
  {
    if(is_en_passant(from, to))
      { en_passant(); }
    else if(is_pawn_reach_other_side(to))
      { pawn_transform(to); }
  }
  if_castling(from,to);
  next_move(from, to);
}

bool board_t::impl_t::is_can_move(const coord_t &fr, const coord_t &to) const
{
  const int dx = abs(diff(to.x, fr.x));
  const int dy = abs(diff(to.y, fr.y));
  const int X_UNIT_VECTOR = (dx == 0) ? 0 : diff(to.x, fr.x)/dx;
  const int Y_UNIT_VECTOR = (dy == 0) ? 0 : diff(to.y, fr.y)/dy;
  const auto FIG = get_colorless_fig(fr);

  if(FIG == HORSE)
    { return (dx*dy == 2 && get_color(fr) != get_color(to)); }
  if(FIG == PAWN)
  {
    if((Y_UNIT_VECTOR < 0 && get_color(fr) == WHITE) || (Y_UNIT_VECTOR > 0 && get_color(fr) == BLACK))
    {
      if((get_figure(to) != FREE_FIELD))
        { return (dy*dx == 1 && get_color(fr) != get_color(to)); }
      else if(dx == 0)
        { return (dy == 1 || (dy == 2 && get_figure(to.x,fr.y + Y_UNIT_VECTOR) == FREE_FIELD && (fr.y == 6 || fr.y == 1))); }
      else return is_en_passant(fr,to);
    }
    return false;
  }
  else if(FIG == KING && dx <= 1 && dy <= 1);
  else if((FIG == ROOK || FIG == QUEEN) && (dx*dy == 0));
  else if((FIG == ELEPHANT || FIG == QUEEN) && (dx == dy));
  else return false;

  coord_t c(fr);
  while(!(c == to))
  {
    c.x += X_UNIT_VECTOR;
    c.y += Y_UNIT_VECTOR;
    if(get_figure(c) == FREE_FIELD || (get_color(to) != get_color(fr) && c == to))
      { continue; }

    return false;
  }
  return true;
}

bool board_t::impl_t::is_en_passant(const coord_t &fr, const coord_t &to) const
{
  const auto &m = m_moves.back();
  bool is_cross = (abs(diff(to.x, fr.x) * diff(to.y, fr.y)) == 1);
  if(is_cross && get_colorless_fig(m.to) == PAWN)
    { return (m.to.y == fr.y && m.to.x == to.x && abs(diff(m.to.y, m.from.y)) == 2); }
  return false;
}

void board_t::impl_t::en_passant()
{
  m_actual_move.type = EN_PASSANT;
  const auto ind = get_field_index(m_moves.back().to);
  m_field[ind] = FREE_FIELD;
}

bool board_t::impl_t::is_pawn_reach_other_side(const coord_t &c) const
{
  enum {FIRST_LINE = 0, LAST_LINE = 7};
  return (c.y == LAST_LINE || c.y == FIRST_LINE);
}

void board_t::impl_t::pawn_transform(const coord_t &c)
{
  m_actual_move.type = PAWN_TRANSFORM;
  auto &f = m_field[get_field_index(c)];
  if(get_figure(c) == W_PAWN)
    { f = W_QUEEN; }
  else f = B_QUEEN;
}

void board_t::impl_t::if_castling(const coord_t &fr, const coord_t &to)
{
  const int dx = abs(diff(to.x, fr.x));
  const int X_UNIT_VEC = dx == 0 ? 0 : diff(to.x, fr.x)/dx;

  if(get_colorless_fig(to) == KING && dx > 1)
  {
    coord_t rook_fr, rook_to;

    rook_fr.y = rook_to.y = to.y;
    if(fr.x == 6 || fr.x == 2)
    {
      rook_fr.x = fr.x + X_UNIT_VEC;
      rook_to.x = fr.x - X_UNIT_VEC * (X_UNIT_VEC > 0 ? 2 : 1);
    }
    else
    {
      rook_fr.x = to.x + X_UNIT_VEC * (X_UNIT_VEC > 0 ? 1 : 2);
      rook_to.x = to.x - X_UNIT_VEC;
      m_actual_move.type = CASTLING;
    }
    set_field(rook_to, rook_fr);
  }
}

bool board_t::impl_t::is_castling(const coord_t &fr, const coord_t &to) const
{
  const int dx = abs(diff(to.x, fr.x));
  const int dy = abs(diff(to.y, fr.y));
  const int X_UNIT_VECTOR = dx == 0 ? 0 : diff(to.x, fr.x)/dx;

  const auto &field = m_field.begin() + get_field_index(fr) + X_UNIT_VECTOR;
  if(get_colorless_fig(fr) == KING && !is_check(get_color(fr)) && dy == 0 && dx == 2 && *field == FREE_FIELD
     && *(field + X_UNIT_VECTOR) == FREE_FIELD && (X_UNIT_VECTOR > 0 || *(field + 2 * X_UNIT_VECTOR) == FREE_FIELD))
  {
    coord_t c((to.x > 4 ? 7 : 0), (get_color(fr) == WHITE ? 7 : 0));
    const bool is_rook_not_moved = (std::find_if(m_moves.begin(), m_moves.end(),
                                   [c](auto &i) {return (c == i.from || c == i.to);}) == m_moves.end());

    const bool is_king_not_moved = (std::find_if(m_moves.begin(), m_moves.end(),
                                   [fr](auto &i) {return (fr == i.from || fr == i.to);}) == m_moves.end());

    return is_king_not_moved && is_rook_not_moved;
  }
  return false;
}

bool board_t::impl_t::is_check(const COLOR color) const
{
  if(color == NONE)
    { return false; }

  coord_t f,t;
  const auto king = (color == WHITE) ? W_KING : B_KING;
  for(t.x = 0; t.x < BOARD_SIDE; ++t.x)
    for(t.y = 0; t.y < BOARD_SIDE; ++t.y)
      if(get_figure(t) == king)
      {
        const auto opp_color = (color == WHITE) ? BLACK : WHITE;
        for(f.x = 0; f.x < BOARD_SIDE; ++f.x)
        {
          for(f.y = 0; f.y < BOARD_SIDE; ++f.y)
          {
            if(get_color(f) == opp_color && is_can_move(f, t))
             { return true; }
          }
        }
        return false;
      }
  return false;
}

bool board_t::impl_t::is_mate()
{
  coord_t f,t;
  for(f.x = 0; f.x < BOARD_SIDE; ++f.x)
  {
    for(f.y = 0; f.y < BOARD_SIDE; ++f.y)
      if(get_color(f) == get_move_color())
      {
        for(t.x = 0; t.x < BOARD_SIDE; ++t.x)
          for(t.y = 0; t.y < BOARD_SIDE; ++t.y)
          {
            if(is_can_move(f, t))
            {
              const FIGURE FIG_TO = get_figure(t);
              set_field(t, f);

              const bool is_mate = is_check(get_move_color());
              set_field(f, t, FIG_TO);

              if(!is_mate)
                { return false; }
            }
          }
      }
  }
  return true;
}

void board_t::impl_t::start_new_game()
{
  while(back_move());
  m_moves_copy.clear();
}

bool board_t::impl_t::back_move()
{
  if(!get_move_num())
    { return false; }

  const Moves &m = m_moves.back();
  set_field(m.from, m.to, m.fig_on_captured_field);

  if(m.type == EN_PASSANT)
    { re_en_passant(); }
  else if(m.type == PAWN_TRANSFORM)
    { pawn_re_transform(); }
  else if(m.type == CASTLING)
    { if_castling(m.to, m.from); }

  m_moves.pop_back();
  return true;
}

void board_t::impl_t::re_en_passant()
{
  const auto ind = get_field_index(m_moves[m_moves.size() - 2].to);
  if(get_color(m_moves.back().from) == BLACK)
    { m_field[ind] = W_PAWN; }
  else m_field[ind] = B_PAWN;
}

void board_t::impl_t::pawn_re_transform()
{
  m_field[get_field_index(m_moves.back().from)] = (get_move_color() == BLACK) ? W_PAWN : B_PAWN;
}

void board_t::impl_t::go_to_history_index(const unsigned index)
{
  m_is_go_to_history_running = true;

  while(index < get_move_num())
    { back_move(); }

  if(index <= m_moves_copy.size())
  {
    for(unsigned i = get_move_num(); i < index; ++i)
      { move(m_moves_copy[i].from, m_moves_copy[i].to); }
  }

  m_is_go_to_history_running = false;
}

std::string board_t::impl_t::get_board_mask() const
{
  return std::string(m_field.begin(), m_field.end());
}

std::string board_t::impl_t::get_moves_history() const
{
  enum {a_LETTER = 'a', EIGHT_ch = '8'};
  std::string history;
  for(auto hirst_elem : m_moves_copy)
  {
    history.push_back(hirst_elem.from.x + a_LETTER);
    history.push_back(EIGHT_ch - hirst_elem.from.y);
    history.push_back(hirst_elem.to.x + a_LETTER);
    history.push_back(EIGHT_ch - hirst_elem.to.y);
  }
  return history;
}

void board_t::impl_t::next_move(const coord_t &from, const coord_t &to)
{
  m_actual_move.to = to;
  m_actual_move.from = from;
  m_moves.push_back(m_actual_move);

  if(!m_is_go_to_history_running)
  {
    if(get_move_num() < m_moves_copy.size())
      { m_moves_copy.erase(m_moves_copy.begin() + get_move_num_from_0(), m_moves_copy.end()); }

    m_moves_copy.push_back(m_moves.back());
  }
}

unsigned board_t::impl_t::get_move_num() const
{
  return m_moves.size();
}

unsigned board_t::impl_t::get_move_num_from_0() const
{
  return get_move_num() ? get_move_num() - 1 : 0;
}

board_t::impl_t::FIGURE board_t::impl_t::get_figure(const coord_t &c) const
{
  return m_field[get_field_index(c)];
}

board_t::impl_t::FIGURE board_t::impl_t::get_figure(const unsigned x, const unsigned y) const
{
  coord_t c(x,y);
  return m_field[get_field_index(c)];
}

board_t::impl_t::COLORLESS_FIG board_t::impl_t::get_colorless_fig(const coord_t &c) const
{
  const auto &f = m_field[get_field_index(c)];
  return COLORLESS_FIG(tolower(f) + toupper(f));
}

board_t::impl_t::COLOR board_t::impl_t::get_move_color() const
{
  return get_move_num() % 2 ? BLACK : WHITE;
}

void board_t::impl_t::set_field(const coord_t &lhs, const coord_t &rhs, const FIGURE & fig)
{
  auto &f = m_field[get_field_index(rhs)];
  m_field[get_field_index(lhs)] = f;
  f = fig;
}

board_t::impl_t::COLOR board_t::impl_t::get_color(const coord_t &c) const
{
  const auto &f = m_field[get_field_index(c)];
  if(f == FREE_FIELD)
    { return NONE; }

  return islower(f) ? WHITE : BLACK;
}

int board_t::impl_t::diff(const int _1, const int _2) const
{
  return (_1 - _2);
}

unsigned board_t::impl_t::get_field_index(const coord_t &c) const
{
  const auto i = c.y * BOARD_SIDE + c.x;

  if(i >= m_field.size())
  {
    std::cout<<"Warning! in Board::get_field_index: Index out of range!"<<std::endl;
    return 0;
  }
  return i;
}
