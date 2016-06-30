#include<cstdlib>
#include<cctype>
#include<fstream>
#include<sstream>
#include <algorithm>
#include<math.h>
#include"headers/chess.h"

Board::Board() : m_is_go_to_history_in_progress(false)
{  
  m_field = {B_ROOK,B_HORSE,B_ELEPHANT,B_QUEEN,B_KING,B_ELEPHANT,B_HORSE,B_ROOK};
  m_field.insert(m_field.end(), BOARD_SIZE, B_PAWN);
  m_field.insert(m_field.end(), BOARD_SIZE * 4, FREE_FIELD);
  m_field.insert(m_field.end(), BOARD_SIZE, B_PAWN);

  std::vector<FIGURES> eight_row = {W_ROOK,W_HORSE,W_ELEPHANT,W_QUEEN,W_KING,W_ELEPHANT,W_HORSE,W_ROOK};
  m_field.insert(m_field.end(), eight_row.begin(), eight_row.end());
}

bool Board::move(const Coord &from, const Coord &to)
{
  std::cout<<"====move CHESS"<<std::endl;
  if(is_right_move_turn(from) && (is_can_move(from, to) || is_castling(from, to)))
    field_change(from, to);
  else return false;

  if(!is_check(get_color(to)))
  {
    if_castling(from,to);
    next_move(from, to);
    return true;
  }
  field_change(to, from);
  return false;
}

bool Board::is_right_move_turn(const Coord &c) const
{
  if((get_actual_move() == 1 && get_color(c) == B_FIG)
     || (get_actual_move() > 1 && get_color(c) != get_move_color()))
   return false;

  return true;
}

void Board::field_change(const Coord &from, const Coord &to)
{
  m_actual_move.fig_on_captured_field = get_figure(to);
  set_field(to, from);
  set_field(from, FREE_FIELD);
}

bool Board::is_can_move(const Coord &fr, const Coord &to) const
{
  const int dx = abs(to.x - fr.x);
  const int dy = abs(to.y - fr.y);
  const int X_UNIT_VECTOR = (dx == 0) ? 0 : (to.x - fr.x)/dx;
  const int Y_UNIT_VECTOR = (dy == 0) ? 0 : (to.y - fr.y)/dy;

  if(get_colorless_figure(fr) == HORSE && dx*dy == 2 && get_color(fr) != get_color(to))
    return true;

  if(get_colorless_figure(fr) == PAWN && ((Y_UNIT_VECTOR < 0 && get_color(fr) == W_FIG)
                                           || (Y_UNIT_VECTOR > 0 && get_color(fr) == B_FIG)))
  {
    return((get_figure(to) != FREE_FIELD && dy*dx == 1 && get_color(fr) != get_color(to))
           || (dx == 0 && ((dy == 1 && get_figure(to) == FREE_FIELD)
               || (dy == 2 && get_figure(to.x,fr.y + Y_UNIT_VECTOR) == FREE_FIELD && (fr.y == 6 || fr.y == 1)))));
  }
  else if(get_colorless_figure(fr) == KING && dx <= 1 && dy <= 1);
  else if((get_colorless_figure(fr) == ROOK || get_colorless_figure(fr) == QUEEN) && (dx*dy == 0));
  else if((get_colorless_figure(fr) == ELEPHANT || get_colorless_figure(fr) == QUEEN) && (dx == dy));
  else return false;

  Coord coord(fr);
  while(!(coord == to))
  {
    coord.x += X_UNIT_VECTOR;
    coord.y += Y_UNIT_VECTOR;
    if(get_figure(coord) != FREE_FIELD && get_color(to) != get_color(fr) && !(coord == to))
      goto failed;
  }
  return true;
  failed:
    return false;
}

void Board::if_castling(const Coord &fr, const Coord &to)
{
  const int dx = abs(to.x - fr.x);
  const int X_UNIT_VEC = dx == 0 ? 0 : (to.x - fr.x)/dx;

  if(get_colorless_figure(to) == KING && dx > 1)
  {
    Coord rook_fr;
    Coord rook_to;

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
    FIGURES fig = get_figure(rook_fr);
    set_field(rook_fr, FREE_FIELD);
    set_field(rook_to, fig);
  }
}

bool Board::is_castling(const Coord &fr, const Coord &to) const
{
  const int dx = abs(to.x - fr.x);
  const int X_UNIT_VECTOR = dx == 0 ? 0 : (to.x - fr.x)/dx;

  if(get_colorless_figure(fr) == KING && !is_check(get_color(fr)) && abs(to.y - fr.y) == 0 && dx == 2
     && get_figure(fr.x + X_UNIT_VECTOR, fr.y) == FREE_FIELD
     && get_figure(fr.x + 2 * X_UNIT_VECTOR, fr.y) == FREE_FIELD
     && (X_UNIT_VECTOR > 0 || get_figure(fr.x + 3 * X_UNIT_VECTOR, fr.y) == FREE_FIELD))
  {
    const bool IS_TWO_ZEROS = to.x > 4;
    Coord coord((IS_TWO_ZEROS ? 7 : 0), (get_color(fr) == W_FIG ? 7 : 0));

    for(unsigned i = 0; i < get_actual_move(); ++i)
      if((fr.x == 4 && (fr.y == 0 || fr.y == 7)) || coord == m_moves[i].from || coord == m_moves[i].to)
        goto cant_castling;

    return true;
  }
  return false;
  cant_castling:
    return false;
}

bool Board::is_check(const COLOR color) const
{
  if(get_actual_move() < 2 || color == NONE)
    return false;

  Coord f;
  Coord t;
  for(t.x = 0; t.x < BOARD_SIZE; ++t.x)
    for(t.y = 0; t.y < BOARD_SIZE; ++t.y)
      if(get_colorless_figure(t) == KING && get_color(t) == color)
      {
        for(f.x = 0; f.x < BOARD_SIZE; ++f.x)
          for(f.y = 0; f.y < BOARD_SIZE; ++f.y) 
            if(get_color(f) != color)
            {
              if(is_can_move(f, t))
                goto check;
            }
        goto not_check;
      }

  not_check:
    return false;

  check:
    return true;
}

bool Board::is_mate()
{
  if(get_actual_move() < 2)
    return false;

  Coord f;
  Coord t;
  for(f.x = 0; f.x < BOARD_SIZE; ++f.x)
    for(f.y = 0; f.y < BOARD_SIZE; ++f.y)
      if(get_figure(f) != FREE_FIELD && get_color(f) == get_move_color())
      {
        for(t.x = 0; t.x < BOARD_SIZE; ++t.x)
          for(t.y = 0; t.y < BOARD_SIZE; ++t.y)
            if(is_can_move(f, t))
            {
              FIGURES fig_from = get_figure(f);
              FIGURES fig_to   = get_figure(t);
              set_field(f,FREE_FIELD);
              set_field(t,fig_from);
              if(!is_check(get_move_color()))
              {
                set_field(f,fig_from);
                set_field(t,fig_to);
                goto not_mate;
              }
              else
              {
                set_field(f,fig_from);
                set_field(t,fig_to);
              }
            }
      }

  return true;

  not_mate:
    return false;
}

void Board::go_to_history_index(const unsigned index)
{
  m_is_go_to_history_in_progress = true;

  if(index < get_last_made_move())
    for(unsigned i = index; i < get_actual_move(); ++i)
      back_move(); 

  else if(index > get_last_made_move() && index <= m_history_copy.size())
    for(unsigned i = get_last_made_move(); i < index; ++i)
      move(m_history_copy[i].from, m_history_copy[i].to);

  m_is_go_to_history_in_progress = false;
}

void Board::make_moves_from_str(const std::string &str)
{
  std::cout<<"==make_move_from_str CHESS: "<<str<<std::endl;
  enum{FROM_X = 0, FROM_Y = 1, TO_X = 2, TO_Y = 3, COORD_NEED_TO_MOVE = 4};

  std::vector<int> coord_str;
  for(unsigned i = 0; i < str.size(); ++i)
  {
    if(!((str[i] >= a_LETTER && str[i] <= h_LETTER) || (str[i] >= ONE_ch && str[i] <= EIGHT_ch)))
      continue;

    coord_str.push_back((str[i] >= a_LETTER) ? str[i] - a_LETTER : EIGHT_ch - str[i]);

    if(coord_str.size() == COORD_NEED_TO_MOVE)
    {
      Coord from(coord_str[FROM_X], coord_str[FROM_Y]);
      Coord to(coord_str[TO_X], coord_str[TO_Y]);

      move(from, to);
      coord_str.clear();
    }
  }
}

const std::string Board::get_board_mask() const
{
  std::cout<<"====get_board_mask()"<<std::endl;
  std::string mask;
  for(int x = 0; x < BOARD_SIDE_SIZE; ++x)
    for(int y = 0; y < BOARD_SIDE_SIZE; ++y)
      mask.push_back(char(get_figure(x,y)));

  return mask;
}

const std::string Board::get_moves_history() const
{
  std::cout<<"====get_moves_history: "<<std::endl;

  std::string history;
  for(unsigned i = 0; i < m_history_copy.size(); ++i)
  {
    history.push_back(m_history_copy[i].from.x + a_LETTER);
    history.push_back(EIGHT_ch - m_history_copy[i].from.y);
    history.push_back('-');
    history.push_back(m_history_copy[i].to.x + a_LETTER);
    history.push_back(EIGHT_ch - m_history_copy[i].to.y);
  }
  return history;
}

void Board::write_moves_to_file(const std::string &path) const
{
  std::cout<<"====write_moves_to_file "<<std::endl;
  std::ofstream in_file(path);
  const std::string history = get_moves_history();
  for(unsigned i = 0; i < history.size(); ++i)
  {
    in_file<<history[i];
    in_file<<FREE_SPACE;
  }
  in_file.close();
}

void Board::read_moves_from_file(const std::string &path)
{
  std::cout<<"====read_moves_from_file "<<std::endl;
  std::ifstream from_file(path);
  std::ostringstream out;
  out<<from_file.rdbuf();

  std::string data_from_file;
  out.str().swap(data_from_file);
  from_file.close();

  std::cout<<"data_from_file: "<<data_from_file<<std::endl;
  start_new_game();
  make_moves_from_str(data_from_file);
}

void Board::start_new_game()
{
  while(get_actual_move() > 1)
    back_move();
  m_history_copy.clear();
}

void Board::back_move()
{
  if(get_actual_move() <= 1)
    return;

  Moves *const move  = &m_moves[get_last_made_move()];
  set_field(move->from, move->to);
  set_field(move->to, move->fig_on_captured_field);
  if_castling(move->to, move->from);
  m_moves.pop_back();
}

void Board::next_move(const Coord &from, const Coord &to)
{
  m_actual_move.from = from;
  m_actual_move.to = to;
  m_moves.push_back(m_actual_move);

  if(get_actual_move() > 1 && !m_is_go_to_history_in_progress)
  {
    if(get_actual_move() < m_history_copy.size())
      m_history_copy.erase(m_history_copy.begin() + get_last_made_move(), m_history_copy.end());

    m_history_copy.push_back(m_moves[get_last_made_move()]);
  }
}

unsigned Board::get_actual_move() const
{
  return m_moves.size();
}

unsigned Board::get_last_made_move() const
{
  return get_actual_move() - 1;
}

Board::FIGURES Board::get_figure(const Coord &c) const
{
  return FIGURES(m_field[get_field_index(c)]);
}

Board::FIGURES Board::get_figure(const unsigned x, const unsigned y) const
{
  return FIGURES(m_field[y * BOARD_SIZE + x]);
}

Board::COLORLESS_FIG Board::get_colorless_figure(const Coord &c) const
{
  return COLORLESS_FIG(tolower(m_field[get_field_index(c)]) + toupper(m_field[get_field_index(c)]));
}

Board::COLOR Board::get_move_color() const
{
  return get_actual_move() % 2 ? W_FIG : B_FIG;
}

void Board::set_field(const Coord &lhs, const Coord &rhs)
{
  m_field[get_field_index(lhs)] = m_field[get_field_index(rhs)];
}

void Board::set_field(const Coord &c, const FIGURES &fig)
{
  m_field[get_field_index(c)] = fig;
}

Board::COLOR Board::get_color(const Coord &c) const
{
  if(m_field[get_field_index(c)] == FREE_FIELD)
    return NONE;

  return islower(m_field[get_field_index(c)]) ? W_FIG : B_FIG;
}

unsigned Board::get_field_index(const Coord &c) const
{
  return c.y * BOARD_SIZE + c.x;
}

Board::Coord::Coord(const unsigned X, const unsigned Y)
{
  x = X;
  y = Y;
}

bool Board::Coord::operator==(const Coord &rhs)
{
  return(x == rhs.x && y == rhs.y);
}
