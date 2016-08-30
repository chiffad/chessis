#include <QtGui/QOpenGLFramebufferObject>
#include <QtQuick/QQuickWindow>
#include <qsgsimpletexturenode.h>
#include "fb_obj.h"
#include "cube_renderer.h"

Fb_obj::Fb_obj() : m_fig_type("board"), m_tilt_angle(0), m_cube_scale(1)
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
  return m_fig_type;
}

void Fb_obj::set_fig_type(const QString &name)
{
  m_fig_type = name;
  update();
  emit fig_type_changed();
}

int Fb_obj::get_tilt_angle() const
{
  return m_tilt_angle;
}

void Fb_obj::set_tilt_angle(const int angl)
{
  m_tilt_angle = angl;
  update();
  emit tilt_angle_changed();
}

float Fb_obj::get_cube_scale() const
{
  return m_cube_scale;
}

void Fb_obj::set_cube_scale(const float scale)
{
  m_cube_scale = scale;
  update();
  emit cube_scale_changed();
}

//======================================================

Fbo_renderer::Fbo_renderer() : m_cube(new Cube_renderer())
{
  m_cube->initialize();
  update();
}

Fbo_renderer::~Fbo_renderer()
{
  delete m_cube;
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
  m_cube->render();
  update();
}

void Fbo_renderer::synchronize(QQuickFramebufferObject *item)
{
  Fb_obj *fb_item = static_cast<Fb_obj *>(item);
  m_cube->set_cube_updates(fb_item->get_fig_type(), fb_item->get_tilt_angle(), fb_item->get_cube_scale());
}