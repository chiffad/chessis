#include <QtGui/QOpenGLFramebufferObject>
#include <QtQuick/QQuickWindow>
#include <qsgsimpletexturenode.h>
#include "headers/fb_obj.h"
#include "headers/cube_renderer.h"

Fb_obj::Fb_obj() : m_fig_type("board"), m_tilt_angle(0)
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
}

int Fb_obj::get_tilt_angle() const
{
  return m_tilt_angle;
}

void Fb_obj::set_tilt_angle(const int angl)
{
  m_tilt_angle = angl;
  emit tilt_angle_changed();
}

//======================================================

Fbo_renderer::Fbo_renderer() : m_fig_type("board"),  m_tilt_angle(0), m_cube(new Cube_renderer(m_fig_type, m_tilt_angle))
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
  m_fig_type = fb_item->get_fig_type();
  m_tilt_angle = fb_item->get_tilt_angle();
  update_cube();
}

void Fbo_renderer::update_cube()
{
  m_cube->set_cube_updates(m_fig_type, m_tilt_angle);
}
