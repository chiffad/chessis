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
  my_ptree_t() = default;

  my_ptree_t(const std::string& data)
  {
    try
    {
      json.str(data);
      boost::property_tree::read_json(json, pt);
    }
    catch(const std::exception& e)
      { throw my_except("Can't read json from data: " + data); }
  }

  std::string get_json(const MESSAGE type)
  {
    try
    {
//      boost::property_tree::ptree pt2;
//      pt2.put("type", type);
//      pt2.put_child("content", type);
//      boost::property_tree::write_json(json, pt2);
      boost::property_tree::write_json(json, pt);
      return json.str();
    }
    catch(const std::exception& e)
      { throw my_except("Can't write json"); }
  }

  #define set_from_json(var)  set_fn(var, #var)
  #define put_to_json(var)  put_fn(var, #var)

  template<typename T>
  void set_fn(T& var, const std::string& var_name) const
  {
    try
      { var = pt.get<T>(var_name); }
    catch(const std::exception& e)
    {
      var = T();
      throw my_except("Can't find '" + var_name + "' in " + json.str());
    }
  }

  template<typename T>
  void put_fn(T& var, const std::string& var_name)
  {
    try
      { pt.put(var_name, var); }
    catch(const std::exception& e)
    {
      throw my_except("Can't put '" + var_name + "' in json");
    }
  }

  private:
    boost::property_tree::ptree pt;
    std::stringstream json;
};

std::string login_t::to_json() const
{
  my_ptree_t pt;
  pt.put_to_json(login);
//    pt.put_to_json(pwd);
  return pt.get_json(LOGIN);
}

void login_t::from_json(const std::string& str)
{
  my_ptree_t pt(str);
  pt.set_from_json(login);
}


std::string move_t::to_json() const
{
  my_ptree_t pt;
  pt.put_to_json(data);
  return pt.get_json(MOVE);
}

void move_t::from_json(const std::string& str)
{
  my_ptree_t pt(str);
  pt.set_from_json(data);
}


std::string go_to_history_t::to_json() const
{
  my_ptree_t pt;
  pt.put_to_json(index);
  return pt.get_json(GO_TO_HISTORY);
}

void go_to_history_t::from_json(const std::string& str)
{
  my_ptree_t pt(str);
  pt.set_from_json(index);
}


std::string inf_request_t::to_json() const
{
  my_ptree_t pt;
  pt.put_to_json(data);
  return pt.get_json(INF_REQUEST);
}

void inf_request_t::from_json(const std::string& str)
{
  my_ptree_t pt(str);
  pt.set_from_json(data);
}


std::string game_inf_t::to_json() const
{
  my_ptree_t pt;
  pt.put_to_json(board_mask);
  pt.put_to_json(moves_history);
  pt.put_to_json(is_mate);
  pt.put_to_json(move_num);

  return pt.get_json(GAME_INF);
}

void game_inf_t::from_json(const std::string& str)
{
  my_ptree_t pt(str);
  pt.set_from_json(board_mask);
  pt.set_from_json(moves_history);
  pt.set_from_json(is_mate);
  pt.set_from_json(move_num);
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


/*
MESSAGE messages::get_type(const std::string& message)
{
  my_ptree_t pt;
  int type = 0;
  pt.set_from_json(type);

  return MESSAGE(type);
}

MESSAGE messages::get_content(const std::string& message)
{
  std::stringstream json;
  json.str(message);

  boost::property_tree::ptree pt;
  boost::property_tree::read_json(json, pt);

  auto conent = pt.get_child("content");
  std::stringstream content_json;

  

  my_ptree_t pt;
  int type = 0;
  pt.set_from_json(type);

  return MESSAGE(type);
}*/


bool operator==(const std::string& str, const MESSAGE m)
{ return str == std::to_string(m); }

bool operator!=(const std::string& str, const MESSAGE m)
{ return str != std::to_string(m); }
