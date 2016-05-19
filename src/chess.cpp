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

bool Board::move(const Coord& fr, const Coord& t)
{
  if(right_move_turn(fr) && step_ver(fr, t)) field_change(fr, t);
  else return false;

  if(!is_check(get_color(t)))
  {
    if_castling(fr,t);
    next_move(fr, t);
    return true;
  }
  field_change(t, fr);
  return false;
}

void Board::if_castling(const Coord& fr, const Coord& t)
{
  const int dx = abs(fr.x - t.x);
  const int X_UNIT_VEC = dx == 0 ? 0 : (t.x - fr.x)/dx;
  if(get_colorless_figure(t) == KING && dx > 1)
  {
    Coord rook_fr;
    Coord rook_to;

    rook_fr.x = X_UNIT_VEC > 0 ? t.x - X_UNIT_VEC : t.x + 2 * X_UNIT_VEC;
    rook_to.x = t.x + X_UNIT_VEC;
    rook_fr.y = rook_to.y = t.y;

    FIGURES fig = get_figure(rook_fr);
    set_field(rook_fr, FREE_FIELD);
    set_field(rook_to, fig);
  }
}

bool Board::right_move_turn(const Coord& coord) const
{
  if(_move_num == 1 && get_color(coord) == B_FIG) return false;
  if(_move_num > 1 && get_color(coord) == get_move_color_i_from_end(1))
    return false;
  return true;
}

void Board::field_change(const Coord& fr, const Coord& t)
{
  moves[_move_num]._fig_on_captured_field = get_figure(t);
  FIGURES fig = get_figure(fr);
  set_field(fr, FREE_FIELD);
  set_field(t, fig);
}

bool Board::step_ver(const Coord& f, const Coord& t) const
{
  const int dx = abs(t.x - f.x);
  const int dy = abs(t.y - f.y);

  const int X_UNIT_VECTOR = dx == 0 ? 0 : (t.x - f.x)/dx;
  const int Y_UNIT_VECTOR = dy == 0 ? 0 : (t.y - f.y)/dy;
  
  if(get_colorless_figure(f) == HORSE && dx*dy == 2)
  {
    if(get_figure(t) == FREE_FIELD || get_color(f) != get_color(t))
      return true;
    else return false;
  }

  else if(get_colorless_figure(f) == KING && ((dx <= 1 && dy <= 1) || (dy == 0 && dx == 2)))
  {
    if(is_can_castling(get_color(f), t.x) && dx == 2)
    {
      if(get_figure(f.x + X_UNIT_VECTOR, f.y) == FREE_FIELD && get_figure(f.x + 2 * X_UNIT_VECTOR,f.y) == FREE_FIELD)
      {
        if(X_UNIT_VECTOR > 0 || (X_UNIT_VECTOR < 0 && get_figure(f.x - 3, f.y) == FREE_FIELD))
          return true;
        else return false;
      }
      return false;
    }
    else if(dx == 2) return false;
  }

  else if(get_colorless_figure(f) == PAWN && dy <= 2 && dx <= 1)
  {
    if(get_figure(t) != FREE_FIELD && dx == 1 && get_color(f) != get_color(t))
      return true;
    if(get_figure(t) == FREE_FIELD && dx == 0 && (dy == 1
    ||(dy == 2 && get_figure(t.x,f.y + Y_UNIT_VECTOR) == FREE_FIELD
    &&((f.y == 6 && get_color(f) == W_FIG) || (f.y == 1 && get_color(f) == B_FIG)))))
      return true;
    return false;
  }
  else if((get_colorless_figure(f) == ROOK || get_colorless_figure(f) == QUEEN) && (dx*dy == 0));
  else if((get_colorless_figure(f) == ELEPHANT || get_colorless_figure(f) == QUEEN) && (dx == dy));
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
    if(is_check(color) && ((get_colorless_figure(moves[i]._from) == KING && get_color(moves[i]._from) == color)
      || coord == moves[i]._from || coord == moves[i]._to))
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
    Coord& prev_from = moves[_move_num - 1]._from;
    Coord& prev_to = moves[_move_num - 1]._to;

    moves[_move_num]._color = get_color(prev_from);
    set_field(prev_from, prev_to);
    set_field(prev_to, moves[_move_num - 1]._fig_on_captured_field);
    --_move_num;
    if_castling(prev_from, prev_to);
    return true;
  }
  return false;
}

void Board::next_move(const Coord& fr, const Coord& t)
{
  Moves* this_move = &moves[_move_num];

  if(_move_num > 0) this_move->_color = get_color(t);
  this_move->_from = fr;
  this_move->_to   = t;
  moves.push_back(moves[_move_num]);
  ++_move_num;
}

inline bool Board::Coord::operator ==(const Coord& rhs)
{
  return(x == rhs.x && y == rhs.y);
}

unsigned Board::get_current_move() const
{
  return _move_num;
}

FIGURES Board::get_figure(const Coord& c) const
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

const Board::Coord& Board::get_i_from_coord_from_end(const unsigned i) const
{
  return moves[_move_num - i]._from;
}

const Board::Coord& Board::get_i_to_coord_from_end(const unsigned i) const
{
  return moves[_move_num - i]._to;
}

int Board::get_colorless_figure(const Coord& c) const
{
  return (tolower(_field[c.x][c.y]) + toupper(_field[c.x][c.y]));
}

void Board::set_field(const Coord& lhs, const Coord& rhs)
{
  _field[lhs.x][lhs.y] = _field[rhs.x][rhs.y];
}

void Board::set_field(const Coord& rhs, FIGURES fig)
{
  _field[rhs.x][rhs.y] = fig;
}

COLOR Board::get_color(const Coord& c) const
{
  if(_field[c.x][c.y] > W_FIG)
    return W_FIG;
  return B_FIG;
}
