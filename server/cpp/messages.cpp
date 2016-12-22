#include "messages.h"

#include <iostream>
#include <algorithm>
#include <utility>

#include "log.h"

using namespace messages;

message_t::message_t(const MESSAGE t)
    : type(t)
{
}

message_t::~message_t()
{
  sr::log("message_t::~message_t()");
}

login_t::login_t(const std::string& str)
    : message_t(LOGIN), login(str)
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

std::shared_ptr<message_t> helper::get_and_init_message_struct (const std::string& str)
{
  const auto first_space = std::find(str.begin(), str.end(), ' ');
  const std::string str_type(str.begin(), first_space);
  const int type = std::stoi(str_type);
  const std::string data((first_space == str.end() ? first_space : first_space + 1), str.end());

  std::shared_ptr<message_t> _1 = std::make_shared<message_t>(HELLO_SERVER);
  switch(type)
  {
    case GO_TO_HISTORY:
    {
      sr::log("GO_TO_HISTORY");
      _1 = std::make_shared<go_to_history_t>(data);
      break;
    }
    case INF_REQUEST:
    {
      sr::log("INF_REQUEST");
      _1 = std::make_shared<inf_request_t>(data);
      break;
    }
    default:
      sr::log("default");
  }

  return _1;
}
