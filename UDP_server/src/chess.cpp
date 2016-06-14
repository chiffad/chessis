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
  if(is_right_move_turn(fr) && (is_can_move(fr, t) || is_castling(fr, t)))
    field_change(fr, t);
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

bool Board::is_right_move_turn(const Coord& coord) const
{
  if(_move_num == 1 && get_color(coord) == B_FIG) return false;
  if(_move_num > 1 && get_color(coord) != get_move_color())
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

bool Board::is_can_move(const Coord& f, const Coord& t) const
{
  const int dx = abs(t.x - f.x);
  const int dy = abs(t.y - f.y);

  const int X_UNIT_VECTOR = dx == 0 ? 0 : (t.x - f.x)/dx;
  const int Y_UNIT_VECTOR = dy == 0 ? 0 : (t.y - f.y)/dy;

  if(get_colorless_figure(f) == HORSE && dx*dy == 2 && get_color(f) != get_color(t))
    return true;
  else if(get_colorless_figure(f) == PAWN && ((Y_UNIT_VECTOR < 0 && get_color(f) == W_FIG)
                                              || (Y_UNIT_VECTOR > 0 && get_color(f) == B_FIG)))
  {
      return ((get_figure(t) != FREE_FIELD && dy * dx == 1 && get_color(f) != get_color(t))
              || (dx == 0 && ((dy == 1 && get_figure(t) == FREE_FIELD)
                  || (dy == 2 && get_figure(t.x,f.y + Y_UNIT_VECTOR) == FREE_FIELD && (f.y == 6 || f.y == 1)))));
  }
  else if(get_colorless_figure(f) == KING && dx <= 1 && dy <= 1);
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
    if(get_figure(coord) != FREE_FIELD && get_color(t) != get_color(f) && !(coord == t))
      goto failed;
  }
  return true;
  failed:
    return false;
}

void Board::if_castling(const Coord& fr, const Coord& t)
{
  const int dx = abs(t.x - fr.x);
  const int X_UNIT_VEC = dx == 0 ? 0 : (t.x - fr.x)/dx;

  if(get_colorless_figure(t) == KING && dx > 1)
  {
    Coord rook_fr;
    Coord rook_to;

    rook_fr.y = rook_to.y = t.y;
    if(fr.x == 6 || fr.x == 2)
    {
      rook_fr.x = fr.x + X_UNIT_VEC;
      rook_to.x = fr.x - X_UNIT_VEC * (X_UNIT_VEC > 0 ? 2 : 1);
    }
    else
    {
      rook_fr.x = t.x + X_UNIT_VEC * (X_UNIT_VEC > 0 ? 1 : 2);
      rook_to.x = t.x - X_UNIT_VEC;
    }
    FIGURES fig = get_figure(rook_fr);
    set_field(rook_fr, FREE_FIELD);
    set_field(rook_to, fig);
  }
}

bool Board::is_castling(const Coord& f, const Coord& t) const
{
  const int dx = abs(t.x - f.x);
  const int X_UNIT_VECTOR = dx == 0 ? 0 : (t.x - f.x)/dx;

  if(get_colorless_figure(f) != KING || abs(t.y - f.y) != 0 || dx != 2);

  else if((get_figure(f.x + X_UNIT_VECTOR, f.y) == FREE_FIELD && get_figure(f.x + 2 * X_UNIT_VECTOR,f.y) == FREE_FIELD)
     && (X_UNIT_VECTOR > 0 || get_figure(f.x + 3 * X_UNIT_VECTOR, f.y) == FREE_FIELD))
  {
    const bool IS_TWO_ZEROS = t.x > 4;
    Coord coord;
    coord.x = IS_TWO_ZEROS ? 7 : 0;
    coord.y = get_color(f) == W_FIG ? 7 : 0;

    for(unsigned i = 0; i < get_current_move(); ++i)
      if(is_check(get_color(f)) || ((get_colorless_figure(moves[i]._from) == KING
         && get_color(moves[i]._from) == get_color(f)) || coord == moves[i]._from || coord == moves[i]._to))
         goto cant_castling;

    return true;
  }
  return false;
  cant_castling:
    return false;
}

bool Board::is_check(COLOR color) const
{
  if(_move_num < 2 || color == NONE) return false;

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

bool Board::is_mate(COLOR color)
{
  if(_move_num < 2 || color == NONE) return false;
  Coord f;
  Coord t;

  for(f.x = 0; f.x < X_SIZE; ++f.x)
    for(f.y = 0; f.y < Y_SIZE; ++f.y)
      if(get_figure(f) != FREE_FIELD && get_color(f) == color)
      {
        for(t.x = 0; t.x < X_SIZE; ++t.x)
          for(t.y = 0; t.y < Y_SIZE; ++t.y)
            if(is_can_move(f, t))
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

void Board::add_move_to_history_copy(const Coord& coord_from, const Coord& coord_to)
{
  history_copy.shrink_to_fit();
  for(unsigned i = get_current_move(); i < history_copy.size(); ++i)
    history_copy.pop_back();
  Fool_move_coord copy;
  copy._from = coord_from;
  copy._to = coord_to;
  history_copy.push_back(copy);
}

void Board::go_to_history_index(const unsigned index)
{
  _is_go_to_history_in_progress = true;
  const unsigned CURRENT_MOVE = get_current_move() - 1;

  if(index < get_current_move())
    for(unsigned i = 0; i < CURRENT_MOVE - index; ++i)
      back_move();

  else if(index > CURRENT_MOVE && index < history_copy.size())
    for(unsigned i = CURRENT_MOVE; i <= index; ++i)
      move(history_copy[i]._from, history_copy[i]._to);

  _is_go_to_history_in_progress = false;
}

void Board::make_moves_from_str(const std::string& str)
{
  std::cout<<"==make_move_from_str CHESS: "<<str<<std::endl;
  enum{FROM_X = 0, FROM_Y = 1, TO_X = 2, TO_Y = 3, COORD_NEED_TO_MOVE = 4};
  const char a_LETTER = 'a';
  const char h_LETTER = 'h';
  const char ONE_ch = '1';
  const char EIGHT_ch = '8';

  std::vector<int> coord_str;
  for(unsigned i = 0; i < str.size(); ++i)
  {
    if(!(str[i] >= a_LETTER && str[i] <= h_LETTER) || (str[i] >= ONE_ch && str[i] <= EIGHT_ch))
      continue;

    coord_str.push_back((str[i] >= a_LETTER) ? str[i] - a_LETTER : str[i] - ONE_ch);

    if(coord_str.size() == COORD_NEED_TO_MOVE)
    {
      Fool_move_coord coord;
      coord._from.x = coord_str[FROM_X];
      coord._from.y = coord_str[FROM_Y];
      coord._to.x = coord_str[TO_X];
      coord._to.y = coord_str[TO_Y];

      move(coord._from, coord._to);
      coord_str.clear();
    }
  }
}

void Board::start_new_game()
{
  std::cout<<"====start_new_game chess"<<std::endl;
  while(get_current_move() != 1)
    back_move();
}

void Board::back_move()
{
  if(_move_num > 1)
    return;

  const int I = _move_num - 1;
  moves[_move_num]._color = get_color(moves[I]._from);
  set_field(moves[I]._from, moves[I]._to);
  set_field(moves[I]._to, moves[_move_num - 1]._fig_on_captured_field);
  if_castling(moves[I]._to, moves[I]._from);
  --_move_num;
}

void Board::next_move(const Coord& fr, const Coord& t)
{
  Moves* this_move = &moves[_move_num];

  if(_move_num > 0)
  {
    this_move->_color = get_color(t);

    if(!_is_go_to_history_in_progress)
      add_move_to_history_copy(fr, t);
  }
  this_move->_from = fr;
  this_move->_to   = t;
  moves.push_back(moves[_move_num]);
  ++_move_num;
}

bool Board::Coord::operator ==(const Coord& rhs)
{
  return(x == rhs.x && y == rhs.y);
}

unsigned Board::get_current_move() const
{
  return _move_num;
}

Board::FIGURES Board::get_figure(const Coord& c) const
{
  return FIGURES(_field[c.x][c.y]);
}

Board::FIGURES Board::get_figure(const int x, const int y) const
{
  return FIGURES(_field[x][y]);
}

Board::COLOR Board::get_move_color() const
{
  return (moves[_move_num - 1]._color == W_FIG) ? B_FIG : W_FIG;
}

const Board::Coord& Board::get_prev_from_coord() const
{
  return moves[_move_num - 1]._from;
}

const Board::Coord& Board::get_prev_to_coord() const
{
  return moves[_move_num - 1]._to;
}

Board::COLORLESS_FIG Board::get_colorless_figure(const Coord& c) const
{
  return COLORLESS_FIG(tolower(_field[c.x][c.y]) + toupper(_field[c.x][c.y]));
}

void Board::set_field(const Coord& lhs, const Coord& rhs)
{
  _field[lhs.x][lhs.y] = _field[rhs.x][rhs.y];
}

void Board::set_field(const Coord& coord, FIGURES fig)
{
  _field[coord.x][coord.y] = fig;
}

Board::COLOR Board::get_color(const Coord& c) const
{
  if(_field[c.x][c.y] == FREE_FIELD)
    return NONE;

  if(_field[c.x][c.y] > W_FIG)
    return W_FIG;
  return B_FIG;
}
