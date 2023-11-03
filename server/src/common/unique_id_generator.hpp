#pragma once

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <messages/messages.hpp>

namespace chess::common {

using uuid_t = boost::uuids::uuid;

class uuid_generator_t
{
public:
  explicit uuid_generator_t();

  uuid_t new_uuid();

private:
  boost::uuids::random_generator generator_;
};

msg::token_t to_msg_token(uuid_t);
uuid_t from_msg_token(const msg::token_t& token);

} // namespace chess::common
