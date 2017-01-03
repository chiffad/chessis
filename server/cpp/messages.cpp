#include "messages.h"

#include <algorithm>
#include <iostream>
#include <exception>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <sstream>

#include "helper.h"


using namespace messages;

boost::property_tree::ptree get_ptree(const std::string& json_str)
{
  std::stringstream ss;
  ss.str(json_str);

  boost::property_tree::ptree pt;
  boost::property_tree::read_json(ss, pt);

  return pt;
}

template<typename T>
bool try_to_set(T& var, const boost::property_tree::ptree& pt, const std::string& name, const std::string& error_mess = std::string())
{
  try
    { var = pt.get<T>(name); }
  catch(const std::exception& e)
  {
    sr::helper::log(error_mess, " ; ", e.what());
    var = T();
    return false;
  }

  return true;
}

login_t::login_t()
    : detail::mess_t()
{
}

std::string login_t::to_json() const
{
  return sr::helper::get_str("{", "\"login\": ", /*",\"pwd\": ", pwd,*/login, "}");
}

void login_t::from_json(const std::string& str)
{
  const auto pt = get_ptree(str);
  if(!try_to_set(login, pt, "login", "login_t: Can not find login or pwd in " + str))
    { is_ok = false; }
}


move_t::move_t()
    : detail::mess_t()
{
}

std::string move_t::to_json() const
{
  return sr::helper::get_str("{", "\"move\": ", data, "}");
}

void move_t::from_json(const std::string& str)
{
  const auto pt = get_ptree(str);
  if(!try_to_set(data, pt, "move", "move_t: Can not find move in " + str))
    { is_ok = false; }
}

go_to_history_t::go_to_history_t()
    : detail::mess_t()
{
}

std::string go_to_history_t::to_json() const
{
  return sr::helper::get_str("{", "\"hist_ind\": ", hist_ind, "}");
}

void go_to_history_t::from_json(const std::string& str)
{
  const auto pt = get_ptree(str);
  if(!try_to_set(hist_ind, pt, "hist_ind", "go_to_history_t: Can not find hist_ind in " + str))
    { is_ok = false; }
}


inf_request_t::inf_request_t()
    : detail::mess_t()
{
}

std::string inf_request_t::to_json() const
{
  return sr::helper::get_str("{", "\"inf\": ", data, "}");
}

void inf_request_t::from_json(const std::string& str)
{
  const auto pt = get_ptree(str);
  if(!try_to_set(data, pt, "inf", "inf_request_t: Can not find inf in " + str))
    { is_ok = false; }
}


game_inf_t::game_inf_t()
    : detail::mess_t()
{
}

std::string game_inf_t::to_json() const
{
  return sr::helper::get_str("{", "\"board_mask\": ", board_mask, ", \"moves_history\": ", moves_history, ", \"is_mate\": ", is_mate, ", \"move_num\": ", move_num, "}");
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
    sr::helper::log("game_inf_t: Can not find inf in ", str);
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
    sr::helper::log("Warning! In cut_type: can not convert " + message.substr(0, type_end) + " to int");
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
