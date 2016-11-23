#include "chess.h"

#include <cstdlib>
#include <cctype>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <cmath>
#include <stdlib.h>


using namespace logic;

struct Board::impl_t
{
  impl_t();
  bool move(const Coord &from, const Coord &to);
  bool back_move();
  void start_new_game();
  void make_moves_from_str(const std::string &str);
  void go_to_history_index(const unsigned index);
  bool is_mate();
  std::string get_moves_history() const;
  void load_moves_from_file(const std::string &path);
  void write_moves_to_file(const std::string &path) const;
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
  enum {a_LETTER = 'a', EIGHT_ch = '8'};

  COLOR get_color(const Coord &c) const;
  COLOR get_move_color() const;
  FIGURE get_figure(const Coord &c) const;
  FIGURE get_figure(const unsigned x, const unsigned y) const;
  COLORLESS_FIG get_colorless_fig(const Coord &c) const;
  void en_passant();
  void re_en_passant();
  void pawn_re_transform();
  void pawn_transform(const Coord &c);
  unsigned get_move_num_from_0() const;
  unsigned get_field_index(const Coord &c) const;
  void if_castling(const Coord &fr, const Coord &to);
  void finish_move(const Coord &from, const Coord &to);
  void next_move(const Coord &from = Coord(), const Coord &to = Coord());
  void set_field(const Coord &lhs, const Coord &rhs, const FIGURE &fig = FREE_FIELD);

  bool is_check(const COLOR color) const;
  bool is_pawn_reach_other_side(const Coord &c) const;
  bool is_castling(const Coord &from, const Coord &to) const;
  bool is_can_move(const Coord &from, const Coord &to) const;
  bool is_en_passant(const Coord &fr, const Coord &to) const;

  int diff(const int _1, const int _2) const;
  struct Moves
  {
   Coord from;
   Coord to;
   FIGURE fig_on_captured_field;
   MOVE_TYPE type;
  }m_actual_move;
  std::vector<Moves> m_moves;
  std::vector<Moves> m_moves_copy;
  std::vector<FIGURE> m_field;
  bool m_is_go_to_history_running;
};

Board::Board()
    : impl(new impl_t)
{
}

Board::~Board()
{
}

bool Board::move(const Coord &from, const Coord &to)
{
  return impl->move(from, to);
}

bool Board::back_move()
{
  return impl->back_move();
}

void Board::start_new_game()
{
  impl->start_new_game();
}

void Board::make_moves_from_str(const std::string &str)
{
  return impl->make_moves_from_str(str);
}

void Board::go_to_history_index(const unsigned index)
{
  impl->go_to_history_index(index);
}

bool Board::is_mate()
{
  return impl->is_mate();
}

std::string Board::get_moves_history() const
{
  return impl->get_moves_history();
}

void Board::load_moves_from_file(const std::string &path)
{
  impl->load_moves_from_file(path);
}

void Board::write_moves_to_file(const std::string &path) const
{
  impl->write_moves_to_file(path);
}

unsigned Board::get_move_num() const
{
  return impl->get_move_num();
}

std::string Board::get_board_mask() const
{
  return impl->get_board_mask();
}


Board::impl_t::impl_t() : m_is_go_to_history_running(false)
{
  m_field.resize(BOARD_SIZE);
  m_field = {B_ROOK,B_HORSE,B_ELEPHANT,B_QUEEN,B_KING,B_ELEPHANT,B_HORSE,B_ROOK};
  m_field.insert(m_field.end(), BOARD_SIDE, B_PAWN);
  m_field.insert(m_field.end(), BOARD_SIDE * 4, FREE_FIELD);
  m_field.insert(m_field.end(), BOARD_SIDE, W_PAWN);

  std::vector<FIGURE> eight_row = {W_ROOK,W_HORSE,W_ELEPHANT,W_QUEEN,W_KING,W_ELEPHANT,W_HORSE,W_ROOK};
  m_field.insert(m_field.end(), eight_row.begin(), eight_row.end());
}


bool Board::impl_t::move(const Coord &from, const Coord &to)
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

void Board::impl_t::finish_move(const Coord &from, const Coord &to)
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

bool Board::impl_t::is_can_move(const Coord &fr, const Coord &to) const
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

  Coord c(fr);
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

bool Board::impl_t::is_en_passant(const Coord &fr, const Coord &to) const
{
  const auto &m = m_moves.back();
  bool is_cross = (abs(diff(to.x, fr.x) * diff(to.y, fr.y)) == 1);
  if(is_cross && get_colorless_fig(m.to) == PAWN)
    { return (m.to.y == fr.y && m.to.x == to.x && abs(diff(m.to.y, m.from.y)) == 2); }
  return false;
}

void Board::impl_t::en_passant()
{
  m_actual_move.type = EN_PASSANT;
  const auto ind = get_field_index(m_moves.back().to);
  m_field[ind] = FREE_FIELD;
}

bool Board::impl_t::is_pawn_reach_other_side(const Coord &c) const
{
  enum {FIRST_LINE = 0, LAST_LINE = 7};
  return (c.y == LAST_LINE || c.y == FIRST_LINE);
}

void Board::impl_t::pawn_transform(const Coord &c)
{
  m_actual_move.type = PAWN_TRANSFORM;
  auto &f = m_field[get_field_index(c)];
  if(get_figure(c) == W_PAWN)
    { f = W_QUEEN; }
  else f = B_QUEEN;
}

void Board::impl_t::if_castling(const Coord &fr, const Coord &to)
{
  const int dx = abs(diff(to.x, fr.x));
  const int X_UNIT_VEC = dx == 0 ? 0 : diff(to.x, fr.x)/dx;

  if(get_colorless_fig(to) == KING && dx > 1)
  {
    Coord rook_fr, rook_to;

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

bool Board::impl_t::is_castling(const Coord &fr, const Coord &to) const
{
  const int dx = abs(diff(to.x, fr.x));
  const int dy = abs(diff(to.y, fr.y));
  const int X_UNIT_VECTOR = dx == 0 ? 0 : diff(to.x, fr.x)/dx;

  const auto &field = m_field.begin() + get_field_index(fr) + X_UNIT_VECTOR;
  if(get_colorless_fig(fr) == KING && !is_check(get_color(fr)) && dy == 0 && dx == 2 && *field == FREE_FIELD
     && *(field + X_UNIT_VECTOR) == FREE_FIELD && (X_UNIT_VECTOR > 0 || *(field + 2 * X_UNIT_VECTOR) == FREE_FIELD))
  {
    Coord c((to.x > 4 ? 7 : 0), (get_color(fr) == WHITE ? 7 : 0));
    const bool is_rook_not_moved = (std::find_if(m_moves.begin(), m_moves.end(),
                                   [c](auto &i) {return (c == i.from || c == i.to);}) == m_moves.end());

    const bool is_king_not_moved = (std::find_if(m_moves.begin(), m_moves.end(),
                                   [fr](auto &i) {return (fr == i.from || fr == i.to);}) == m_moves.end());

    return is_king_not_moved && is_rook_not_moved;
  }
  return false;
}

bool Board::impl_t::is_check(const COLOR color) const
{
  if(color == NONE)
    { return false; }

  Coord f,t;
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

bool Board::impl_t::is_mate()
{
  Coord f,t;
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

void Board::impl_t::start_new_game()
{
  while(back_move());
  m_moves_copy.clear();
}

bool Board::impl_t::back_move()
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

void Board::impl_t::re_en_passant()
{
  const auto ind = get_field_index(m_moves[m_moves.size() - 2].to);
  if(get_color(m_moves.back().from) == BLACK)
    { m_field[ind] = W_PAWN; }
  else m_field[ind] = B_PAWN;
}

void Board::impl_t::pawn_re_transform()
{
  m_field[get_field_index(m_moves.back().from)] = (get_move_color() == BLACK) ? W_PAWN : B_PAWN;
}

void Board::impl_t::go_to_history_index(const unsigned index)
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

void Board::impl_t::make_moves_from_str(const std::string &str)
{
  enum{FROM_X = 0, FROM_Y = 1, TO_X = 2, TO_Y = 3, COORD_NEED_TO_MOVE = 4, h_LETTER = 'h', ONE_ch = '1'};

  std::vector<int> coord_str;
  for(auto res : str)
  {
    if(!((res >= a_LETTER && res <= h_LETTER) || (res >= ONE_ch && res <= EIGHT_ch)))
     {  continue; }

    coord_str.push_back(isalpha(res) ? res - a_LETTER : EIGHT_ch - res);

    if(coord_str.size() == COORD_NEED_TO_MOVE)
    {
      move(Coord(coord_str[FROM_X], coord_str[FROM_Y]),
           Coord(coord_str[TO_X], coord_str[TO_Y]));
      coord_str.clear();
    }
  }
}

std::string Board::impl_t::get_board_mask() const
{
  return std::string(m_field.begin(), m_field.end());
}

std::string Board::impl_t::get_moves_history() const
{
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

void Board::impl_t::next_move(const Coord &from, const Coord &to)
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

void Board::impl_t::write_moves_to_file(const std::string &path) const
{
  std::ofstream in_file(path);

  if(!in_file.is_open())
  {
    std::cout<<"Warning! in Board::write_moves_to_file: Couldn't open file."<<std::endl;
    return;
  }

  std::string history = get_moves_history();
  std::copy(history.begin(), history.end(), std::ostreambuf_iterator<char>(in_file));
}

void Board::impl_t::load_moves_from_file(const std::string &path)
{
  std::ifstream from_file(path);

  if(!from_file.is_open())
  {
    std::cout<<"Warning! in Board::write_moves_to_file: Couldn't open file."<<std::endl;
    return;
  }

  std::string data_from_file(std::istreambuf_iterator<char>(from_file), (std::istreambuf_iterator<char>()));

  start_new_game();
  make_moves_from_str(data_from_file);
}

unsigned Board::impl_t::get_move_num() const
{
  return m_moves.size();
}

unsigned Board::impl_t::get_move_num_from_0() const
{
  return get_move_num() ? get_move_num() - 1 : 0;
}

Board::impl_t::FIGURE Board::impl_t::get_figure(const Coord &c) const
{
  return m_field[get_field_index(c)];
}

Board::impl_t::FIGURE Board::impl_t::get_figure(const unsigned x, const unsigned y) const
{
  Coord c(x,y);
  return m_field[get_field_index(c)];
}

Board::impl_t::COLORLESS_FIG Board::impl_t::get_colorless_fig(const Coord &c) const
{
  const auto &f = m_field[get_field_index(c)];
  return COLORLESS_FIG(tolower(f) + toupper(f));
}

Board::impl_t::COLOR Board::impl_t::get_move_color() const
{
  return get_move_num() % 2 ? BLACK : WHITE;
}

void Board::impl_t::set_field(const Coord &lhs, const Coord &rhs, const FIGURE & fig)
{
  auto &f = m_field[get_field_index(rhs)];
  m_field[get_field_index(lhs)] = f;
  f = fig;
}

Board::impl_t::COLOR Board::impl_t::get_color(const Coord &c) const
{
  const auto &f = m_field[get_field_index(c)];
  if(f == FREE_FIELD)
    { return NONE; }

  return islower(f) ? WHITE : BLACK;
}

int Board::impl_t::diff(const int _1, const int _2) const
{
  return (_1 - _2);
}

unsigned Board::impl_t::get_field_index(const Coord &c) const
{
  const auto i = c.y * BOARD_SIDE + c.x;

  if(i >= m_field.size())
  {
    std::cout<<"Warning! in Board::get_field_index: Index out of range!"<<std::endl;
    return 0;
  }
  return i;
}
