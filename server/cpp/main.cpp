#include <string>
#include <memory>
#include <exception>
#include <algorithm>
#include <boost/asio.hpp>

#include "server.h"
#include "messages.h"
#include "helper.h"
#include "handle_message.h"


int main() try
{
  boost::asio::io_service io_service;
  sr::server_t server(io_service);
  sr::handle_message_t handler;

  while(true)
  {
    io_service.run_one();

    for(auto data : server.pull())
      { handler.new_message(io_service, data.address, data.message); }

    for(auto c : handler)
    {
      if(c->is_message_for_server_append())
        { server.send(c->pull_for_server().data(), c->get_address()); }

      if(c->is_message_for_logic_append())
      {
        const auto message = c->pull_for_logic();
        sr::helper::log("message_from_logic: ", message);
        const auto type = msg::get_msg_type(message);

        switch(type)
        {
          //case msg::message_received_t: //in client
          //case msg::is_server_lost_t: // no need cause on this message already was sended responce
          case msg::id<msg::login_t       >(): handler.handle<msg::login_t       >(message, c); break;
          case msg::id<msg::opponent_inf_t>(): handler.handle<msg::opponent_inf_t>(message, c); break;
          case msg::id<msg::my_inf_t      >(): handler.handle<msg::my_inf_t      >(message, c); break;
          case msg::id<msg::client_lost_t >(): handler.handle<msg::client_lost_t >(message, c); break;
          default:
          {
            switch(type)
            {
              case msg::id<msg::move_t         >(): handler.handle<msg::move_t         >(message, c); break;
              case msg::id<msg::back_move_t    >(): handler.handle<msg::back_move_t    >(message, c); break;
              case msg::id<msg::go_to_history_t>(): handler.handle<msg::go_to_history_t>(message, c); break;
              case msg::id<msg::new_game_t     >(): handler.handle<msg::back_move_t    >(message, c); break;
              default:
                sr::helper::throw_except("Unknown message type!: ", type);
            }
            
            handler.board_updated(c);
          }
        }
      }
    }
  }
  
  return 0;
}

catch(std::exception const& ex)
{
  std::cout<<"Exception! "<<ex.what()<<std::endl;
}
