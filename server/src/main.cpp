#include <algorithm>
#include <boost/asio.hpp>
#include <exception>
#include <memory>
#include <string>

#include "common/helper.hpp"
#include "common/logger.hpp"
#include "logic/boards_holder.hpp"
#include "server/handle_message.hpp"
#include "server/server.hpp"

int main()
try
{
  logger::logger_t::get().init();

  boost::asio::io_service io_service;
  server::server_t server{io_service};
  server::clients_holder_t clients{io_service};
  logic::boards_holder_t boards_holder;

  server::handle_message_t handler{clients, boards_holder};

  while (true)
  {
    io_service.run_one();

    for (auto data : server.pull())
    {
      handler.new_message(io_service, data.address, data.message);
    }

    for (auto c : clients)
    {
      if (c->is_message_for_server_append())
      {
        server.send(c->pull_for_server().data(), c->get_address());
      }

      if (c->is_message_for_logic_append())
      {
        const auto message = c->pull_for_logic();
        SPDLOG_TRACE("message_from_logic={}", message);
        handler.handle(message, c);
      }
    }
  }

  return 0;
}
catch (std::exception const& ex)
{
  SPDLOG_CRITICAL("Exception! ", ex.what());
}
