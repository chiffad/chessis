#pragma once

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/mpl/find.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/serialization/string.hpp>
#include <concepts>
#include <string>
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

struct incoming_datagramm_t
{
  std::string data;
  int ser_num{};
};

struct some_datagramm_t
{
  std::string data;
  int type{};
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

#define struct_proto(name)                                                                                                                                     \
  struct name                                                                                                                                                  \
  {                                                                                                                                                            \
  private:                                                                                                                                                     \
    friend class boost::serialization::access;                                                                                                                 \
    template<typename Archive>                                                                                                                                 \
    void serialize(Archive& /*ar*/, const unsigned /*version*/)                                                                                                \
    {}                                                                                                                                                         \
  };
struct_proto(hello_server_t);
struct_proto(message_received_t);
struct_proto(is_server_lost_t);
struct_proto(is_client_lost_t);
struct_proto(opponent_inf_t);
struct_proto(my_inf_t);
struct_proto(get_login_t);
struct_proto(incorrect_log_t);
struct_proto(back_move_t);
struct_proto(new_game_t);
struct_proto(server_lost_t);
struct_proto(server_here_t);
struct_proto(client_lost_t);
struct_proto(opponent_lost_t);
#undef struct_proto

using message_types = boost::mpl::vector<hello_server_t, message_received_t, is_server_lost_t, is_client_lost_t, opponent_inf_t, my_inf_t, get_login_t, login_t,
                                         incorrect_log_t, move_t, back_move_t, go_to_history_t, game_inf_t, new_game_t, inf_request_t, server_lost_t,
                                         server_here_t, client_lost_t, opponent_lost_t, incoming_datagramm_t, some_datagramm_t>;

template<typename T, typename... U>
concept one_of = (std::same_as<T, U> || ...);

template<typename T>
concept one_of_msg_types = one_of<T, hello_server_t, message_received_t, is_server_lost_t, is_client_lost_t, opponent_inf_t, my_inf_t, get_login_t, login_t,
                                  incorrect_log_t, move_t, back_move_t, go_to_history_t, game_inf_t, new_game_t, inf_request_t, server_lost_t, server_here_t,
                                  client_lost_t, opponent_lost_t, incoming_datagramm_t, some_datagramm_t>;

template<typename T>
inline constexpr int id_v = boost::mpl::find<message_types, T>::type::pos::value;

namespace details {

template<one_of_msg_types T>
inline T from_string(const std::string& str)
{
  std::stringstream ss;
  ss.str(str);
  boost::archive::text_iarchive ia(ss);
  T msg;
  ia >> msg;
  return msg;
}

template<one_of_msg_types T>
inline std::string to_string(const T& msg)
{
  std::stringstream ss;
  boost::archive::text_oarchive oa(ss);
  oa << msg;
  return ss.str();
}

} // namespace details

template<one_of_msg_types T>
T init(const some_datagramm_t& datagramm)
{
  if (datagramm.type != id_v<T>)
  {
    throw my_archive_exception("Wrong type for init! Can not create msg with type=" + std::to_string(id_v<T>) +
                               "; from type=" + std::to_string(datagramm.type));
  }

  return details::from_string<T>(datagramm.data);
}

template<one_of_msg_types T>
inline T init(const std::string& str)
{
  some_datagramm_t _1 = init<some_datagramm_t>(str);
  return init<T>(_1);
}

template<>
inline some_datagramm_t init<some_datagramm_t>(const std::string& str)
{
  return details::from_string<some_datagramm_t>(str);
}

template<one_of_msg_types T>
inline std::string prepare_for_send(const T& msg)
{
  return prepare_for_send(some_datagramm_t{details::to_string(msg), id_v<T>});
}

template<>
inline std::string prepare_for_send<some_datagramm_t>(const some_datagramm_t& msg)
{
  return details::to_string(msg);
}

template<one_of_msg_types T>
inline bool is_equal_types(const std::string& str)
{
  return init<some_datagramm_t>(str).type == id_v<T>;
}

} // namespace msg
