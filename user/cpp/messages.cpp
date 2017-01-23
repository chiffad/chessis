#include "messages.h"

using namespace msg;

namespace detail
{

struct message_t
{
  int ser_num;
  std::string mess;
};
    
template<typename Archive>
void serialize(Archive& ar, message_t& _1, const unsigned /*version*/)
{
  ar & _1.ser_num;
  ar & _1.mess;
}

}// namespace detail

template<typename T>
T create_T_and_fill(const std::string& str)
{
  std::stringstream ss;
  ss.str(str);
  T m;
  boost::archive::text_iarchive ia(ss);
  ia>>m;
  
  return m;
}

int msg::get_ser_num(const std::string& message)
{
  return create_T_and_fill<detail::message_t>(message).ser_num;
}

std::string msg::get_msg_data(const std::string& message)
{
  return create_T_and_fill<detail::message_t>(message).mess;
}

std::string msg::add_ser_num(const std::string& message, const int num)
{
  detail::message_t m;
  m.ser_num = num;
  m.mess = message;
  
  std::stringstream ss;
  boost::archive::text_oarchive oa(ss);
  oa<<m;
  
  return ss.str();
}

int msg::get_msg_type(const std::string& message)
{
  return create_T_and_fill<int>(message);
}
