#include <algorithm>
#include <boost/asio.hpp>
#include <exception>
#include <memory>
#include <string>

#include "common/helper.hpp"
#include "common/logger.hpp"
#include "logic/games_manager.hpp"
#include "server/handle_message.hpp"
#include "server/server.hpp"

int main()
try
{
  logger::logger_t::get().init();

  io_service_t io_service;
  server::server_t server{io_service};
  logic::games_manager_t games_manager{io_service};
  server::handle_message_t handler{games_manager, server};

  while (true)
  {
    io_service.run_one();

    for (const auto& data : server.read())
    {
      handler.process_server_message(data.address, data.message);
    }

    server.process();
  }

  return 0;
}
catch (std::exception const& ex)
{
  SPDLOG_CRITICAL("Exception! ", ex.what());
}
