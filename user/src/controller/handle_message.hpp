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
concept one_of_msg_to_ignore =
  msg::one_of<T, msg::some_datagramm_t, msg::message_received_t, msg::is_client_lost_t>; // handled on client side or should not be processed

template<typename T>
void process_mess(const msg::some_datagramm_t& datagram, controller::message_processor_t& mp)
{
  if constexpr (!one_of_msg_to_ignore<typename T::type>) // to_client_messages_t and not one of the message to ignore
  {
    if (datagram.type == msg::id_v<typename T::type>)
    {
      process<typename T::type>(mp, datagram);
      return;
    }
  }
  process_mess<typename boost::mpl::next<T>::type>(datagram, mp);
}

template<>
inline void process_mess<boost::mpl::end<msg::to_client_messages_t>::type>(const msg::some_datagramm_t& datagram, controller::message_processor_t& /*mp*/)
{
  SPDLOG_ERROR("No type found for message to client type={}", datagram.type);
}

template<typename T>
void process(controller::message_processor_t& mp, const msg::some_datagramm_t& datagram)
{
  SPDLOG_DEBUG("Process struct={}", typeid(T).name());
  mp.process(msg::init<T>(datagram));
}

} // namespace details

inline void process_mess_begin(const std::string& str, controller::message_processor_t& mp)
{
  details::process_mess<boost::mpl::begin<msg::to_client_messages_t>::type>(msg::init<msg::some_datagramm_t>(str), mp);
}

} // namespace handler
