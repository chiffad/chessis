#include "common/helper.hpp"
#include "common/logger.hpp"
#include "logic/games_manager.hpp"
#include "logic/message_handler.hpp"
#include "server/server.hpp"
#include "user/user_status_monitor.hpp"
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
  chess::server::user::user_status_monitor_t user_status_monitor;

  chess::server::server_t server{io_service, users_data_manager, user_status_monitor};
  chess::logic::games_manager_t games_manager{io_service};
  chess::logic::message_handler_t handler{games_manager, server, users_data_manager};

  const auto user_status_changed_connection = user_status_monitor.connect_user_status_changed(
    [&](const chess::client_uuid_t& uuid, bool online) { io_service.post([uuid, online, &handler]() { handler.user_connection_changed(std::move(uuid), online); }); });
  while (true)
  {
    io_service.run_one();

    for (auto& [client_uuid, msgs] : server.read())
    {
      for (auto& msg : msgs)
      {
        handler.process_server_message(client_uuid, std::move(msg));
      }
    }

    server.process();
  }

  user_status_changed_connection.disconnect();

  return 0;
}
catch (const std::exception& ex)
{
  SPDLOG_CRITICAL("Exception={}", ex.what());
}
