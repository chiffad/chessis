#include<cstdlib>
#include<cctype>
#include<fstream>
#include<math.h>
#include"headers/chess.h"

Board::Board()
{
  moves.push_back(m);
  _move_num = 0;  
  moves[_move_num]._color = NONE;
  
  for(int i = 0; i < X_SIZE; ++i)
    for(int j = 0; j < Y_SIZE; ++j)
    {
      _field[i][j] = FREE_FIELD;
      if(j == 1) _field[i][j] = B_PAWN;
      if(j == 6) _field[i][j] = W_PAWN;
    }
  _field[0][0] = B_ROOK;
  _field[7][0] = B_ROOK;
  _field[1][0] = B_HORSE;
  _field[6][0] = B_HORSE;
  _field[2][0] = B_ELEPHANT;
  _field[5][0] = B_ELEPHANT;
  _field[3][0] = B_QUEEN;
  _field[4][0] = B_KING;

  _field[0][7] = W_ROOK;
  _field[7][7] = W_ROOK;
  _field[1][7] = W_HORSE;
  _field[6][7] = W_HORSE;
  _field[2][7] = W_ELEPHANT;
  _field[5][7] = W_ELEPHANT;
  _field[3][7] = W_QUEEN;
  _field[4][7] = W_KING;

  next_move();
}

bool Board::move(Coord const& fr, Coord const& t)
{
  _f = fr; _t = t;
  if(right_move_turn() && step_ver(fr, t)) field_change();
  else return false;

  if(!is_check(get_color(t)))
  {
    next_move();
    return true;
  }
  _f = t; _t = fr;
  field_change();
  return false;
}

bool Board::right_move_turn() const
{
  if(_move_num == 1 && get_color(_f) == B_FIG) return false;
  if(_move_num > 1 && get_color(_f) == get_move_color_i_from_end(1))
    return false;
  return true;
}                                                                                                  

void Board::field_change()
{
  moves[_move_num]._fig_on_captured_field = get_figure(_t);
  FIGURES fig = get_figure(_f);
  set_field(_f, FREE_FIELD);
  set_field(_t, fig);
}

bool Board::step_ver(Coord const& f, Coord const& t) const
{
  const int dx = t.x - f.x;
  const int dy = t.y - f.y;

  const int X_UNIT_VECTOR = dx == 0 ? 0 : dx/abs(dx);
  const int Y_UNIT_VECTOR = dy == 0 ? 0 : dy/abs(dy);
  
  if(get_colorless_figure(f) == HORSE && abs(dx*dy) == 2)
  {
    if(get_figure(t) == FREE_FIELD || get_color(f) != get_color(t))
      return true;
    else return false;
  }

  else if(get_colorless_figure(f) == KING && ((abs(dx) <= 1 && abs(dy) <= 1) || (dy == 0 && abs(dx) == 2)))
  {
    if(is_can_castling(get_color(f), t.x) && abs(dx) == 2 && dy == 0)
    {
      if(dx == 2 && get_figure(f.x + X_UNIT_VECTOR, f.y) == FREE_FIELD && get_figure(f.x + 2 * X_UNIT_VECTOR,f.y) == FREE_FIELD)
      {
        if(X_UNIT_VECTOR < 0 && get_figure(f.x - 3, f.y) == FREE_FIELD)
            return true;

        else if(X_UNIT_VECTOR > 0) return true;
      }
      return false;
    }
  }

  else if(get_colorless_figure(f) == PAWN && abs(dy) <= 2 && abs(dx) <= 1)
  {
    if(get_figure(t) != FREE_FIELD && abs(dx) == 1 && get_color(f) != get_color(t))
      return true;
    if(get_figure(t) == FREE_FIELD && abs(dx) == 0 && (abs(dy) == 1
    ||(abs(dy) == 2 && get_figure(t.x,f.y + Y_UNIT_VECTOR) == FREE_FIELD && (f.y == 6 || f.y == 1))))
      return true;
    return false;
  }
  else if((get_colorless_figure(f) == ROOK || get_colorless_figure(f) == QUEEN) && (dx*dy == 0));
  else if((get_colorless_figure(f) == ELEPHANT || get_colorless_figure(f) == QUEEN) && (abs(dx) == abs(dy)));
  else return false;

  Coord coord;
  coord.x = f.x;
  coord.y = f.y;

  while(!(coord == t))
  {
    coord.x += X_UNIT_VECTOR;
    coord.y += Y_UNIT_VECTOR;
    if(get_figure(coord) == FREE_FIELD || (get_color(t) != get_color(coord) && coord == t))
      continue;

    return false;
  }
  return true;
}

bool Board::is_can_castling(COLOR color, const int x) const
{
  const bool IS_TWO_ZEROS = x > 4;
  Coord coord;
  coord.x = IS_TWO_ZEROS ? 7 : 0;
  coord.y = (color == W_FIG) ? 7 : 0;

  for(unsigned i = 0; i < get_current_move(); ++i)
    if(get_colorless_figure(moves[i]._from) == KING && get_color(moves[i]._from) == color)
      goto cant_castling;

  for(unsigned i = 0; i < get_current_move(); ++i)
    if(coord == moves[i]._from && get_colorless_figure(coord) != ROOK && get_color(coord) != color)
      goto cant_castling;

  return true;

  cant_castling:
    return false;
}

bool Board::is_check(COLOR color) const
{
  if(_move_num < 2) return false;

  Coord f;
  Coord t;

  for(t.x = 0; t.x < X_SIZE; ++t.x)
    for(t.y = 0; t.y < Y_SIZE; ++t.y)
      if(get_colorless_figure(t) == KING && get_color(t) == color)
      {
        for(f.x = 0; f.x < X_SIZE; ++f.x)
          for(f.y = 0; f.y < Y_SIZE; ++f.y) 
            if(get_color(f) != color)
            {
              if(step_ver(f, t))
                goto check;
            }
        goto not_check;
      }

  not_check:
    return false;

  check:
    return true;
}

bool Board::is_mate(COLOR color)
{
  if(_move_num < 2) return false;
  Coord f;
  Coord t;

  for(f.x = 0; f.x < X_SIZE; ++f.x)
    for(f.y = 0; f.y < Y_SIZE; ++f.y)
      if(get_figure(f) != FREE_FIELD && get_color(f) == color)
      {
        for(t.x = 0; t.x < X_SIZE; ++t.x)
          for(t.y = 0; t.y < Y_SIZE; ++t.y)
            if(step_ver(f, t)) 
            {
              FIGURES fig_from = get_figure(f);
              FIGURES fig_to   = get_figure(t);
              set_field(f,FREE_FIELD);
              set_field(t,fig_from);
              if(!is_check(color))
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

bool Board::back_move()
{
  if(_move_num > 1)
  {
    Coord& m_prev_from = moves[_move_num - 1]._from;
    Coord& m_prev_to = moves[_move_num - 1]._to;

    moves[_move_num]._color = get_color(m_prev_from);
    set_field(m_prev_from, m_prev_to);
    set_field(m_prev_from, m_prev_to);
    set_field(m_prev_to, moves[_move_num - 1]._fig_on_captured_field);
    --_move_num;
    return true;
  }
  return false;
}

void Board::next_move()
{
  Moves* this_move = &moves[_move_num];

  if(_move_num > 0) this_move->_color = get_color(_t);
  this_move->_from = _f;
  this_move->_to   = _t;
  moves.push_back(moves[_move_num]);
  ++_move_num;
}

bool Board::Coord::operator ==(Coord const& rhv)
{
  return(x == rhv.x && y == rhv.y);
}

unsigned Board::get_current_move() const
{
  return _move_num;
}

FIGURES Board::get_figure(Coord const& c) const
{
  return FIGURES(_field[c.x][c.y]);
}

FIGURES Board::get_figure(const int x, const int y) const
{
  return FIGURES(_field[x][y]);
}

COLOR Board::get_move_color_i_from_end(const unsigned i) const
{
  return moves[_move_num - i]._color;
}

Board::Coord const& Board::get_i_from_coord_from_end(const unsigned i) const
{
  return moves[_move_num - i]._from;
}

Board::Coord const& Board::get_i_to_coord_from_end(const unsigned i) const
{
  return moves[_move_num - i]._to;
}

int Board::get_colorless_figure(Coord const& c) const
{
  return (tolower(_field[c.x][c.y]) + toupper(_field[c.x][c.y]));
}

void Board::set_field(Coord const& rhv, Coord const& lhv)
{
  _field[rhv.x][rhv.y] = _field[lhv.x][lhv.y];
}

void Board::set_field(Coord const& rhv, FIGURES fig)
{
  _field[rhv.x][rhv.y] = fig;
}

COLOR Board::get_color(Coord const& c) const
{
  if(_field[c.x][c.y] > W_FIG)
    return W_FIG;
  return B_FIG;
}
