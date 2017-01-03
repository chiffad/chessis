#include "messages.h"

#include <algorithm>
#include <iostream>
#include <exception>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <sstream>

#include "log.h"


using namespace messages;

boost::property_tree::ptree get_ptree(const std::string& json_str)
{
  std::stringstream ss;
  ss.str(json_str);

  boost::property_tree::ptree pt;
  boost::property_tree::read_json(ss, pt);

  return pt;
}

template<typename OS, typename T>
void get_string1(OS& ostream, T _1)
{
  ostream<< _1;
}

template<typename OS, typename T, typename... Args>
void get_string1(OS& ostream, T _1, Args... args)
{
  ostream<<_1;
  get_string1(ostream, args...);
}

template<typename... Args>
std::string get_string(Args... args)
{
  std::ostringstream ss;
  get_string1(ss, args...);
  return ss.str();
}

template<typename T>
bool try_to_set(T& var, const boost::property_tree::ptree& pt, const std::string& name, const std::string& error_mess = std::string())
{
  try
    { var = pt.get<T>(name); }
  catch(const std::exception& e)
  {
    sr::log(error_mess, " ; ", e.what());
    var = T();
    return false;
  }

  return true;
}

login_t::login_t(const std::string& str)
    : detail::mess_t(), login(str)
{
}

std::string login_t::to_json() const
{
  return get_string("{", "\"login\": ", /*",\"pwd\": ", pwd,*/login, "}");
}

void login_t::from_json(const std::string& str)
{
  const auto pt = get_ptree(str);
  if(!try_to_set(login, pt, "login", "login_t: Can not find login or pwd in " + str))
    { is_ok = false; }
}


move_t::move_t(const std::string& str)
    : data(str)
{
}

std::string move_t::to_json() const
{
  return get_string("{", "\"move\": ", data, "}");
}

void move_t::from_json(const std::string& str)
{
  const auto pt = get_ptree(str);
  if(!try_to_set(data, pt, "move", "move_t: Can not find move in " + str))
    { is_ok = false; }
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

std::string go_to_history_t::to_json() const
{
  return get_string("{", "\"hist_ind\": ", hist_ind, "}");
}

void go_to_history_t::from_json(const std::string& str)
{
  const auto pt = get_ptree(str);
  if(!try_to_set(hist_ind, pt, "hist_ind", "go_to_history_t: Can not find hist_ind in " + str))
    { is_ok = false; }
}


inf_request_t::inf_request_t(const std::string& str)
    : detail::mess_t(), data(str)
{
}

std::string inf_request_t::to_json() const
{
  return get_string("{", "\"inf\": ", data, "}");
}

void inf_request_t::from_json(const std::string& str)
{
  const auto pt = get_ptree(str);
  if(!try_to_set(data, pt, "inf", "inf_request_t: Can not find inf in " + str))
    { is_ok = false; }
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

std::string game_inf_t::to_json() const
{
  return get_string("{", "\"board_mask\": ", board_mask, ", \"moves_history\": ", moves_history, ", \"is_mate\": ", is_mate, ", \"move_num\": ", move_num, "}");
}

void game_inf_t::from_json(const std::string& str)
{
  const auto pt = get_ptree(str);
  if(
    !try_to_set(board_mask,    pt, "board_mask")    ||
    !try_to_set(moves_history, pt, "moves_history") ||
    !try_to_set(is_mate,       pt, "is_mate")       ||
    !try_to_set(move_num,      pt, "move_num")
  )
  {
    is_ok = false;
    sr::log("game_inf_t: Can not find inf in ", str);
  }
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
