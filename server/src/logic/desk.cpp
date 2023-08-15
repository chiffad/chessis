#include "logic/desk.hpp"

#include "server/client.hpp"

#include <spdlog/spdlog.h>

#include <fstream>
#include <sstream>
#include <vector>

namespace logic {

struct desk_t::impl_t
{
  impl_t(const player_ptr first_player, const player_ptr second_player)
    : first_player_(first_player)
    , second_player_(second_player)
  {}

  const desk_t::player_ptr first_player_;
  const desk_t::player_ptr second_player_;
};

desk_t::desk_t(const player_ptr _1, const player_ptr _2)
  : board_logic_t()
  , impl_(std::make_unique<impl_t>(_1, _2))
{}

desk_t::~desk_t() = default;

bool desk_t::contains_player(const std::weak_ptr<server::client_t>& _1) const
{
  return (_1.lock() == impl_->first_player_.lock() || _1.lock() == impl_->second_player_.lock());
}

// TODO: better to have players() fn instead
desk_t::player_ptr desk_t::get_opponent(const std::shared_ptr<const server::client_t>& player) const
{
  return player == impl_->first_player_.lock() ? impl_->second_player_ : impl_->first_player_;
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