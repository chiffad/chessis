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

int msg::get_ser_num(const std::string& message)
{
  std::stringstream ss;
  ss.str(message);
  detail::message_t m;
  boost::archive::text_iarchive ia(ss);
  ia>>m;
  
  return m.ser_num;
}

std::string msg::get_msg_data(const std::string& message)
{
  std::stringstream ss;
  ss.str(message);
  detail::message_t m;
  boost::archive::text_iarchive ia(ss);
  ia>>m;
  
  return m.mess;
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
  std::stringstream ss;
  ss.str(message);
  int type;
  boost::archive::text_iarchive ia(ss);
  ia>>type;
  
  return type;
}
