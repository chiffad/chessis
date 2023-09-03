#pragma once

#include "controller/message_processor.hpp"
#include "helper.h"
#include <messages/messages.hpp>

#include <boost/mpl/begin_end.hpp>
#include <spdlog/spdlog.h>
#include <type_traits>
#include <typeinfo>

namespace handler {

namespace details {

template<typename T>
concept one_of_processible_msgs = msg::one_of<T, msg::inf_request_t, msg::game_inf_t, msg::get_login_t, msg::incorrect_log_t, msg::opponent_lost_t>;

template<typename T>
void process_mess(const std::string& str, controller::message_processor_t& mp)
{
  if constexpr (one_of_processible_msgs<typename T::type>)
  {
    if (msg::is_equal_types<typename T::type>(str))
    {
      process<typename T::type>(mp, str);
      return;
    }
  }
  process_mess<typename boost::mpl::next<T>::type>(str, mp);
}

template<>
inline void process_mess<boost::mpl::end<msg::messages_t>::type>(const std::string& str, controller::message_processor_t& /*mp*/)
{
  SPDLOG_ERROR("No type found for message to client type={}", msg::init<msg::some_datagramm_t>(str).type);
}

template<one_of_processible_msgs msg_t>
void process(controller::message_processor_t& mp, const std::string& message)
{
  SPDLOG_DEBUG("Process struct={}", typeid(msg_t).name());
  mp.process(msg::init<msg_t>(message));
}

} // namespace details

inline void process_mess_begin(const std::string& str, controller::message_processor_t& mp)
{
  details::process_mess<boost::mpl::begin<msg::messages_t>::type>(str, mp);
}

} // namespace handler
