#include <cstdlib>
#include <cctype>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <cmath>
#include <exception>
#include "chess.h"

Board::Board() : m_is_go_to_history_in_progress(false)
{
  m_field.resize(BOARD_SIZE);
  m_field = {B_ROOK,B_HORSE,B_ELEPHANT,B_QUEEN,B_KING,B_ELEPHANT,B_HORSE,B_ROOK};
  m_field.insert(m_field.end(), BOARD_SIDE, B_PAWN);
  m_field.insert(m_field.end(), BOARD_SIDE * 4, FREE_FIELD);
  m_field.insert(m_field.end(), BOARD_SIDE, W_PAWN);

  std::vector<FIGURE> eight_row = {W_ROOK,W_HORSE,W_ELEPHANT,W_QUEEN,W_KING,W_ELEPHANT,W_HORSE,W_ROOK};
  m_field.insert(m_field.end(), eight_row.begin(), eight_row.end());
}

bool Board::move(const Coord &from, const Coord &to)
{
  std::cout<<"Board::move CHESS "<<std::endl;
  if((get_color(from) == get_move_color()) && (is_can_move(from, to) || is_castling(from, to)))
  {
    set_field(to, from);
    if(!is_check(get_color(to)))
    {
      finish_move(from,to);
      return true;
    }
  }
  set_field(from, to);
  return false;
}

void Board::finish_move(const Coord &from, const Coord &to)
{
  if(get_colorless_fig(to) == PAWN && is_pawn_cross_beat(from, to))
    if_pawn_cross_beat();
  if_castling(from,to);
  next_move(from, to);
}

bool Board::is_can_move(const Coord &fr, const Coord &to) const
{
  const int dx = abs(diff(to.x, fr.x));
  const int dy = abs(diff(to.y, fr.y));
  const int X_UNIT_VECTOR = (dx == 0) ? 0 : diff(to.x, fr.x)/dx;
  const int Y_UNIT_VECTOR = (dy == 0) ? 0 : diff(to.y, fr.y)/dy;
  const COLORLESS_FIG FIG = get_colorless_fig(fr);

  if(FIG == HORSE)
    return (dx*dy == 2 && get_color(fr) != get_color(to));
  if(FIG == PAWN)
  {
    if((Y_UNIT_VECTOR < 0 && get_color(fr) == WHITE) || (Y_UNIT_VECTOR > 0 && get_color(fr) == BLACK))
    {
      if((get_figure(to) != FREE_FIELD))
        return (dy*dx == 1 && get_color(fr) != get_color(to));
      else if(dx == 0)
        return (dy == 1 || (dy == 2 && get_figure(to.x,fr.y + Y_UNIT_VECTOR) == FREE_FIELD && (fr.y == 6 || fr.y == 1)));
      else return is_pawn_cross_beat(fr,to);
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
      continue;

    return false;
  }
  return !(fr == to);
}

bool Board::is_pawn_cross_beat(const Coord &fr, const Coord &to) const
{
  const auto &m = m_moves[m_moves.size() - 2];
  if(abs(diff(to.x, fr.x) * diff(to.y, fr.y)) == 1 && get_colorless_fig(m.to) == PAWN)
    return (diff(m.to.y, fr.y) + diff(m.to.x, to.x) == 0 && abs(diff(m.to.y, m.from.y)) == 2);

  return false;
}

void Board::if_pawn_cross_beat()
{
  auto ind = get_field_index(m_moves[m_moves.size() - 2].to);
  m_field[ind] = FREE_FIELD;
}

void Board::if_castling(const Coord &fr, const Coord &to)
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
    }
    set_field(rook_to, rook_fr);
  }
}

bool Board::is_castling(const Coord &fr, const Coord &to) const
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
                                   [c](auto const &i) {return (c == i.from || c == i.to);}) == m_moves.end());

    const bool is_king_not_moved = (std::find_if(m_moves.begin(), m_moves.end(),
                                   [fr](auto const &i) {return (fr == i.from || fr == i.to);}) == m_moves.end());

    return is_king_not_moved && is_rook_not_moved;
  }
  return false;
}

bool Board::is_check(const COLOR color) const
{
  if(color == NONE)
    return false;

  Coord f,t;
  for(t.x = 0; t.x < BOARD_SIDE; ++t.x)
    for(t.y = 0; t.y < BOARD_SIDE; ++t.y)
      if(get_colorless_fig(t) == KING && get_color(t) == color)
      {
        for(f.x = 0; f.x < BOARD_SIDE; ++f.x)
        {
          for(f.y = 0; f.y < BOARD_SIDE; ++f.y)
            if(get_color(f) != color && is_can_move(f, t))
              return true;
        }
        return false;
      }
  return false;
}

bool Board::is_mate()
{
  Coord f,t;
  for(f.x = 0; f.x < BOARD_SIDE; ++f.x)
  {
    for(f.y = 0; f.y < BOARD_SIDE; ++f.y)
      if(get_figure(f) != FREE_FIELD && get_color(f) == get_move_color())
      {
        for(t.x = 0; t.x < BOARD_SIDE; ++t.x)
          for(t.y = 0; t.y < BOARD_SIDE; ++t.y)
            if(is_can_move(f, t))
            {
              const FIGURE FIG_TO = get_figure(t);
              set_field(t, f);

              const bool is_mate = is_check(get_move_color());
              set_field(f, t, FIG_TO);

              if(!is_mate)
                return false;
            }
      }
  }
  return true;
}

void Board::go_to_history_index(const unsigned index)
{
  m_is_go_to_history_in_progress = true;

  std::cout<<"Board::go_to_history_index: "<<index<<" "<<get_move_num()<<std::endl;

  while(index < get_move_num())
    back_move();

  if(index <= m_moves_copy.size())
    for(unsigned i = get_move_num(); i < index; ++i)
      move(m_moves_copy[i].from, m_moves_copy[i].to);

  m_is_go_to_history_in_progress = false;
}

void Board::make_moves_from_str(const std::string &str)
{
  std::cout<<"Board::make_move_from_str CHESS: "<<str<<std::endl;
  enum{FROM_X = 0, FROM_Y = 1, TO_X = 2, TO_Y = 3, COORD_NEED_TO_MOVE = 4, h_LETTER = 'h', ONE_ch = '1'};

  std::vector<int> coord_str;
  auto res = str.begin();
  while(res != str.end())
  {
    res = std::find_if(res, str.end(),
                 [](auto const& i) {return ((i >= a_LETTER && i <= h_LETTER) || (i >= ONE_ch && i <= EIGHT_ch));});

    coord_str.push_back(isalpha(*res) ? *res - a_LETTER : EIGHT_ch - *res);
    ++res;

    if(coord_str.size() == COORD_NEED_TO_MOVE)
    {
      move(Coord(coord_str[FROM_X], coord_str[FROM_Y]),
           Coord(coord_str[TO_X], coord_str[TO_Y]));
      coord_str.clear();
    }
  }
}

const std::string Board::get_board_mask() const
{
  std::cout<<"Board::get_board_mask()"<<std::endl;
  return std::string(m_field.begin(), m_field.end());
}

const std::string Board::get_moves_history() const
{
  std::cout<<"Board::get_moves_history: "<<std::endl;

  std::string history;
  for(auto hist_elem : m_moves_copy)
  {
    history.push_back(hist_elem.from.x + a_LETTER);
    history.push_back(EIGHT_ch - hist_elem.from.y);
    history.push_back(hist_elem.to.x + a_LETTER);
    history.push_back(EIGHT_ch - hist_elem.to.y);
  }
  return history;
}

void Board::write_moves_to_file(const std::string &path) const
{
  std::cout<<"Board::write_moves_to_file "<<std::endl;
  std::ofstream in_file(path);

  try
  {
    if(!in_file.is_open())
      throw "Couldn't open file.";

    std::string history = get_moves_history();
    std::copy(history.begin(), history.end(), std::ostreambuf_iterator<char>(in_file));
  }
  catch(const char *ex)
  {
    std::cout<<"!Exception! Board::write_moves_from_file "<<ex<<std::endl;
  }
}

void Board::load_moves_from_file(const std::string &path)
{
  std::cout<<"Board::read_moves_from_file "<<std::endl;
  std::ifstream from_file(path);

  try
  {
    if(!from_file.is_open())
      throw "Couldn't open file.";

    std::string data_from_file(std::istreambuf_iterator<char>(from_file), (std::istreambuf_iterator<char>()));

    std::cout<<"data_from_file: "<<data_from_file<<std::endl;
    start_new_game();
    make_moves_from_str(data_from_file);
  }
  catch(const char *ex)
  {
    std::cout<<"!Exception! Board::load_moves_from_file "<<ex<<std::endl;
  }
}

void Board::start_new_game()
{
  while(back_move());
  m_moves_copy.clear();
}

bool Board::back_move()
{
  std::cout<<"Board::back_move"<<std::endl;
  if(!get_move_num())
    return false;

  const Moves &m = m_moves.back();
  set_field(m.from, m.to, m.fig_on_captured_field);

  if(get_colorless_fig(m.to) == PAWN && is_pawn_cross_beat(m.from, m.to))
  {
    auto ind = get_field_index(m_moves[m_moves.size() - 2].to);
    m_field[ind] = FREE_FIELD;
    if(get_color(m.to) == BLACK)
      m_field[ind] = W_PAWN;
    else m_field[ind] = B_PAWN;
  }
  if_castling(m.to, m.from);
  m_moves.pop_back();
  return true;
}

void Board::next_move(const Coord &from, const Coord &to)
{
  m_actual_move.to = to;
  m_actual_move.from = from;
  m_actual_move.fig_on_captured_field = get_figure(to);
  m_moves.push_back(m_actual_move);

  if(!m_is_go_to_history_in_progress)
  {
    if(get_move_num() < m_moves_copy.size())
      m_moves_copy.erase(m_moves_copy.begin() + get_move_num_from_0(), m_moves_copy.end());

    m_moves_copy.push_back(m_moves.back());
  }
}

unsigned Board::get_move_num() const
{
  return m_moves.size();
}

unsigned Board::get_move_num_from_0() const
{
  return get_move_num() ? get_move_num() - 1 : 0;
}

Board::FIGURE Board::get_figure(const Coord &c) const
{
  return m_field[get_field_index(c)];
}

Board::FIGURE Board::get_figure(const unsigned x, const unsigned y) const
{
  return m_field[y * BOARD_SIDE + x];
}

Board::COLORLESS_FIG Board::get_colorless_fig(const Coord &c) const
{
  return COLORLESS_FIG(tolower(m_field[get_field_index(c)]) + toupper(m_field[get_field_index(c)]));
}

Board::COLOR Board::get_move_color() const
{
  return get_move_num() % 2 ? BLACK : WHITE;
}

void Board::set_field(const Coord &lhs, const Coord &rhs, const FIGURE & fig)
{
  m_field[get_field_index(lhs)] = m_field[get_field_index(rhs)];
  m_field[get_field_index(rhs)] = fig;
}

Board::COLOR Board::get_color(const Coord &c) const
{
  if(m_field[get_field_index(c)] == FREE_FIELD)
    return NONE;

  return islower(m_field[get_field_index(c)]) ? WHITE : BLACK;
}

int Board::diff(const int _1, const int _2) const
{
  return (_1 - _2);
}

unsigned Board::get_field_index(const Coord &c) const
{
  return c.y * BOARD_SIDE + c.x;
}

Board::Coord::Coord(const unsigned X, const unsigned Y) : x(X), y(Y)
{
}

bool Board::Coord::operator==(const Coord &rhs) const
{
  return(x == rhs.x && y == rhs.y);
}

