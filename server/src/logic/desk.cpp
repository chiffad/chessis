#include "logic/desk.hpp"

#include "server/client.hpp"

#include <fstream>
#include <sstream>
#include <vector>

namespace logic {

struct desk_t::impl_t
{
  impl_t(const std::weak_ptr<const server::client_t> _1, const std::weak_ptr<const server::client_t> _2, board_t* par);
  void make_moves_from_str(const std::string& str);
  bool is_contain_player(const std::weak_ptr<server::client_t>& _1) const;
  const std::weak_ptr<const server::client_t> get_opponent(const std::shared_ptr<const server::client_t>& _1) const;

  void load_moves_from_file(const std::string& path);
  void write_moves_to_file(const std::string& path) const;

  const std::weak_ptr<const server::client_t> first_player_;
  const std::weak_ptr<const server::client_t> second_player_;

  board_t* parent_;
};

desk_t::desk_t(const std::weak_ptr<const server::client_t> _1, const std::weak_ptr<const server::client_t> _2)
  : board_t()
  , impl_(std::make_unique<impl_t>(_1, _2, this))
{}

desk_t::~desk_t()
{}

void desk_t::make_moves_from_str(const std::string& str)
{
  impl_->make_moves_from_str(str);
}

bool desk_t::is_contain_player(const std::weak_ptr<server::client_t>& _1) const
{
  return impl_->is_contain_player(_1);
}

const std::weak_ptr<const server::client_t> desk_t::get_opponent(const std::shared_ptr<const server::client_t>& _1) const
{
  return impl_->get_opponent(_1);
}

void desk_t::load_moves_from_file(const std::string& path)
{
  impl_->load_moves_from_file(path);
}

void desk_t::write_moves_to_file(const std::string& path) const
{
  impl_->write_moves_to_file(path);
}

desk_t::impl_t::impl_t(const std::weak_ptr<const server::client_t> _1, const std::weak_ptr<const server::client_t> _2, board_t* par)
  : first_player_(_1)
  , second_player_(_2)
  , parent_(par)
{}

void desk_t::impl_t::make_moves_from_str(const std::string& str)
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
  for (auto res : str)
  {
    if (!((res >= a_LETTER && res <= h_LETTER) || (res >= ONE_ch && res <= EIGHT_ch)))
    {
      continue;
    }

    coord_str.push_back(isalpha(res) ? res - a_LETTER : EIGHT_ch - res);

    if (coord_str.size() == COORD_NEED_TO_MOVE)
    {
      parent_->move(coord_t(coord_str[FROM_X], coord_str[FROM_Y]), coord_t(coord_str[TO_X], coord_str[TO_Y]));
      coord_str.clear();
    }
  }
}

bool desk_t::impl_t::is_contain_player(const std::weak_ptr<server::client_t>& _1) const
{
  return (_1.lock() == first_player_.lock() || _1.lock() == second_player_.lock());
}

const std::weak_ptr<const server::client_t> desk_t::impl_t::get_opponent(const std::shared_ptr<const server::client_t>& _1) const
{
  return (_1 == first_player_.lock() ? second_player_ : first_player_);
}

void desk_t::impl_t::write_moves_to_file(const std::string& path) const
{
  std::ofstream in_file(path);

  if (!in_file.is_open())
  {
    std::cout << "Warning! in Board::write_moves_to_file: Couldn't open file." << std::endl;
    return;
  }

  std::string history = parent_->get_moves_history();
  std::copy(history.begin(), history.end(), std::ostreambuf_iterator<char>(in_file));
}

void desk_t::impl_t::load_moves_from_file(const std::string& path)
{
  std::ifstream from_file(path);

  if (!from_file.is_open())
  {
    std::cout << "Warning! in Board::write_moves_to_file: Couldn't open file." << std::endl;
    return;
  }

  std::string data_from_file(std::istreambuf_iterator<char>(from_file), (std::istreambuf_iterator<char>()));

  parent_->start_new_game();
  make_moves_from_str(data_from_file);
}

} // namespace logic