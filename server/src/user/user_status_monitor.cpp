#include "user/user_status_monitor.hpp"

namespace chess::server::user {

boost::signals2::connection user_status_monitor_t::connect_user_status_changed(const user_status_changed_t::slot_type& subscriber)
{
  return user_status_changed_signal_.connect(subscriber);
}

bool user_status_monitor_t::status(const client_uuid_t& uuid)
try
{
  return uuid_to_status_map_.at(uuid);
}
catch (...)
{
  return false;
}

void user_status_monitor_t::update_status(const client_uuid_t& uuid, const bool status)
{
  auto status_it = uuid_to_status_map_.find(uuid);
  if (status_it == uuid_to_status_map_.end())
  {
    uuid_to_status_map_[uuid] = status;
    user_status_changed_signal_(uuid, status);
    return;
  }

  if (status_it->second != status)
  {
    status_it->second = status;
    user_status_changed_signal_(uuid, status);
  }
}

} // namespace chess::server::user