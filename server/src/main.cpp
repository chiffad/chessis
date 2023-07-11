#include <algorithm>
#include <boost/asio.hpp>
#include <exception>
#include <memory>
#include <string>

#include "handle_message.h"
#include "helper.h"
#include "server.h"

int main()
try
{
  boost::asio::io_service io_service;
  sr::server_t server(io_service);
  sr::handle_message_t handler;

  while (true)
  {
    io_service.run_one();

    for (auto data : server.pull())
    {
      handler.new_message(io_service, data.address, data.message);
    }

    for (auto c : handler)
    {
      if (c->is_message_for_server_append())
      {
        server.send(c->pull_for_server().data(), c->get_address());
      }

      if (c->is_message_for_logic_append())
      {
        const auto message = c->pull_for_logic();
        sr::helper::log("message_from_logic: ", message);

        handler.handle(message, c);
      }
    }
  }

  return 0;
}

catch (std::exception const& ex)
{
  std::cout << "Exception! " << ex.what() << std::endl;
}
