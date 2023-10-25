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
  chess::logger::logger_t::get().init();

  chess::io_service_t io_service;
  std::unique_ptr<chess::logic::message_handler_t> handler;
  chess::server::server_t server{io_service, [&](const chess::server::logic::client_t& cl, const bool online) { handler->client_connection_changed(cl.address(), online); },
                                 [](chess::server::authentication::server_t::client_uuid_t uuid) {}};

  chess::logic::games_manager_t games_manager{io_service};
  handler = std::make_unique<chess::logic::message_handler_t>(games_manager, server);

  while (true)
  {
    io_service.run_one();

    for (const auto& data : server.read())
    {
      handler->process_server_message(data.address, data.message);
    }

    server.process();
  }

  return 0;
}
catch (const std::exception& ex)
{
  SPDLOG_CRITICAL("Exception={}", ex.what());
}
