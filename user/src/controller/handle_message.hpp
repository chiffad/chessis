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
void process_mess(const std::string& str, controller::message_processor_t& mp)
{
  if (msg::is_equal_types<typename T::type>(str))
  {
    process<typename T::type>(mp, str);
  }
  else
  {
    process_mess<typename boost::mpl::next<T>::type>(str, mp);
  }
}

template<>
inline void process_mess<boost::mpl::end<msg::message_types>::type>(const std::string& /*str*/, controller::message_processor_t& /*mp*/)
{
  SPDLOG_ERROR("no type found!!");
}

template<typename struct_t>
void process(controller::message_processor_t& mp, const std::string& message)
{
  using processible_message_types = boost::mpl::vector<msg::inf_request_t, msg::game_inf_t, msg::get_login_t, msg::incorrect_log_t, msg::opponent_lost_t>;
  using end_t = typename boost::mpl::end<processible_message_types>::type;
  using found_type = typename boost::mpl::find<processible_message_types, struct_t>::type;
  if constexpr (std::is_same_v<found_type, end_t>)
  {
    SPDLOG_DEBUG("Received unexpected message type={}", typeid(struct_t).name());
    return;
  }
  else
  {
    SPDLOG_DEBUG("Process struct={}", typeid(struct_t).name());
    mp.process(msg::init<struct_t>(message));
  }
}

} // namespace details

inline void process_mess_begin(const std::string& str, controller::message_processor_t& mp)
{
  details::process_mess<boost::mpl::begin<msg::message_types>::type>(str, mp);
}

} // namespace handler
