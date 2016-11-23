#ifndef __MY_CUBE_RENDERER_H__HUIOGTYFYIKUBHLNJMOKISUIDGATYWVDOLA
#define __MY_CUBE_RENDERER_H__HUIOGTYFYIKUBHLNJMOKISUIDGATYWVDOLA

#include <QString>
#include <memory>


namespace graphic
{

class Cube_renderer
{
public:
  Cube_renderer();
  ~Cube_renderer();

  void render();
  void initialize();
  void set_cube_updates(const QString &fig_type, const int tilt_angle, const float scale);

public:
  Cube_renderer(const Cube_renderer&) = delete;
  Cube_renderer& operator=(const Cube_renderer&) = delete;

private:
  struct impl_t;
  std::unique_ptr<impl_t> impl;
};

} // namespace graphic

#endif //__MY_CUBE_RENDERER_H__HUIOGTYFYIKUBHLNJMOKISUIDGATYWVDOLA
