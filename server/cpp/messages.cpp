#include "messages.h"

#include <algorithm>
#include <iostream>
#include <exception>

#include "log.h"


using namespace messages;

login_t::login_t(const std::string& str)
    : detail::mess_t(), login(str)
{
}

move_t::move_t(const std::string& str)
    : data(str)
{
}

go_to_history_t::go_to_history_t(const std::string& str)
    : detail::mess_t()
{
  try
    { hist_ind = std::stoi(str); }
  catch(const std::invalid_argument& e)
  {
    sr::log("Warning! In go_to_history_t can not convert " + str + " to int");
    hist_ind = 0;
    is_ok = false;
  }
}

inf_request_t::inf_request_t(const std::string& str)
    : detail::mess_t(), data(str)
{
}

game_inf_t::game_inf_t(const std::string& str)
    : detail::mess_t()
{
  const std::string separ = ";";
  const auto sep_size = separ.size();
  const auto first_end = str.find(separ);
  const auto second_end = str.find(separ, first_end + sep_size);
  const auto third_end = str.find(separ, second_end + sep_size);

  board_mask = str.substr(0, first_end);
  moves_history = str.substr(first_end + sep_size, (second_end - sep_size) - first_end);
  is_mate = !(str.substr(second_end + sep_size, (third_end - sep_size) - second_end).empty());
  try
    { move_num = std::stoi(str.substr(third_end + 1)); }
  catch(const std::invalid_argument& e)
  {
    sr::log("Warning! In game_inf_t: can not convert " + str.substr(third_end + 1) + " to int");
    is_ok = false;
    move_num = 0;
  }

  std::cout<<"in game_inf_t str was:"<< str<<std::endl;
  std::cout<<"in game_inf_t:"<< board_mask<< "/ "<< moves_history<< "/ "<< (is_mate)<< "/ "<< move_num<<std::endl;
}

MESSAGE messages::cut_type(std::string& message)
{
  const auto type_end = message.find(" ");
  try
  {
    auto i = (std::stoi(message.substr(0, type_end)));
    message.erase(message.begin(), message.begin() + type_end + 1);
    return MESSAGE(i);
  }
  catch(const std::invalid_argument& e)
  {
    sr::log("Warning! In cut_type: can not convert " + message.substr(0, type_end) + " to int");
    return WRONG_TYPE;
  }
}

detail::mess_t::~mess_t()
{
}

bool operator==(const std::string& str, const MESSAGE m)
{ return str == std::to_string(m); }

bool operator!=(const std::string& str, const MESSAGE m)
{ return str != std::to_string(m); }
