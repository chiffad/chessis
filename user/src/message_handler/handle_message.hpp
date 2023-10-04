#pragma once

#include <messages/messages.hpp>

#include <boost/mpl/begin_end.hpp>
#include <spdlog/spdlog.h>
#include <type_traits>
#include <typeinfo>

namespace message_handler::helper {
namespace details {

template<typename T>
concept one_of_msg_to_ignore =
  msg::one_of<T, msg::some_datagramm_t, msg::message_received_t, msg::is_client_lost_t>; // handled on client side or should not be processed

template<typename T, typename Handler>
struct process_message_t
{
  static void exec(const msg::some_datagramm_t& datagram, Handler& handler)
  {
    if constexpr (!one_of_msg_to_ignore<typename T::type>) // to_client_messages_t and not one of the message to ignore
    {
      if (datagram.type == msg::id_v<typename T::type>)
      {
        process<typename T::type>(datagram, handler);
        return;
      }
    }
    process_message_t<typename boost::mpl::next<T>::type, Handler>::exec(datagram, handler);
  }
};

template<typename Handler>
struct process_message_t<boost::mpl::end<msg::to_client_messages_t>::type, Handler>
{
  static void exec(const msg::some_datagramm_t& datagram, Handler& /*handler*/) { SPDLOG_ERROR("No type found for message to client type={}", datagram.type); }
};

template<typename T, typename Handler>
void process(const msg::some_datagramm_t& datagram, Handler& handler)
{
  SPDLOG_DEBUG("Process struct={}", typeid(T).name());
  handler.handle(msg::init<T>(datagram));
}

} // namespace details

template<typename Handler>
void process_mess_begin(const std::string& str, Handler& handler)
{
  details::process_message_t<boost::mpl::begin<msg::to_client_messages_t>::type, Handler>::exec(msg::init<msg::some_datagramm_t>(str), handler);
}

} // namespace message_handler::helper
