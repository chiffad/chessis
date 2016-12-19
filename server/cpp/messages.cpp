#include "messages.h"
#include "log.h"
#include <iostream>

using namespace messages;

message_t::message_t(const int t)
    : type(t)
{
}

void message_t::gather(const std::string& str)
{
  std::cout<<"message_t::gather"<<str<<std::endl;
}

go_to_history_t::go_to_history_t()
    : message_t(GO_TO_HISTORY)
{
}

void go_to_history_t::gather(const std::string& str)
{
  std::cout<<"go_to_history_t::gather"<<str<<std::endl;
  hist_ind = std::stoi(str);
}

inf_request_t::inf_request_t()
    : message_t(INF_REQUEST)
{
}

void inf_request_t::gather(const std::string& str)
{
  std::cout<<"inf_request_t::gather"<<str<<std::endl;
  data = str;
}

std::shared_ptr<message_t> helper::get_and_init_message_struct(const std::string& str)
{
  const int first_space = str.find_first_of(" ");
  const std::string str_type(str.begin(), str.begin() + first_space);
  const int type = std::stoi(str_type);
  const std::string data(str.begin() + first_space + 1, str.end());

  std::shared_ptr<message_t> _1;
  switch(type)
  {
    case GO_TO_HISTORY:
    {
      std::cout<<"GO_TO_HISTORY"<<std::endl;
      _1 = std::make_shared<go_to_history_t>();
      break;
    }
    case INF_REQUEST:
    {
      std::cout<<"INF_REQUEST"<<std::endl;
      _1 = std::make_shared<inf_request_t>();
      break;
    }
    default:
      std::cout<<"default"<<std::endl;
      _1 = std::make_shared<message_t>(1);
  }

  std::cout<<"here"<<std::endl;
  std::cout<<_1->type<<std::endl;

  (_1)->gather(data);


std::cout<<"here5"<<std::endl;

  return _1;
}

