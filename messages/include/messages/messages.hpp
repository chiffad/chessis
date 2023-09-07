#pragma once

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/mpl/find.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/serialization/string.hpp>
#include <concepts>
#include <string>
#include <string_view>
#include <vector>

namespace msg {
struct my_archive_exception : public boost::archive::archive_exception
{
  my_archive_exception(const std::string& ex)
    : boost::archive::archive_exception(boost::archive::archive_exception::input_stream_error)
    , message(ex)
  {}
  virtual const char* what() const throw() { return message.data(); }
  std::string message;
};

struct login_t
{
  std::string login;
  std::string pwd;
};

struct move_t
{
  std::string data;
};

struct go_to_history_t
{
  int index{};
};

struct inf_request_t
{
  std::string data;
};

struct game_inf_t
{
  std::string board_mask;
  std::string moves_history;
  bool is_mate{};
  int move_num{};
  bool playing_white{};
};

struct login_response_t
{
  std::string uuid;
  std::string logic_server_address;
  unsigned logic_server_port;
};

struct incoming_datagramm_t
{
  std::string data;
  int ser_num{};
};

struct some_datagramm_t
{
  some_datagramm_t() = default;
  some_datagramm_t(std::string data, std::string type)
    : data(std::move(data))
    , type(std::move(type))
  {}

  some_datagramm_t(std::string data, std::string_view type)
    : some_datagramm_t(std::move(data), std::string(type))
  {}

  std::string data;
  std::string type;
};

template<typename Archive>
void serialize(Archive& ar, game_inf_t& _1, const unsigned /*version*/)
{
  ar& _1.board_mask;
  ar& _1.moves_history;
  ar& _1.is_mate;
  ar& _1.move_num;
  ar& _1.playing_white;
}

template<typename Archive>
void serialize(Archive& ar, inf_request_t& _1, const unsigned /*version*/)
{
  ar& _1.data;
}

template<typename Archive>
void serialize(Archive& ar, go_to_history_t& _1, const unsigned /*version*/)
{
  ar& _1.index;
}

template<typename Archive>
void serialize(Archive& ar, move_t& _1, const unsigned /*version*/)
{
  ar& _1.data;
}

template<typename Archive>
void serialize(Archive& ar, login_t& _1, const unsigned /*version*/)
{
  ar& _1.login;
  ar& _1.pwd;
}

template<typename Archive>
void serialize(Archive& ar, login_response_t& _1, const unsigned /*version*/)
{
  ar& _1.logic_server_address;
  ar& _1.logic_server_port;
  ar& _1.uuid;
}

template<typename Archive>
void serialize(Archive& ar, incoming_datagramm_t& _1, const unsigned /*version*/)
{
  ar& _1.ser_num;
  ar& _1.data;
}

template<typename Archive>
void serialize(Archive& ar, some_datagramm_t& _1, const unsigned /*version*/)
{
  ar& _1.type;
  ar& _1.data;
}

template<typename T>
constexpr std::string_view msg_type();

#define MSG_TYPE(name)                                                                                                                                         \
  template<>                                                                                                                                                   \
  constexpr std::string_view msg_type<name>()                                                                                                                  \
  {                                                                                                                                                            \
    return #name;                                                                                                                                              \
  }

MSG_TYPE(incoming_datagramm_t);
MSG_TYPE(some_datagramm_t);
MSG_TYPE(game_inf_t);
MSG_TYPE(login_t);
MSG_TYPE(login_response_t);
MSG_TYPE(move_t);
MSG_TYPE(go_to_history_t);
MSG_TYPE(inf_request_t);

#define SIMPLE_MSG(name)                                                                                                                                       \
  struct name                                                                                                                                                  \
  {};                                                                                                                                                          \
  template<typename Archive>                                                                                                                                   \
  void serialize(Archive& /*ar*/, name&, const unsigned /*version*/)                                                                                           \
  {}                                                                                                                                                           \
  MSG_TYPE(name)

SIMPLE_MSG(hello_server_t);
SIMPLE_MSG(message_received_t);
SIMPLE_MSG(is_server_lost_t);
SIMPLE_MSG(is_client_lost_t);
SIMPLE_MSG(opponent_inf_t);
SIMPLE_MSG(my_inf_t);
SIMPLE_MSG(get_login_t);
SIMPLE_MSG(incorrect_log_t);
SIMPLE_MSG(back_move_t);
SIMPLE_MSG(new_game_t);
SIMPLE_MSG(opponent_lost_t);
#undef SIMPLE_MSG
#undef MSG_TYPE

using messages_t = boost::mpl::vector<hello_server_t, message_received_t, is_server_lost_t, is_client_lost_t, opponent_inf_t, my_inf_t, get_login_t, login_t,
                                      login_response_t, incorrect_log_t, move_t, back_move_t, go_to_history_t, game_inf_t, new_game_t, inf_request_t,
                                      opponent_lost_t, incoming_datagramm_t, some_datagramm_t>;

using to_server_messages_t = boost::mpl::vector<some_datagramm_t, message_received_t, is_server_lost_t, hello_server_t, login_t, opponent_inf_t, my_inf_t,
                                                move_t, back_move_t, go_to_history_t, new_game_t>;

using to_client_messages_t = boost::mpl::vector<some_datagramm_t, message_received_t, get_login_t, login_response_t, is_client_lost_t, opponent_lost_t,
                                                inf_request_t, incorrect_log_t, game_inf_t>;

template<typename mpl_vector, typename T>
concept mpl_vector_has_type = !std::same_as<typename boost::mpl::find<mpl_vector, T>::type, typename boost::mpl::end<mpl_vector>::type>;

template<typename T, typename... U>
concept one_of = (std::same_as<T, U> || ...);

template<typename T>
concept one_of_to_server_msgs = mpl_vector_has_type<to_server_messages_t, T>;

template<typename T>
concept one_of_to_client_msgs = mpl_vector_has_type<to_client_messages_t, T>;

template<typename T>
concept one_of_msgs = mpl_vector_has_type<messages_t, T>;

namespace details {

template<one_of_msgs T>
inline T from_string(const std::string& str)
{
  std::stringstream ss;
  ss.str(str);
  boost::archive::text_iarchive ia(ss);
  T msg;
  ia >> msg;
  return msg;
}

template<one_of_msgs T>
inline std::string to_string(const T& msg)
{
  std::stringstream ss;
  boost::archive::text_oarchive oa(ss);
  oa << msg;
  return ss.str();
}

} // namespace details

template<typename T>
inline constexpr std::string_view id_v = msg_type<T>();

template<one_of_msgs T>
T init(const some_datagramm_t& datagramm)
{
  if (datagramm.type != id_v<T>)
  {
    throw my_archive_exception("Wrong type for init! Can not create msg with type=" + std::string(id_v<T>) + "; from type=" + datagramm.type);
  }

  return details::from_string<T>(datagramm.data);
}

template<one_of_msgs T>
inline T init(const std::string& unprocessed_str)
{
  some_datagramm_t _1 = init<some_datagramm_t>(unprocessed_str);
  return init<T>(_1);
}

template<>
inline some_datagramm_t init<some_datagramm_t>(const std::string& str)
{
  return details::from_string<some_datagramm_t>(str);
}

template<one_of_msgs T>
inline std::string prepare_for_send(const T& msg)
{
  return prepare_for_send(some_datagramm_t{details::to_string(msg), id_v<T>});
}

template<>
inline std::string prepare_for_send<some_datagramm_t>(const some_datagramm_t& msg)
{
  return details::to_string(msg);
}

template<one_of_msgs T>
inline bool is_equal_types(const std::string& str)
{
  return init<some_datagramm_t>(str).type == id_v<T>;
}

} // namespace msg
