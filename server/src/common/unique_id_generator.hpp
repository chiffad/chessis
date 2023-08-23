#pragma once

#include "spdlog/fmt/ostr.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

namespace common {

using uuid_t = boost::uuids::uuid;

class uuid_generator_t
{
public:
  explicit uuid_generator_t();

  uuid_t new_uuid();

private:
  boost::uuids::random_generator generator_;
};

std::ostream& operator<<(std::ostream& os, const uuid_t& id);

} // namespace common
