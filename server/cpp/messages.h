#ifndef __MY_ENUM_H__UILGBAWLIDBAWYGDTAGFDWTYAD
#define __MY_ENUM_H__UILGBAWLIDBAWYGDTAGFDWTYAD

#define BOOST_MPL_LIMIT_VECTOR_SIZE 30
#define BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS

#include <string>
#include <vector>
#include <boost/serialization/string.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/find.hpp>

#include "helper.h"


namespace msg
{
  struct my_archive_exception: public boost::archive::archive_exception
  {
    my_archive_exception(const std::string& ex) : boost::archive::archive_exception(boost::archive::archive_exception::input_stream_error), message(ex)
    {}
    virtual const char* what() const throw()
      { return message.data(); }
    std::string message;
  };

  struct login_t
  {
    login_t() = default;
    login_t(const std::string& log, const std::string& password) : login(log), pwd(password)
    {}

    std::string login;
    std::string pwd;
  };

  struct move_t
  {
    move_t() = default;
    move_t(const std::string move) : data(move)
    {}

    std::string data;
  };

  struct go_to_history_t
  {
    go_to_history_t() = default;
    go_to_history_t(const int ind) : index(ind)
    {}

    int index;
  };

  struct inf_request_t
  {
    inf_request_t() = default;
    inf_request_t(const std::string& str)
        : data(str)
    {}

    std::string data;
  };

  struct game_inf_t
  {
    game_inf_t() = default;
    game_inf_t(const std::string& b_mask, const std::string& m_hist, const bool mate, const int move_number)
        : board_mask(b_mask), moves_history(m_hist), is_mate(mate), move_num(move_number)
    {}

    std::string board_mask;
    std::string moves_history;
    bool is_mate;
    int move_num;
  };
  
  struct incoming_datagramm_t
  {
    incoming_datagramm_t() = default;
    incoming_datagramm_t(const std::string& mess, const int num) : ser_num(num), data(mess)
    {}

    int ser_num;
    std::string data;
  };
  
  struct some_datagramm_t
  {
    some_datagramm_t() = default;
    some_datagramm_t(const std::string& mess, const int mess_type) : type(mess_type), data(mess)
    {}

    int type;
    std::string data;
  };

  template <typename Archive>
  void serialize(Archive& ar, game_inf_t& _1, const unsigned /*version*/)
  {
    ar & _1.board_mask;
    ar & _1.moves_history;
    ar & _1.is_mate;
    ar & _1.move_num;
  }
  
  template <typename Archive>
  void serialize(Archive& ar, inf_request_t& _1, const unsigned /*version*/)
  { ar & _1.data; }
  
  template <typename Archive>
  void serialize(Archive& ar, go_to_history_t _1, const unsigned /*version*/)
  { ar & _1.index; }
  
  template <typename Archive>
  void serialize(Archive& ar, move_t& _1, const unsigned /*version*/)
  { ar & _1.data; }

  template <typename Archive>
  void serialize(Archive& ar, login_t& _1, const unsigned /*version*/)
  {
    ar & _1.login;
    ar & _1.pwd;
  }
  
  template<typename Archive>
  void serialize(Archive& ar, incoming_datagramm_t& _1, const unsigned /*version*/)
  {
    ar & _1.ser_num;
    ar & _1.data;
  }
  
  template<typename Archive>
  void serialize(Archive& ar, some_datagramm_t& _1, const unsigned /*version*/)
  {
    ar & _1.type;
    ar & _1.data;
  }

  #define struct_proto(name)  struct name { private: friend class boost::serialization::access; template <typename Archive> void serialize(Archive &/*ar*/, const unsigned /*version*/){} };
  struct_proto(hello_server_t    );
  struct_proto(message_received_t);
  struct_proto(is_server_lost_t  );
  struct_proto(is_client_lost_t  );
  struct_proto(opponent_inf_t    );
  struct_proto(my_inf_t          );
  struct_proto(get_login_t       );
  struct_proto(incorrect_log_t   );
  struct_proto(back_move_t       );
  struct_proto(new_game_t        );
  struct_proto(server_lost_t     );
  struct_proto(server_here_t     );
  struct_proto(client_lost_t     );
  struct_proto(opponent_lost_t   );
  #undef struct_proto
  
  typedef boost::mpl::vector<hello_server_t, message_received_t, is_server_lost_t, is_client_lost_t, opponent_inf_t,
                             my_inf_t, get_login_t, login_t, incorrect_log_t, move_t, back_move_t, go_to_history_t,
                             game_inf_t, new_game_t, inf_request_t, server_lost_t, server_here_t, client_lost_t, opponent_lost_t,
                             incoming_datagramm_t, some_datagramm_t>
           message_types;

  template<typename m_type>
  struct id
  {
    enum { value = boost::mpl::find<message_types, m_type>::type::pos::value };
    constexpr operator int()
    {return value;}
  };
    
  template<typename struct_t>
  void update_struct(struct_t& s, const std::string& str)
  {
    std::stringstream ss;
    ss.str(str);
    boost::archive::text_iarchive ia(ss);
    ia>>s;
  }
  
  template<typename struct_t>
  struct_t init(const std::string& str)
  {
    some_datagramm_t _1;
    update_struct(_1, str);

    if(id<struct_t>() == id<some_datagramm_t>())
      { _1.data = str; }

    else if(_1.type != id<struct_t>())
      { throw my_archive_exception("wrong struct_t type for init!!"); }

    struct_t my_struct;
    update_struct(my_struct, _1.data);

    return my_struct;
  }

  template<typename struct_t>
  std::string prepare_for_send(const struct_t& s)
  {
    std::stringstream ss;
    boost::archive::text_oarchive oa(ss);
    oa<<s;

    if(id<struct_t>() != id<some_datagramm_t>())
    {
      some_datagramm_t _1(ss.str(), id<struct_t>::value);
      return prepare_for_send(_1);
    }

    return ss.str();
  }

  template<typename struct_t>
  bool is_equal_types(const std::string& str)
  { return init<some_datagramm_t>(str).type == id<struct_t>(); }

}// namespace msg


#undef BOOST_MPL_LIMIT_VECTOR_SIZE
#undef BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS

#endif // __MY_ENUM_H__UILGBAWLIDBAWYGDTAGFDWTYAD
