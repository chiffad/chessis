#include "logic/board_logic.hpp"

#include <spdlog/spdlog.h>

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iterator>
#include <sstream>
#include <stdlib.h>
#include <vector>

namespace logic {

const auto BOARD_SIDE = 8;
const auto FIGURES_NUMBER = 32;
const auto BOARD_SIZE = BOARD_SIDE * BOARD_SIDE;

enum COLOR
{
  NONE,
  WHITE,
  BLACK
};
enum FIGURE
{
  B_QUEEN = 'Q',
  B_KING = 'K',
  B_ROOK = 'R',
  B_ELEPHANT = 'E',
  B_HORSE = 'H',
  B_PAWN = 'P',
  W_QUEEN = 'q',
  W_KING = 'k',
  W_ROOK = 'r',
  W_ELEPHANT = 'e',
  W_HORSE = 'h',
  W_PAWN = 'p',
  FREE_FIELD = '.'
};
enum COLORLESS_FIG
{
  NOT_FIGURE = FREE_FIELD,
  QUEEN = B_QUEEN + W_QUEEN,
  KING = W_KING + B_KING,
  ROOK = W_ROOK + B_ROOK,
  PAWN = W_PAWN + B_PAWN,
  ELEPHANT = W_ELEPHANT + B_ELEPHANT,
  HORSE = W_HORSE + B_HORSE
};

enum MOVE_TYPE
{
  USUAL,
  CASTLING,
  EN_PASSANT,
  PAWN_TRANSFORM
};

struct board_logic_t::impl_t
{
  explicit impl_t(const uuid_t& uuid);
  bool move(const coord_t& from, const coord_t& to);
  bool back_move();
  void start_new_game();
  void go_to_history(size_t i);
  bool mate() const;
  std::string get_moves_history() const;
  uint16_t get_move_num() const;
  std::string get_board_mask() const;

  COLOR get_color(const coord_t& c) const;
  COLOR get_move_color() const;
  FIGURE get_figure(const coord_t& c) const;
  FIGURE get_figure(const unsigned x, const unsigned y) const;
  COLORLESS_FIG get_colorless_fig(const coord_t& c) const;
  void en_passant();
  void re_en_passant();
  void pawn_re_transform();
  void pawn_transform(const coord_t& c);
  uint16_t get_move_num_from_0() const;
  unsigned get_field_index(const coord_t& c) const;
  void if_castling(const coord_t& fr, const coord_t& to);
  void finish_move(const coord_t& from, const coord_t& to);
  void next_move(const coord_t& from = coord_t(), const coord_t& to = coord_t());
  void set_field(const coord_t& lhs, const coord_t& rhs, const FIGURE& fig = FREE_FIELD);
  void test_on_mate();

  bool is_check(const COLOR color) const;
  bool is_pawn_reach_other_side(const coord_t& c) const;
  bool is_castling(const coord_t& from, const coord_t& to) const;
  bool is_can_move(const coord_t& from, const coord_t& to) const;
  bool is_en_passant(const coord_t& fr, const coord_t& to) const;

  int diff(const int first, const int second) const;

  struct Moves
  {
    coord_t from;
    coord_t to;
    FIGURE fig_on_captured_field;
    MOVE_TYPE type;
  } actual_move_;

  std::vector<Moves> moves_;
  std::vector<Moves> moves_copy_;
  std::vector<FIGURE> field_;
  bool go_to_history_running_;
  bool mate_;
  const uuid_t uuid_;
};

board_logic_t::board_logic_t(const uuid_t& uuid)
  : impl_(std::make_unique<impl_t>(uuid))
{}

board_logic_t::~board_logic_t() = default;

bool board_logic_t::move(const coord_t& from, const coord_t& to)
{
  return impl_->move(from, to);
}

bool board_logic_t::back_move()
{
  return impl_->back_move();
}

void board_logic_t::start_new_game()
{
  impl_->start_new_game();
}

void board_logic_t::go_to_history(size_t i)
{
  impl_->go_to_history(i);
}

bool board_logic_t::mate() const
{
  return impl_->mate();
}

std::string board_logic_t::get_moves_history() const
{
  return impl_->get_moves_history();
}

uint16_t board_logic_t::get_move_num() const
{
  return impl_->get_move_num();
}

std::string board_logic_t::get_board_mask() const
{
  return impl_->get_board_mask();
}

board_logic_t::impl_t::impl_t(const uuid_t& uuid)
  : go_to_history_running_(false)
  , mate_(false)
  , uuid_(uuid)
{
  field_.resize(BOARD_SIZE);
  field_ = {B_ROOK, B_HORSE, B_ELEPHANT, B_QUEEN, B_KING, B_ELEPHANT, B_HORSE, B_ROOK};
  field_.insert(field_.end(), BOARD_SIDE, B_PAWN);
  field_.insert(field_.end(), BOARD_SIDE * 4, FREE_FIELD);
  field_.insert(field_.end(), BOARD_SIDE, W_PAWN);

  std::vector<FIGURE> eight_row = {W_ROOK, W_HORSE, W_ELEPHANT, W_QUEEN, W_KING, W_ELEPHANT, W_HORSE, W_ROOK};
  field_.insert(field_.end(), eight_row.begin(), eight_row.end());
}

bool board_logic_t::impl_t::move(const coord_t& from, const coord_t& to)
{
  if (from == to)
  {
    return false;
  }

  if ((get_color(from) == get_move_color()) && (is_can_move(from, to) || is_castling(from, to)))
  {
    actual_move_.fig_on_captured_field = get_figure(to);
    set_field(to, from);
    if (!is_check(get_color(to)))
    {
      finish_move(from, to);
      test_on_mate();
      return true;
    }
    else set_field(from, to, actual_move_.fig_on_captured_field);
  }
  return false;
}

void board_logic_t::impl_t::finish_move(const coord_t& from, const coord_t& to)
{
  actual_move_.type = USUAL;
  if (get_colorless_fig(to) == PAWN)
  {
    if (is_en_passant(from, to))
    {
      en_passant();
    }
    else if (is_pawn_reach_other_side(to))
    {
      pawn_transform(to);
    }
  }
  if_castling(from, to);
  next_move(from, to);
}

bool board_logic_t::impl_t::is_can_move(const coord_t& fr, const coord_t& to) const
{
  const int dx = abs(diff(to.x, fr.x));
  const int dy = abs(diff(to.y, fr.y));
  const int X_UNIT_VECTOR = (dx == 0) ? 0 : diff(to.x, fr.x) / dx;
  const int Y_UNIT_VECTOR = (dy == 0) ? 0 : diff(to.y, fr.y) / dy;
  const auto FIG = get_colorless_fig(fr);

  if (FIG == HORSE)
  {
    return (dx * dy == 2 && get_color(fr) != get_color(to));
  }
  if (FIG == PAWN)
  {
    if ((Y_UNIT_VECTOR < 0 && get_color(fr) == WHITE) || (Y_UNIT_VECTOR > 0 && get_color(fr) == BLACK))
    {
      if ((get_figure(to) != FREE_FIELD))
      {
        return (dy * dx == 1 && get_color(fr) != get_color(to));
      }
      else if (dx == 0)
      {
        return (dy == 1 || (dy == 2 && get_figure(to.x, fr.y + Y_UNIT_VECTOR) == FREE_FIELD && (fr.y == 6 || fr.y == 1)));
      }
      else return is_en_passant(fr, to);
    }
    return false;
  }
  else if (FIG == KING && dx <= 1 && dy <= 1)
    ;
  else if ((FIG == ROOK || FIG == QUEEN) && (dx * dy == 0))
    ;
  else if ((FIG == ELEPHANT || FIG == QUEEN) && (dx == dy))
    ;
  else return false;

  coord_t c(fr);
  while (!(c == to))
  {
    c.x += X_UNIT_VECTOR;
    c.y += Y_UNIT_VECTOR;
    if (get_figure(c) == FREE_FIELD || (get_color(to) != get_color(fr) && c == to))
    {
      continue;
    }

    return false;
  }
  return true;
}

bool board_logic_t::impl_t::is_en_passant(const coord_t& fr, const coord_t& to) const
{
  const auto& m = moves_.back();
  bool is_cross = (abs(diff(to.x, fr.x) * diff(to.y, fr.y)) == 1);
  if (is_cross && get_colorless_fig(m.to) == PAWN)
  {
    return (m.to.y == fr.y && m.to.x == to.x && abs(diff(m.to.y, m.from.y)) == 2);
  }
  return false;
}

void board_logic_t::impl_t::en_passant()
{
  actual_move_.type = EN_PASSANT;
  const auto ind = get_field_index(moves_.back().to);
  field_[ind] = FREE_FIELD;
}

bool board_logic_t::impl_t::is_pawn_reach_other_side(const coord_t& c) const
{
  enum
  {
    FIRST_LINE = 0,
    LAST_LINE = 7
  };
  return (c.y == LAST_LINE || c.y == FIRST_LINE);
}

void board_logic_t::impl_t::pawn_transform(const coord_t& c)
{
  actual_move_.type = PAWN_TRANSFORM;
  auto& f = field_[get_field_index(c)];
  if (get_figure(c) == W_PAWN)
  {
    f = W_QUEEN;
  }
  else f = B_QUEEN;
}

void board_logic_t::impl_t::if_castling(const coord_t& fr, const coord_t& to)
{
  const int dx = abs(diff(to.x, fr.x));
  const int X_UNIT_VEC = dx == 0 ? 0 : diff(to.x, fr.x) / dx;

  if (get_colorless_fig(to) == KING && dx > 1)
  {
    coord_t rook_fr, rook_to;

    rook_fr.y = rook_to.y = to.y;
    if (fr.x == 6 || fr.x == 2)
    {
      rook_fr.x = fr.x + X_UNIT_VEC;
      rook_to.x = fr.x - X_UNIT_VEC * (X_UNIT_VEC > 0 ? 2 : 1);
    }
    else
    {
      rook_fr.x = to.x + X_UNIT_VEC * (X_UNIT_VEC > 0 ? 1 : 2);
      rook_to.x = to.x - X_UNIT_VEC;
      actual_move_.type = CASTLING;
    }
    set_field(rook_to, rook_fr);
  }
}

bool board_logic_t::impl_t::is_castling(const coord_t& fr, const coord_t& to) const
{
  const int dx = abs(diff(to.x, fr.x));
  const int dy = abs(diff(to.y, fr.y));
  const int X_UNIT_VECTOR = dx == 0 ? 0 : diff(to.x, fr.x) / dx;

  const auto& field = field_.begin() + get_field_index(fr) + X_UNIT_VECTOR;
  if (get_colorless_fig(fr) == KING && !is_check(get_color(fr)) && dy == 0 && dx == 2 && *field == FREE_FIELD && *(field + X_UNIT_VECTOR) == FREE_FIELD &&
      (X_UNIT_VECTOR > 0 || *(field + 2 * X_UNIT_VECTOR) == FREE_FIELD))
  {
    coord_t c((to.x > 4 ? 7 : 0), (get_color(fr) == WHITE ? 7 : 0));
    const bool is_rook_not_moved = (std::find_if(moves_.begin(), moves_.end(), [c](auto& i) { return (c == i.from || c == i.to); }) == moves_.end());

    const bool is_king_not_moved = (std::find_if(moves_.begin(), moves_.end(), [fr](auto& i) { return (fr == i.from || fr == i.to); }) == moves_.end());

    return is_king_not_moved && is_rook_not_moved;
  }
  return false;
}

bool board_logic_t::impl_t::is_check(const COLOR color) const
{
  if (color == NONE)
  {
    return false;
  }

  coord_t f, t;
  const auto king = (color == WHITE) ? W_KING : B_KING;
  for (t.x = 0; t.x < BOARD_SIDE; ++t.x)
    for (t.y = 0; t.y < BOARD_SIDE; ++t.y)
      if (get_figure(t) == king)
      {
        const auto opp_color = (color == WHITE) ? BLACK : WHITE;
        for (f.x = 0; f.x < BOARD_SIDE; ++f.x)
        {
          for (f.y = 0; f.y < BOARD_SIDE; ++f.y)
          {
            if (get_color(f) == opp_color && is_can_move(f, t))
            {
              return true;
            }
          }
        }
        return false;
      }
  return false;
}

void board_logic_t::impl_t::test_on_mate()
{
  coord_t f, t;
  for (f.x = 0; f.x < BOARD_SIDE; ++f.x)
  {
    for (f.y = 0; f.y < BOARD_SIDE; ++f.y)
      if (get_color(f) == get_move_color())
      {
        for (t.x = 0; t.x < BOARD_SIDE; ++t.x)
          for (t.y = 0; t.y < BOARD_SIDE; ++t.y)
          {
            if (is_can_move(f, t))
            {
              const FIGURE FIG_TO = get_figure(t);
              set_field(t, f);

              mate_ = is_check(get_move_color());
              set_field(f, t, FIG_TO);

              if (!mate_)
              {
                return;
              }
            }
          }
      }
  }
  mate_ = true;
}

const board_logic_t::uuid_t& board_logic_t::uuid() const
{
  return impl_->uuid_;
}

bool board_logic_t::impl_t::mate() const
{
  return mate_;
}

void board_logic_t::impl_t::start_new_game()
{
  while (back_move())
    ;
  moves_copy_.clear();
}

bool board_logic_t::impl_t::back_move()
{
  if (!get_move_num())
  {
    return false;
  }

  const Moves& m = moves_.back();
  set_field(m.from, m.to, m.fig_on_captured_field);

  if (m.type == EN_PASSANT)
  {
    re_en_passant();
  }
  else if (m.type == PAWN_TRANSFORM)
  {
    pawn_re_transform();
  }
  else if (m.type == CASTLING)
  {
    if_castling(m.to, m.from);
  }

  moves_.pop_back();

  mate_ = false;
  return true;
}

void board_logic_t::impl_t::re_en_passant()
{
  const auto ind = get_field_index(moves_[moves_.size() - 2].to);
  if (get_color(moves_.back().from) == BLACK)
  {
    field_[ind] = W_PAWN;
  }
  else field_[ind] = B_PAWN;
}

void board_logic_t::impl_t::pawn_re_transform()
{
  field_[get_field_index(moves_.back().from)] = (get_move_color() == BLACK) ? W_PAWN : B_PAWN;
}

void board_logic_t::impl_t::go_to_history(const size_t history_i)
{
  go_to_history_running_ = true;

  while (history_i < get_move_num())
  {
    back_move();
  }

  if (history_i <= moves_copy_.size())
  {
    for (unsigned i = get_move_num(); i < history_i; ++i)
    {
      move(moves_copy_[i].from, moves_copy_[i].to);
    }

    test_on_mate();
  }

  go_to_history_running_ = false;
}

std::string board_logic_t::impl_t::get_board_mask() const
{
  return std::string(field_.begin(), field_.end());
}

std::string board_logic_t::impl_t::get_moves_history() const
{
  enum
  {
    a_LETTER = 'a',
    EIGHT_ch = '8'
  };
  std::string history;
  for (auto hirst_elem : moves_copy_)
  {
    history.push_back(hirst_elem.from.x + a_LETTER);
    history.push_back(EIGHT_ch - hirst_elem.from.y);
    history.push_back(hirst_elem.to.x + a_LETTER);
    history.push_back(EIGHT_ch - hirst_elem.to.y);
  }
  return history;
}

void board_logic_t::impl_t::next_move(const coord_t& from, const coord_t& to)
{
  actual_move_.to = to;
  actual_move_.from = from;
  moves_.push_back(actual_move_);

  if (!go_to_history_running_)
  {
    if (get_move_num() <= moves_copy_.size())
    {
      moves_copy_.erase(moves_copy_.begin() + get_move_num_from_0(), moves_copy_.end());
    }

    moves_copy_.push_back(moves_.back());
  }
}

uint16_t board_logic_t::impl_t::get_move_num() const
{
  return moves_.size();
}

uint16_t board_logic_t::impl_t::get_move_num_from_0() const
{
  return get_move_num() ? get_move_num() - 1 : 0;
}

FIGURE board_logic_t::impl_t::get_figure(const coord_t& c) const
{
  return field_[get_field_index(c)];
}

FIGURE board_logic_t::impl_t::get_figure(const unsigned x, const unsigned y) const
{
  coord_t c(x, y);
  return field_[get_field_index(c)];
}

COLORLESS_FIG board_logic_t::impl_t::get_colorless_fig(const coord_t& c) const
{
  const auto& f = field_[get_field_index(c)];
  return COLORLESS_FIG(tolower(f) + toupper(f));
}

COLOR board_logic_t::impl_t::get_move_color() const
{
  return get_move_num() % 2 ? BLACK : WHITE;
}

void board_logic_t::impl_t::set_field(const coord_t& lhs, const coord_t& rhs, const FIGURE& fig)
{
  if (fig == FREE_FIELD && lhs == rhs)
  {
    return;
  }

  auto& f = field_[get_field_index(rhs)];
  field_[get_field_index(lhs)] = f;
  f = fig;
}

COLOR board_logic_t::impl_t::get_color(const coord_t& c) const
{
  const auto& f = field_[get_field_index(c)];
  if (f == FREE_FIELD)
  {
    return NONE;
  }

  return islower(f) ? WHITE : BLACK;
}

int board_logic_t::impl_t::diff(const int first, const int second) const
{
  return first - second;
}

unsigned board_logic_t::impl_t::get_field_index(const coord_t& c) const
{
  const auto i = c.y * BOARD_SIDE + c.x;

  if (i >= field_.size())
  {
    SPDLOG_WARN("Index={} out of range!", i);
    return 0;
  }
  return i;
}

void make_moves_from_str(const std::string& str, board_logic_t& desk)
{
  enum
  {
    FROM_X = 0,
    FROM_Y = 1,
    TO_X = 2,
    TO_Y = 3,
    COORD_NEED_TO_MOVE = 4,
    a_LETTER = 'a',
    h_LETTER = 'h',
    ONE_ch = '1',
    EIGHT_ch = '8'
  };

  std::vector<int> coord_str;
  for (const auto ch : str)
  {
    if (!((ch >= a_LETTER && ch <= h_LETTER) || (ch >= ONE_ch && ch <= EIGHT_ch)))
    {
      continue;
    }

    coord_str.push_back(isalpha(ch) ? ch - a_LETTER : EIGHT_ch - ch);

    if (coord_str.size() == COORD_NEED_TO_MOVE)
    {
      desk.move(coord_t(coord_str[FROM_X], coord_str[FROM_Y]), coord_t(coord_str[TO_X], coord_str[TO_Y]));
      coord_str.clear();
    }
  }
}

void load_moves_from_file(const std::string& path, board_logic_t& desk)
{
  std::ifstream from_file(path);

  if (!from_file.is_open())
  {
    SPDLOG_WARN("Couldn't open file path={}", path);
    return;
  }

  desk.start_new_game();

  const std::string data_from_file(std::istreambuf_iterator<char>(from_file), (std::istreambuf_iterator<char>()));
  make_moves_from_str(data_from_file, desk);
}

void write_moves_to_file(const std::string& path, board_logic_t& desk)
{
  std::ofstream in_file(path);

  if (!in_file.is_open())
  {
    SPDLOG_WARN("Couldn't open file path={}", path);
    return;
  }

  const std::string history = desk.get_moves_history();
  std::copy(history.begin(), history.end(), std::ostreambuf_iterator<char>(in_file));
}

} // namespace logic
