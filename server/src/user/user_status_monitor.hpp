#pragma once

#include "common/helper.hpp"
#include <boost/signals2.hpp>

namespace chess::server::user {

class user_status_monitor_t
{
public:
  using user_status_changed_t = boost::signals2::signal<void(const client_uuid_t&, bool /*online*/)>;

  boost::signals2::connection connect_user_status_changed(const user_status_changed_t::slot_type& subscriber);
  bool status(const client_uuid_t& uuid);
  void update_status(const client_uuid_t& uuid, bool status);

private:
  std::map<client_uuid_t, bool> uuid_to_status_map_;
  user_status_changed_t user_status_changed_signal_;
};

} // namespace chess::server::user