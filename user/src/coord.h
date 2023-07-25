#pragma once

struct coord_t
{
  coord_t(const unsigned x = 0, const unsigned y = 0)
    : x(x)
    , y(y)
  {}

  bool operator==(const coord_t& rhs) const { return (x == rhs.x && y == rhs.y); }

  unsigned x;
  unsigned y;
};
