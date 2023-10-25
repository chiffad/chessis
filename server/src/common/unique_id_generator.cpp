#include "common/unique_id_generator.hpp"

namespace chess::common {

uuid_generator_t::uuid_generator_t() = default;

uuid_t uuid_generator_t::new_uuid()
{
  return generator_();
}

} // namespace chess::common
