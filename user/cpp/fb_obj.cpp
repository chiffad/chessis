#include "cube_renderer.h"


#include <QtGui/QOpenGLFramebufferObject>
#include <QtQuick/QQuickWindow>

#include <qsgsimpletexturenode.h>
#include "fb_obj.h"


using namespace graphic;

class Fbo_renderer : public QQuickFramebufferObject::Renderer
{
public:
  Fbo_renderer();
  ~Fbo_renderer();
  QOpenGLFramebufferObject* createFramebufferObject(const QSize &size) Q_DECL_OVERRIDE;
  void synchronize(QQuickFramebufferObject *item) Q_DECL_OVERRIDE;
  void render() Q_DECL_OVERRIDE;

private:
  Cube_renderer *cube;
};


Fb_obj::Fb_obj()
    : fig_type("board"), tilt_angle(0), cube_scale(1)
{
}

Fb_obj::~Fb_obj()
{
}

QQuickFramebufferObject::Renderer* Fb_obj::createRenderer() const
{
  return new Fbo_renderer();
}

QString Fb_obj::get_fig_type() const
{
  return fig_type;
}

void Fb_obj::set_fig_type(const QString &name)
{
  fig_type = name;
  update();
  emit fig_type_changed();
}

int Fb_obj::get_tilt_angle() const
{
  return tilt_angle;
}

void Fb_obj::set_tilt_angle(const int angl)
{
  tilt_angle = angl;
  update();
  emit tilt_angle_changed();
}

float Fb_obj::get_cube_scale() const
{
  return cube_scale;
}

void Fb_obj::set_cube_scale(const float scale)
{
  cube_scale = scale;
  update();
  emit cube_scale_changed();
}

//======================================================

Fbo_renderer::Fbo_renderer() : cube(new Cube_renderer())
{
  cube->initialize();
  update();
}

Fbo_renderer::~Fbo_renderer()
{
  delete cube;
}

QOpenGLFramebufferObject* Fbo_renderer::createFramebufferObject(const QSize &size)
{
  QOpenGLFramebufferObjectFormat format;
  format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
  format.setSamples(4);
  return new QOpenGLFramebufferObject(size, format);
}

void Fbo_renderer::render()
{
  cube->render();
  update();
}

void Fbo_renderer::synchronize(QQuickFramebufferObject *item)
{
  Fb_obj *fb_item = static_cast<Fb_obj *>(item);
  cube->set_cube_updates(fb_item->get_fig_type(), fb_item->get_tilt_angle(), fb_item->get_cube_scale());
}
