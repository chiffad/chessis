#include <QtGui/QOpenGLFramebufferObject>
#include <QtQuick/QQuickWindow>
#include <qsgsimpletexturenode.h>
#include "headers/fb_obj.h"
#include "headers/cube_renderer.h"

Fb_obj::Fb_obj()
{
  qDebug()<<"here";
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
  m_fig_type = fb_item->get_fig_type();
  update_cube();
}

void Fbo_renderer::update_cube()
{
  QVector3D vect = m_fig_type == "board" ? QVector3D(1,1,0.06) : QVector3D(1,1,1);
  cube->set_cube_updates(vect);
}
