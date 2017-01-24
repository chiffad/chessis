#include "messages.h"

int msg::get_msg_type(const std::string& str)
{
  std::stringstream ss;
  ss.str(str);
  int m;
  boost::archive::text_iarchive ia(ss);
  ia>>m;
  
  return m;
}
