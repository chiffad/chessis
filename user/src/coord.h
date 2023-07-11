#pragma once

namespace graphic {

struct Coord
{
  Coord(const unsigned X = 0, const unsigned Y = 0)
    : x(X)
    , y(Y)
  {}

  bool operator==(const Coord& rhs) const { return (x == rhs.x && y == rhs.y); }

  unsigned x;
  unsigned y;
};

} // namespace graphic
