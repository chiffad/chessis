#include "messages.h"
#include <iostream>
#include <algorithm>

using namespace messages;

message_t::message_t(const MESSAGE t)
    : type(t)
{
}

message_t::~message_t()
{
}


go_to_history_t::go_to_history_t(const std::string& str)
    : message_t(GO_TO_HISTORY), hist_ind(std::stoi(str))
{
}

inf_request_t::inf_request_t(const std::string& str)
    : message_t(INF_REQUEST), data(str)
{
}


std::shared_ptr<void> helper::get_and_init_message_struct(const std::string& str)
{
  const auto first_space = std::find(str.begin(), str.end(), ' ');
  const std::string str_type(str.begin(), first_space);
  const int type = std::stoi(str_type);
  const std::string data(first_space + 1, str.end());

  std::shared_ptr<void> _1 = nullptr;
  switch(type)
  {
    case GO_TO_HISTORY:
    {
      std::cout<<"GO_TO_HISTORY"<<std::endl;
      _1 = std::make_shared<go_to_history_t>(data);
      break;
    }
    case INF_REQUEST:
    {
      std::cout<<"INF_REQUEST"<<std::endl;
      _1 = std::make_shared<inf_request_t>(data);
      break;
    }
    default:
      std::cout<<"default"<<std::endl;
      _1 = std::make_shared<message_t>(HELLO_SERVER);
  }

  return _1;
}
