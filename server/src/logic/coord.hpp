#pragma once
namespace logic {

struct coord_t
{
  coord_t(const unsigned X = 0, const unsigned Y = 0)
    : x(X)
    , y(Y)
  {}

  bool operator==(const coord_t& rhs) const { return (x == rhs.x && y == rhs.y); }

  unsigned x;
  unsigned y;
};

} // namespace logic
