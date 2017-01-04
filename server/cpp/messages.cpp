#include "messages.h"

#include <algorithm>
#include <iostream>
#include <exception>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <sstream>
#include <utility>

#include "helper.h"


using namespace messages;

struct my_ptree_t
{
  my_ptree_t(const std::string& data)
      : json(data)
  {
    try
    {
      std::stringstream ss;
      ss.str(data);

      boost::property_tree::read_json(ss, pt);
    }
    catch(const std::exception& e)
      { throw my_except("Can't read json from data: " + data); }
  }

  #define set(var)  set_fn(var, #var)

  template<typename T>
  void set_fn(T& var, const std::string& var_name) const
  {
    try
      { var = pt.get<T>(var_name); }
    catch(const std::exception& e)
    {
      var = T();
      throw my_except("Can't find '" + var_name + "' in " + json);
    }
  }
  private:
    boost::property_tree::ptree pt;
    std::string json;
};

std::string login_t::to_json() const
{
  return sr::helper::get_str("{\"login\": ", /*",\"pwd\": ", pwd,*/"\"",login, "\"}");
}

void login_t::from_json(const std::string& str)
{
  my_ptree_t pt(str);
  pt.set(login);
}


std::string move_t::to_json() const
{
  return sr::helper::get_str("{\"data\": \"",data, "\"}");
}

void move_t::from_json(const std::string& str)
{
  my_ptree_t pt(str);
  pt.set(data);
}


std::string go_to_history_t::to_json() const
{
  return sr::helper::get_str("{\"index\": ", index, "}");
}

void go_to_history_t::from_json(const std::string& str)
{
  my_ptree_t pt(str);
  pt.set(index);
}


std::string inf_request_t::to_json() const
{
  return sr::helper::get_str("{\"data\": \"", data, "\"}");
}

void inf_request_t::from_json(const std::string& str)
{
  my_ptree_t pt(str);
  pt.set(data);
}


std::string game_inf_t::to_json() const
{
  return sr::helper::get_str("{\"board_mask\": \"", board_mask, "\", \"moves_history\": \"", moves_history, "\", \"is_mate\": ", is_mate, ", \"move_num\": ", move_num, "}");
}

void game_inf_t::from_json(const std::string& str)
{
  my_ptree_t pt(str);
  pt.set(board_mask);
  pt.set(moves_history);
  pt.set(is_mate);
  pt.set(move_num);
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

bool operator==(const std::string& str, const MESSAGE m)
{ return str == std::to_string(m); }

bool operator!=(const std::string& str, const MESSAGE m)
{ return str != std::to_string(m); }
