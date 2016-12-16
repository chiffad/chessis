#include "fb_obj.h"

#include <QtGui/QOpenGLFramebufferObject>
#include <QtQuick/QQuickWindow>
#include <qsgsimpletexturenode.h>

#include "cube_renderer.h"


using namespace graphic;

class fbo_renderer_t : public QQuickFramebufferObject::Renderer
{
public:
  fbo_renderer_t();
  ~fbo_renderer_t();
  QOpenGLFramebufferObject* createFramebufferObject(const QSize& size) Q_DECL_OVERRIDE;
  void synchronize(QQuickFramebufferObject *item) Q_DECL_OVERRIDE;
  void render() Q_DECL_OVERRIDE;

private:
  cube_renderer_t *cube;
};


fb_obj_t::fb_obj_t()
    : fig_type("board"), tilt_angle(0), cube_scale(1)
{
}

fb_obj_t::~fb_obj_t()
{
}

QQuickFramebufferObject::Renderer* fb_obj_t::createRenderer() const
{
  return new fbo_renderer_t();
}

QString fb_obj_t::get_fig_type() const
{
  return fig_type;
}

void fb_obj_t::set_fig_type(const QString& name)
{
  fig_type = name;
  update();
  emit fig_type_changed();
}

int fb_obj_t::get_tilt_angle() const
{
  return tilt_angle;
}

void fb_obj_t::set_tilt_angle(const int angl)
{
  tilt_angle = angl;
  update();
  emit tilt_angle_changed();
}

float fb_obj_t::get_cube_scale() const
{
  return cube_scale;
}

void fb_obj_t::set_cube_scale(const float scale)
{
  cube_scale = scale;
  update();
  emit cube_scale_changed();
}

//======================================================

fbo_renderer_t::fbo_renderer_t() : cube(new cube_renderer_t())
{
  cube->initialize();
  update();
}

fbo_renderer_t::~fbo_renderer_t()
{
  delete cube;
}

QOpenGLFramebufferObject* fbo_renderer_t::createFramebufferObject(const QSize& size)
{
  QOpenGLFramebufferObjectFormat format;
  format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
  format.setSamples(4);
  return new QOpenGLFramebufferObject(size, format);
}

void fbo_renderer_t::render()
{
  cube->render();
  update();
}

void fbo_renderer_t::synchronize(QQuickFramebufferObject *item)
{
  fb_obj_t *fb_item = static_cast<fb_obj_t *>(item);
  cube->set_cube_updates(fb_item->get_fig_type(), fb_item->get_tilt_angle(), fb_item->get_cube_scale());
}
