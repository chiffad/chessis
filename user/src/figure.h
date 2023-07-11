#ifndef __MY_FIGURE_H__BVYHKUVFTYVKUBVYUFTYVKBNJMKL
#define __MY_FIGURE_H__BVYHKUVFTYVKUBVYUFTYVKBNJMKL

#include <QString>

#include "coord.h"


namespace graphic
{

class figure_t
{
public:
  figure_t(const QString& name, const int x, const int y, const bool visible)
      : _name(name), _x(x), _y(y), _visible(visible)
  {
  }

  inline int x() const {return _x;}
  inline int y() const {return _y;}
  inline QString name() const {return _name;}
  inline bool visible() const {return _visible;}

  inline void set_name(const QString& new_name) {_name = new_name;}
  inline void set_visible(const bool new_visible) {_visible = new_visible;}
  inline void set_coord(const Coord& new_coord) {_x = new_coord.x; _y = new_coord.y;}

  bool operator==(const figure_t& rhs)
  {return(name() == rhs.name() && x() == rhs.x() && y() == rhs.y() && visible() == rhs.visible());}

private:
  QString _name;
  int _x;
  int _y;
  bool _visible;
};

} //namespace graphic


#endif // __MY_FIGURE_H__BVYHKUVFTYVKUBVYUFTYVKBNJMKL