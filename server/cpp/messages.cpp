#include "messages.h"

#include <iostream>
#include <exception>

#include "helper.h"


using namespace messages;

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
