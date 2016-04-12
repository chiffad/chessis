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

  moves[_move_num]._w_king_m = false;
  moves[_move_num]._b_king_m = false;
  moves[_move_num]._w_l_rook_m = false;
  moves[_move_num]._w_r_rook_m = false;
  moves[_move_num]._b_l_rook_m = false;
  moves[_move_num]._b_r_rook_m = false;
  next_move();
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

COLOR Board::get_index_move_color_from_end(const unsigned index) const
{
  return moves[_move_num - index]._color;
}

Board::Coord const& Board::get_history_from_coord() const
{
  return moves[_move_num]._history_from;
}

Board::Coord const& Board::get_history_to_coord() const
{
  return moves[_move_num]._history_to;
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
  if(_move_num > 1 && get_color(_f) == get_index_move_color_from_end(1))
    return false;
  return true;
}                                                                                                  

void Board::field_change()
{
  Moves* this_move = &moves[_move_num];
  this_move->_fig_on_captured_field = _field[_t.x][_t.y];
  FIGURES fig = _field[_f.x][_f.y];
  _field[_f.x][_f.y] = FREE_FIELD;
  _field[_t.x][_t.y] = fig;
  
  if(fig == B_KING) {this_move->_b_king_m = true;}
  else if(fig == W_ROOK) 
  {
    if(_f.x == 0) this_move->_w_l_rook_m = true;
    else if(_f.x == 7) this_move->_w_r_rook_m = true;
  }
  else if(fig == B_ROOK) 
  {
    if(_f.x == 0) this_move->_b_l_rook_m = true;
    else if(_f.x == 7) this_move->_b_r_rook_m = true;
  }
}

COLOR Board::get_color(Coord const& c) const
{
  if(_field[c.x][c.y] > W_FIG)
    return W_FIG;
  return B_FIG;
}

int Board::get_field(Coord const& c) const
{
  return (tolower(_field[c.x][c.y]) + toupper(_field[c.x][c.y]));
}

bool Board::step_ver(Coord const& f, Coord const& t) const
{
  const int dx = t.x - f.x;
  const int dy = t.y - f.y;
  
  if((get_field(f) == ROOK || get_field(f) == QUEEN) && (dx*dy == 0));
  else if((get_field(f) == ELEPHANT || get_field(f) == QUEEN) && (abs(dx) == abs(dy)));
  else if(get_field(f) == HORSE && abs(dx*dy) == 2);
  else if(get_field(f) == KING && ((abs(dx) <= 1 && abs(dy) <= 1) || (dy == 0 && abs(dx) == 2)));
  else if(_field[f.x][f.y] == B_PAWN && dy <= 2 && abs(dx) <= 1);
  else if(_field[f.x][f.y] == W_PAWN && dy >= -2 && abs(dx) <= 1);
  else return false;
  return step_ver_2(f, t);
}

bool Board::step_ver_2(Coord const& f, Coord const& t) const
{
  const int dx = t.x - f.x;
  const int dy = t.y - f.y;

  if(get_field(f) == PAWN)
  {
    int direction = dy/ abs(dy);
    if(_field[t.x][t.y] != FREE_FIELD && abs(dx) == 1 && 
      get_color(f) != get_color(t)) return true;
    if(_field[t.x][t.y] == FREE_FIELD && abs(dx) == 0 && (abs(dy) == 1
    || (abs(dy) == 2 && _field[t.x][f.y + direction] == FREE_FIELD && (f.y == 6 || f.y == 1))))
      return true;
    else return false;
  }

  if(get_field(f) == KING && abs(dx) == 2 && dy == 0)
  {
    if(is_king_and_rook_not_moved(get_color(f), true))
    {
      if(dx == 2 && _field[f.x + 1][f.y] == FREE_FIELD && _field[f.x + 2][f.y] == FREE_FIELD)
       return true;
      if(dx == -2 &&_field[f.x - 1][f.y] == FREE_FIELD && _field[f.x - 2][f.y] == FREE_FIELD
         && _field[f.x - 3][f.y] == FREE_FIELD)
        return true;
    }
    return false;
  }

  if(get_field(f) == HORSE && (_field[t.x][t.y] == FREE_FIELD ||
    get_color(f) != get_color(t))) return true; 
  else if(get_field(f) == HORSE) return false;
  
  int x_s = f.x;
  int y_s = f.y;

  while(x_s != t.x || y_s != t.y)
  {
    if(f.x != t.x && f.y != t.y)
    {
      if(x_s < t.x) ++x_s; 
      else --x_s;
      
      if(y_s < t.y) ++y_s; 
      else --y_s;
    }
    else if(f.x == t.x)
    {
      if(y_s < t.y) ++y_s; 
      else --y_s;
    }
    else if(f.y == t.y)
    {
      if(x_s < t.x) ++x_s; 
      else --x_s;
    }
    if(_field[x_s][y_s] == FREE_FIELD || (x_s == t.x && y_s == t.y && 
      get_color(t) != get_color(f)))
    continue;
     
    return false;
  }
  return true;
}

bool Board::is_king_and_rook_not_moved(COLOR color, bool on_left_side) const
{
  const Moves* this_move = &moves[_move_num];
  if(color == W_FIG && !this_move->_w_king_m)
  {
    if(on_left_side && !this_move->_w_l_rook_m)
      return true;

    if(!on_left_side && !this_move->_w_r_rook_m)
      return true;
  }

  else if(color == B_FIG && !this_move->_b_king_m)
  {
    if(on_left_side && !this_move->_b_l_rook_m)
      return true;

    if(!on_left_side && !this_move->_b_r_rook_m)
      return true;
  }
  return false;
}

bool Board::is_check(COLOR color) const
{
  if(_move_num < 2) return false;

  Coord f;
  Coord t;
  bool m_is_check = false;

  for(t.x = 0; t.x < X_SIZE; ++t.x)
    for(t.y = 0; t.y < Y_SIZE; ++t.y)
      if(get_field(t) == KING && get_color(t) == color)
      {
        for(f.x = 0; f.x < X_SIZE; ++f.x)
          for(f.y = 0; f.y < Y_SIZE; ++f.y)
            if(get_color(f) != color)
            {
              if(step_ver(f, t))
              {
                m_is_check = true;
                goto check;
              }
            }
        goto check;
      }

  check:
  return m_is_check;
}

bool Board::is_mate(COLOR color)//mate to who/color
{
  if(_move_num < 2) return false;

  Coord f;
  Coord t;
  bool m_is_mate = true;

  for(f.x = 0; f.x < X_SIZE; ++f.x)
    for(f.y = 0; f.y < Y_SIZE; ++f.y)
      if(_field[f.x][f.y] != FREE_FIELD && get_color(f) == color)
      {
        for(t.x = 0; t.x < X_SIZE; ++t.x)
          for(t.y = 0; t.y < Y_SIZE; ++t.y)
            if(step_ver(f, t)) 
            {
              FIGURES fig_from = get_figure(f);
              FIGURES fig_to   = get_figure(t);
              _field[f.x][f.y] = FREE_FIELD;
              _field[t.x][t.y] = fig_from;
              if(!is_check(color))
              {
                _field[f.x][f.y] = fig_from;
                _field[t.x][t.y] = fig_to;
                m_is_mate = false;
                goto not_mate;
              }
              else
              {
                _field[f.x][f.y] = fig_from;
                _field[t.x][t.y] = fig_to;
              }
            }
      }

  return m_is_mate;

  not_mate:
  return m_is_mate;
}

bool Board::back_move()
{
  if(_move_num > 1)
  {
    Moves* prev_move = &moves[_move_num - 1];

    moves[_move_num]._color = get_color(prev_move->_history_from);
    _field[prev_move->_history_from.x][prev_move->_history_from.y] = _field[prev_move->_history_to.x][prev_move->_history_to.y];
    _field[prev_move->_history_to.x][prev_move->_history_to.y] = prev_move->_fig_on_captured_field;
    --_move_num;
    return true;
  }
  return false;
}

void Board::next_move()
{
  Moves* prev_move = &moves[_move_num - 1];
  Moves* this_move = &moves[_move_num];

  if(_move_num > 0) this_move->_color = get_color(_t);
  this_move->_history_from = _f;
  this_move->_history_to   = _t;
  moves.push_back(moves[_move_num]);
  ++_move_num;
  this_move->_w_king_m = prev_move->_w_king_m;
  this_move->_b_king_m = prev_move->_b_king_m;
  this_move->_w_l_rook_m = prev_move->_w_l_rook_m;
  this_move->_w_r_rook_m = prev_move->_w_r_rook_m;
  this_move->_b_l_rook_m = prev_move->_b_l_rook_m;
  this_move->_b_r_rook_m = prev_move->_b_r_rook_m;
}
