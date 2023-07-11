#ifndef __MY_CUBE_RENDERER_H__HUIOGTYFYIKUBHLNJMOKISUIDGATYWVDOLA
#define __MY_CUBE_RENDERER_H__HUIOGTYFYIKUBHLNJMOKISUIDGATYWVDOLA

#include <QString>
#include <memory>

namespace graphic {

class cube_renderer_t
{
public:
  cube_renderer_t();
  ~cube_renderer_t();

  void render();
  void initialize();
  void set_cube_updates(const QString& fig_type, const int tilt_angle, const float scale);

public:
  cube_renderer_t(const cube_renderer_t&) = delete;
  cube_renderer_t& operator=(const cube_renderer_t&) = delete;

private:
  struct impl_t;
  std::unique_ptr<impl_t> impl;
};

} // namespace graphic

#endif //__MY_CUBE_RENDERER_H__HUIOGTYFYIKUBHLNJMOKISUIDGATYWVDOLA
