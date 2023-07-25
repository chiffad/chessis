#pragma once

#include <QString>

#include "coord.h"

class figure_t
{
public:
  explicit figure_t() = default;

  figure_t(const QString& name, const coord_t& coord, const bool visible)
    : name_{name}
    , coord_{coord}
    , visible_{visible}
  {}

  inline const coord_t& coord() const { return coord_; }
  inline QString name() const { return name_; }
  inline bool visible() const { return visible_; }

  inline void set_name(const QString& new_name) { name_ = new_name; }
  inline void set_visible(const bool new_visible) { visible_ = new_visible; }
  inline void set_coord(const coord_t& new_coord) { coord_ = new_coord; }

  bool operator==(const figure_t& rhs) { return (name() == rhs.name() && coord() == rhs.coord() && visible() == rhs.visible()); }

private:
  QString name_{};
  coord_t coord_{};
  bool visible_{};
};
