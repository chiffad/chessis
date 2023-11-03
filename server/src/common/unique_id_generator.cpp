#include "common/unique_id_generator.hpp"
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace chess::common {

uuid_generator_t::uuid_generator_t() = default;

uuid_t uuid_generator_t::new_uuid()
{
  return generator_();
}

msg::token_t to_msg_token(uuid_t uuid)
{
  return msg::token_t{boost::uuids::to_string(uuid)};
}

uuid_t from_msg_token(const msg::token_t& token)
{
  return boost::lexical_cast<uuid_t>(token.uuid);
}

} // namespace chess::common
