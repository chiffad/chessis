#pragma once

#include "spdlog/fmt/ostr.h"

struct coord_t
{
  coord_t(const unsigned x = 0, const unsigned y = 0)
    : x(x)
    , y(y)
  {}

  inline bool operator==(const coord_t& rhs) const { return (x == rhs.x && y == rhs.y); }

  unsigned x;
  unsigned y;
};

inline std::ostream& operator<<(std::ostream& os, const coord_t& c)
{
  return os << "[" << c.x << "; " << c.y << "]";
}
