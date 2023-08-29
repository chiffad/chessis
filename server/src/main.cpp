#include "common/helper.hpp"
#include "common/logger.hpp"
#include "logic/games_manager.hpp"
#include "logic/message_handler.hpp"
#include "server/server.hpp"

#include <boost/asio.hpp>
#include <exception>
#include <spdlog/spdlog.h>

int main()
try
{
  logger::logger_t::get().init();

  io_service_t io_service;
  server::server_t server{io_service};
  logic::games_manager_t games_manager{io_service};
  logic::message_handler_t handler{games_manager, server};

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
catch (const std::exception& ex)
{
  SPDLOG_CRITICAL("Exception={}", ex.what());
}
