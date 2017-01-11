#include "messages.h"

using namespace msg;

int msg::get_msg_type(const std::string& message)
{
  std::stringstream ss;
  ss.str(message);
  int type;
  boost::archive::text_iarchive ia(ss);
  ia>>type;
  
  return type;
}
