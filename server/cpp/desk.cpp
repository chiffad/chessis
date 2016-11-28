#include "desk.h"

#include <vector>
#include <fstream>
#include <sstream>


using namespace logic;


desk_t::desk_t(const unsigned first_p, const unsigned second_p)
    : board_t(), _first_player_ind(first_p), _second_player_ind(second_p)
{
}

bool desk_t::contain_player(const int ind) const
{
  return (ind == _first_player_ind || ind == _second_player_ind);
}

void desk_t::make_moves_from_str(const std::string &str)
{
  enum{FROM_X = 0, FROM_Y = 1, TO_X = 2, TO_Y = 3, COORD_NEED_TO_MOVE = 4, a_LETTER = 'a', h_LETTER = 'h', ONE_ch = '1', EIGHT_ch = '8'};

  std::vector<int> coord_str;
  for(auto res : str)
  {
    if(!((res >= a_LETTER && res <= h_LETTER) || (res >= ONE_ch && res <= EIGHT_ch)))
     {  continue; }

    coord_str.push_back(isalpha(res) ? res - a_LETTER : EIGHT_ch - res);

    if(coord_str.size() == COORD_NEED_TO_MOVE)
    {
      move(coord_t(coord_str[FROM_X], coord_str[FROM_Y]),
           coord_t(coord_str[TO_X], coord_str[TO_Y]));
      coord_str.clear();
    }
  }
}

void desk_t::write_moves_to_file(const std::string &path) const
{
  std::ofstream in_file(path);

  if(!in_file.is_open())
  {
    std::cout<<"Warning! in Board::write_moves_to_file: Couldn't open file."<<std::endl;
    return;
  }

  std::string history = get_moves_history();
  std::copy(history.begin(), history.end(), std::ostreambuf_iterator<char>(in_file));
}

void desk_t::load_moves_from_file(const std::string &path)
{
  std::ifstream from_file(path);

  if(!from_file.is_open())
  {
    std::cout<<"Warning! in Board::write_moves_to_file: Couldn't open file."<<std::endl;
    return;
  }

  std::string data_from_file(std::istreambuf_iterator<char>(from_file), (std::istreambuf_iterator<char>()));

  start_new_game();
  make_moves_from_str(data_from_file);
}

