#ifndef __MY_HANDLE_MESSAGE_T__JAUIBDUYBGWADNAOIWDJIWD
#define __MY_HANDLE_MESSAGE_T__JAUIBDUYBGWADNAOIWDJIWD

#include <string>
#include <vector>
#include <memory>
#include <typeinfo>
#include <boost/asio.hpp>

#include "client.h"
#include "desk.h"
#include "messages.h"


namespace sr
{
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
  handle_macro(msg::login_t        );
  handle_macro(msg::opponent_inf_t );
  handle_macro(msg::my_inf_t       );
  handle_macro(msg::client_lost_t  );
  handle_macro(msg::move_t         );
  handle_macro(msg::back_move_t    );
  handle_macro(msg::go_to_history_t);
  handle_macro(msg::new_game_t     );
  #undef handle_macro

}//namespace sr

#endif //__MY_HANDLE_MESSAGE_T__JAUIBDUYBGWADNAOIWDJIWD