#ifndef FBOGRENDERER_H
#define FBORENDERER_H

#include <QtQuick/QQuickFramebufferObject>
#include "headers/cube_renderer.h"

class Fb_obj : public QQuickFramebufferObject
{
  Q_OBJECT

public:
  Fb_obj();
  ~Fb_obj();
  Renderer* createRenderer() const Q_DECL_OVERRIDE;

  Q_PROPERTY(QString fig_type READ get_fig_type WRITE set_fig_type)
  QString get_fig_type() const;
  void set_fig_type(const QString &name);

  Q_PROPERTY(int tilt_angle READ get_tilt_angle WRITE set_tilt_angle NOTIFY tilt_angle_changed)
  int get_tilt_angle() const;
  void set_tilt_angle(const int angl);

signals:
  void tilt_angle_changed();

private:
  QString m_fig_type;
  int m_tilt_angle;
};


class Fbo_renderer : public QQuickFramebufferObject::Renderer
{

public:
  Fbo_renderer();
  ~Fbo_renderer();
  QOpenGLFramebufferObject* createFramebufferObject(const QSize &size) Q_DECL_OVERRIDE;
  void synchronize(QQuickFramebufferObject *item) Q_DECL_OVERRIDE;
  void render() Q_DECL_OVERRIDE;
  void update_cube();

private:
  QString m_fig_type;
  int m_tilt_angle;
  Cube_renderer *m_cube;
};
#endif
