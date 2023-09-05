#include "common/unique_id_generator.hpp"

namespace common {

uuid_generator_t::uuid_generator_t() = default;

uuid_t uuid_generator_t::new_uuid()
{
  return generator_();
}

std::ostream& operator<<(std::ostream& os, const uuid_t& id)
{
  return os << boost::uuids::to_string(id);
}

} // namespace common
