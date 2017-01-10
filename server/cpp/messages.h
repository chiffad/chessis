#ifndef __MY_ENUM_H__UILGBAWLIDBAWYGDTAGFDWTYAD
#define __MY_ENUM_H__UILGBAWLIDBAWYGDTAGFDWTYAD

#include <string>
#include <vector>
#include <memory>
#include <typeinfo>
#include <boost/serialization/string.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/find.hpp>

#include "client.h"
#include "desk.h"
#include "helper.h"


namespace msg
{
  int get_msg_type(const std::string& message);

  struct login_t
  {
    std::string login;
//    std::string pwd;
  };

  struct move_t
  {
    std::string data;
  };

  struct go_to_history_t
  {
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
  
  #define struct_proto(name)   struct name { private: friend class boost::serialization::access; template <typename Archive> void serialize(Archive &/*ar*/, const unsigned /*version*/){} };
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
                             new_game_t, inf_request_t, server_lost_t, server_here_t, client_lost_t, opponent_lost_t, game_inf_t>
           message_types;

  template<typename m_type>
  struct id
  {
    enum { value = boost::mpl::find<message_types, m_type>::type::pos::value };
    constexpr operator int()
    {return value;}
  };
    
  template<typename struct_t>
  struct_t init(const std::string& str)
  {
    struct_t my_struct;
    update_struct(my_struct, str);
    
    return my_struct;
  }
  
  template<typename struct_t>
  void update_struct(struct_t& s, const std::string& str)
  {
    std::stringstream ss;
    ss.str(str);
    boost::archive::text_iarchive ia(ss);
    ia >> s;
  }
  
  template<typename struct_t>
  std::string prepare_for_send(const struct_t& s)
  {
    std::stringstream ss;
    boost::archive::text_oarchive oa(ss);
    oa<<id<struct_t>::value;
    oa<<s;
    
    return ss.str();
  }
  
  template<typename struct_t>
  bool is_equal_types(const std::string& str)
    { return (get_msg_type(str) == id<struct_t>()); }
  
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
//    ar & _1.pwd;
  }
  
  template<typename Archive>
  void serialize(Archive& ar, int& type, const unsigned /*version*/)
    { ar & type; }
  

  class handle_message_t
  {
  public:
    handle_message_t() = default;
    template<typename T>
    void handle(const std::string& str, std::shared_ptr<sr::client_t>& client)
      { std::cout<<"For type "<< typeid(T).name()<<" tactic isn't defined!"<<str<<std::endl; }
    void board_updated(std::shared_ptr<sr::client_t>& client);
    void new_message(boost::asio::io_service& io_service, const boost::asio::ip::udp::endpoint& addr, const std::string& message);
    
    std::vector<std::shared_ptr<sr::client_t>>::iterator begin() noexcept;
    std::vector<std::shared_ptr<sr::client_t>>::iterator end() noexcept;
    
  public:
    handle_message_t(const handle_message_t&) = delete;
    handle_message_t& operator=(const handle_message_t&) = delete;
    
    
  private:
    std::vector<std::shared_ptr<logic::desk_t>>::iterator get_desk(const std::shared_ptr<sr::client_t>& client);
    std::vector<std::shared_ptr<sr::client_t>>::iterator get_opponent(const std::shared_ptr<sr::client_t>& client);
    
    std::vector<std::shared_ptr<sr::client_t>> clients;
    std::vector<std::shared_ptr<logic::desk_t>> desks;
  };
  
  #define handle_macro(struct_type)  template<> void handle_message_t::handle<struct_type>(const std::string& str, std::shared_ptr<sr::client_t>& client);
  handle_macro(login_t        );
  handle_macro(opponent_inf_t );
  handle_macro(my_inf_t       );
  handle_macro(client_lost_t  );
  handle_macro(move_t         );
  handle_macro(back_move_t    );
  handle_macro(go_to_history_t);
  handle_macro(new_game_t     );
  #undef handle_macro
  
}

#endif // __MY_ENUM_H__UILGBAWLIDBAWYGDTAGFDWTYAD
