#include "messages.h"

#include <algorithm>
#include <iostream>

#include "log.h"

using namespace messages;

login_t::login_t(const std::string& str)
    : login(str)
{
}

move_t::move_t(const std::string& str)
    : data(str)
{
}

go_to_history_t::go_to_history_t(const std::string& str)
    : hist_ind(std::stoi(str))
{
}

inf_request_t::inf_request_t(const std::string& str)
    : data(str)
{
}

game_inf_t::game_inf_t(const std::string& str)
{
  const std::string separ = ";";
  const auto sep_size = separ.size();
  const auto first_end = str.find(separ);
  const auto second_end = str.find(separ, first_end + sep_size);
  const auto third_end = str.find(separ, second_end + sep_size);

  board_mask = str.substr(0, first_end);
  moves_history = str.substr(first_end + sep_size, (second_end - sep_size) - first_end);
  is_mate = !(str.substr(second_end + sep_size, (third_end - sep_size) - second_end).empty());
  move_num = std::stoi(str.substr(third_end + 1));

  std::cout<<"in game_inf_t str was:"<< str<<std::endl;
  std::cout<<"in game_inf_t:"<< board_mask<< "/ "<< moves_history<< "/ "<< (is_mate)<< "/ "<< move_num<<std::endl;
}

MESSAGE helper::cut_type(std::string& message)
{
  const auto type_end = message.find(" ");
  auto i = (std::stoi(message.substr(0, type_end)));
  message.erase(message.begin(), message.begin() + type_end + 1);

  return MESSAGE(i);
}
