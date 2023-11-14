#pragma once

#include "common/helper.hpp"
#include "messages/messages.hpp"

namespace chess::server::user_data {

class session_token_generator_t
{
public:
  inline msg::token_t gen(const client_uuid_t& uuid) const { return msg::token_t{boost::uuids::to_string(uuid)}; }
};

} // namespace chess::server::user_data