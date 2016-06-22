#include<cstdlib>
#include<cctype>
#include<fstream>
#include<math.h>
#include"headers/chess.h"

Board::Board()
{
  for(int i = 0; i < BOARD_SIZE; ++i)
    for(int j = 0; j < BOARD_SIZE; ++j)
    {
      m_field[i][j] = FREE_FIELD;
      if(j == 1) m_field[i][j] = B_PAWN;
      if(j == 6) m_field[i][j] = W_PAWN;
    }

  m_field[0][0] = B_ROOK;
  m_field[7][0] = B_ROOK;
  m_field[1][0] = B_HORSE;
  m_field[6][0] = B_HORSE;
  m_field[2][0] = B_ELEPHANT;
  m_field[5][0] = B_ELEPHANT;
  m_field[3][0] = B_QUEEN;
  m_field[4][0] = B_KING;

  m_field[0][7] = W_ROOK;
  m_field[7][7] = W_ROOK;
  m_field[1][7] = W_HORSE;
  m_field[6][7] = W_HORSE;
  m_field[2][7] = W_ELEPHANT;
  m_field[5][7] = W_ELEPHANT;
  m_field[3][7] = W_QUEEN;
  m_field[4][7] = W_KING;

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
  if((get_act_move() == 1 && get_color(coord) == B_FIG)
     || (get_act_move() > 1 && get_color(coord) != get_move_color()))
   return false;

  return true;
}

void Board::field_change(const Coord& fr, const Coord& t)
{
  m_moves[get_act_move()].fig_on_captured_field = get_figure(t);
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

    for(unsigned i = 0; i < get_act_move(); ++i)
      if(is_check(get_color(f)) || ((get_colorless_figure(m_moves[i].from) == KING
         && get_color(m_moves[i].from) == get_color(f)) || coord == m_moves[i].from || coord == m_moves[i].to))
         goto cant_castling;

    return true;
  }
  return false;
  cant_castling:
    return false;
}

bool Board::is_check(const enum COLOR& color) const
{
  if(get_act_move() < 2 || color == NONE)
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

bool Board::is_mate(const enum COLOR& color)
{
  if(get_act_move() < 2 || color == NONE)
    return false;

  Coord f;
  Coord t;
  for(f.x = 0; f.x < BOARD_SIZE; ++f.x)
    for(f.y = 0; f.y < BOARD_SIZE; ++f.y)
      if(get_figure(f) != FREE_FIELD && get_color(f) == color)
      {
        for(t.x = 0; t.x < BOARD_SIZE; ++t.x)
          for(t.y = 0; t.y < BOARD_SIZE; ++t.y)
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
  m_history_copy.shrink_to_fit();
  for(unsigned i = get_act_move(); i < m_history_copy.size(); ++i)
    m_history_copy.pop_back();

  Fool_move_coord copy;
  copy.from = coord_from;
  copy.to = coord_to;
  m_history_copy.push_back(copy);
}

void Board::go_to_history_index(const unsigned index)
{
  m_is_go_to_history_in_progress = true;

  if(index < get_act_move())
    for(unsigned i = 0; i < get_last_made_move() - index; ++i)
      back_move();

  else if(index > get_last_made_move() && index < m_history_copy.size())
    for(unsigned i = get_last_made_move(); i <= index; ++i)
      move(m_history_copy[i].from, m_history_copy[i].to);

  m_is_go_to_history_in_progress = false;
}

void Board::make_moves_from_str(const std::string& str)
{
  std::cout<<"==make_move_from_str CHESS: "<<str<<std::endl;
  enum{FROM_X = 0, FROM_Y = 1, TO_X = 2, TO_Y = 3, COORD_NEED_TO_MOVE = 4};

  std::vector<int> coord_str;
  for(unsigned i = 0; i < str.size(); ++i)
  {
    if(!(str[i] >= a_LETTER && str[i] <= h_LETTER) || (str[i] >= ONE_ch && str[i] <= EIGHT_ch))
      continue;

    coord_str.push_back((str[i] >= a_LETTER) ? str[i] - a_LETTER : str[i] - ONE_ch);

    if(coord_str.size() == COORD_NEED_TO_MOVE)
    {
      Fool_move_coord coord;
      coord.from.x = coord_str[FROM_X];
      coord.from.y = coord_str[FROM_Y];
      coord.to.x = coord_str[TO_X];
      coord.to.y = coord_str[TO_Y];

      move(coord.from, coord.to);
      coord_str.clear();
    }
  }
}

const std::string Board::get_board_mask() const
{
  std::string mask;
  for(int x = 0; x < BOARD_SIZE; ++x)
    for(int y = 0; y < BOARD_SIZE; ++y)
      mask.push_back(char(get_figure(x,y)));

  return mask;
}

const std::string Board::get_moves_history() const
{
  std::string history;
  for(unsigned i = 0; i < m_history_copy.size(); ++i)
  {
    history.push_back(m_history_copy[i].from.x + a_LETTER);
    history.push_back(m_history_copy[i].from.y + ONE_ch);
    history.push_back('-');
    history.push_back(m_history_copy[i].to.x + a_LETTER);
    history.push_back(m_history_copy[i].to.y + ONE_ch);
  }
  return history;
}

void Board::start_new_game()
{
  std::cout<<"====start_new_game chess"<<std::endl;
  while(!m_moves.empty())
    back_move();
}

void Board::back_move()
{
  if(get_act_move() < 1)
    return;

  Moves *curr_move  = &m_moves[get_last_made_move()];
  set_field(curr_move->from, curr_move->to);
  set_field(curr_move->to, curr_move->fig_on_captured_field);
  if_castling(curr_move->to, curr_move->from);
  m_moves.pop_back();
}

void Board::next_move(const Coord& fr, const Coord& t)
{
  if(!m_moves.empty() && !m_is_go_to_history_in_progress)
    add_move_to_history_copy(fr, t);

  Moves m;
  m.from = fr;
  m.to = t;

  m_moves.push_back(m);
}

bool Board::Coord::operator==(const Coord& rhs)
{
  return(x == rhs.x && y == rhs.y);
}

unsigned Board::get_act_move() const
{
  return m_moves.size();
}

unsigned Board::get_last_made_move() const
{
  return get_act_move() - 1;
}

Board::FIGURES Board::get_figure(const Coord& c) const
{
  return FIGURES(m_field[c.x][c.y]);
}

Board::FIGURES Board::get_figure(const int x, const int y) const
{
  return FIGURES(m_field[x][y]);
}

Board::COLOR Board::get_move_color() const
{
  return get_act_move() % 2 ? B_FIG : W_FIG;
}

const Board::Coord& Board::get_prev_from_coord() const
{
  return m_moves[get_last_made_move() - 1].from;
}

const Board::Coord& Board::get_prev_to_coord() const
{
  return m_moves[get_last_made_move() - 1].to;
}

Board::COLORLESS_FIG Board::get_colorless_figure(const Coord& c) const
{
  return COLORLESS_FIG(tolower(m_field[c.x][c.y]) + toupper(m_field[c.x][c.y]));
}

void Board::set_field(const Coord& lhs, const Coord& rhs)
{
  m_field[lhs.x][lhs.y] = m_field[rhs.x][rhs.y];
}

void Board::set_field(const Coord& c, const enum FIGURES& fig)
{
  m_field[c.x][c.y] = fig;
}

Board::COLOR Board::get_color(const Coord& c) const
{
  if(m_field[c.x][c.y] == FREE_FIELD)
    return NONE;

  return (m_field[c.x][c.y] > a_LETTER) ? W_FIG : B_FIG;
}
