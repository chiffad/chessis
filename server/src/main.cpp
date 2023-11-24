#include "common/helper.hpp"
#include "common/logger.hpp"
#include "logic/games_manager.hpp"
#include "logic/message_handler.hpp"
#include "server/server.hpp"
#include "user/users_data_manager.hpp"

#include <boost/asio.hpp>
#include <exception>
#include <spdlog/spdlog.h>

int main()
try
{
  chess::logger::logger_t::get().init();

  chess::io_service_t io_service;
  chess::server::user::users_data_manager_t users_data_manager;

  std::unique_ptr<chess::logic::message_handler_t> handler;
  chess::server::server_t server{io_service, users_data_manager, [&](const chess::client_uuid_t& uuid, bool online) {
                                   io_service.post([uuid, online, &handler]() { handler->user_connection_changed(std::move(uuid), online); });
                                 }};

  chess::logic::games_manager_t games_manager{io_service};
  handler = std::make_unique<chess::logic::message_handler_t>(games_manager, server, users_data_manager);

  while (true)
  {
    io_service.run_one();

    for (auto& [client_uuid, msgs] : server.read())
    {
      for (auto& msg : msgs)
      {
        handler->process_server_message(client_uuid, std::move(msg));
      }
    }

    server.process();
  }

  return 0;
}
catch (const std::exception& ex)
{
  SPDLOG_CRITICAL("Exception={}", ex.what());
}
