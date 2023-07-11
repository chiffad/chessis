#pragma once

#include <QString>

#include "coord.h"

namespace graphic {

class figure_t
{
public:
  figure_t(const QString& name, const int x, const int y, const bool visible)
    : name_(name)
    , x_(x)
    , y_(y)
    , visible_(visible)
  {}

  inline int x() const { return x_; }
  inline int y() const { return y_; }
  inline QString name() const { return name_; }
  inline bool visible() const { return visible_; }

  inline void set_name(const QString& new_name) { name_ = new_name; }
  inline void set_visible(const bool new_visible) { visible_ = new_visible; }
  inline void set_coord(const Coord& new_coord)
  {
    x_ = new_coord.x;
    y_ = new_coord.y;
  }

  bool operator==(const figure_t& rhs) { return (name() == rhs.name() && x() == rhs.x() && y() == rhs.y() && visible() == rhs.visible()); }

private:
  QString name_;
  int x_;
  int y_;
  bool visible_;
};

} // namespace graphic
