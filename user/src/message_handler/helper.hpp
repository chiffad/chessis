#pragma once

#include <messages/messages.hpp>

#include <boost/mpl/begin_end.hpp>
#include <spdlog/spdlog.h>
#include <type_traits>
#include <typeinfo>

namespace chess::message_handler::helper {
namespace details {

template<typename T>
concept one_of_msg_to_ignore = msg::one_of<T, msg::some_datagram_t, msg::message_received_t, msg::is_client_lost_t>; // handled on client side or should not be processed

template<typename T_it, typename Handler>
struct msg_type_finder_t
{
  using T = typename T_it::type;
  static_assert(msg::one_of_to_client_msgs<T>, "Only to client msgs should be processed!");

  static void exec(msg::some_datagram_t datagram, Handler& handler)
  {
    if constexpr (!one_of_msg_to_ignore<T>) // to_client_messages_t and not one of the message to ignore
    {
      if (datagram.type == msg::id_v<T>)
      {
        SPDLOG_DEBUG("Process struct={}", typeid(T).name());
        handler.handle(msg::init<T>(std::move(datagram)));
        return;
      }
    }
    msg_type_finder_t<typename boost::mpl::next<T_it>::type, Handler>::exec(std::move(datagram), handler);
  }
};

template<typename Handler>
struct msg_type_finder_t<boost::mpl::end<msg::to_client_messages_t>::type, Handler>
{
  static void exec(const msg::some_datagram_t& datagram, Handler& /*handler*/) { SPDLOG_ERROR("No type found for message to client type={}", datagram.type); }
};

} // namespace details

template<typename Handler>
void find_msg_type_and_handle(msg::some_datagram_t d, Handler& handler)
{
  details::msg_type_finder_t<boost::mpl::begin<msg::to_client_messages_t>::type, Handler>::exec(std::move(d), handler);
}

} // namespace chess::message_handler::helper
