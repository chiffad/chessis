#ifndef FBOGRENDERER_H
#define FBORENDERER_H

#include <QtQuick/QQuickFramebufferObject>
#include "cube_renderer.h"

class Fb_obj : public QQuickFramebufferObject
{
  Q_OBJECT

public:
  Fb_obj();
  ~Fb_obj();
  Renderer* createRenderer() const Q_DECL_OVERRIDE;

  Q_PROPERTY(QString fig_type READ get_fig_type WRITE set_fig_type NOTIFY fig_type_changed)
  QString get_fig_type() const;
  void set_fig_type(const QString &name);

  Q_PROPERTY(int tilt_angle READ get_tilt_angle WRITE set_tilt_angle NOTIFY tilt_angle_changed)
  int get_tilt_angle() const;
  void set_tilt_angle(const int angl);

  Q_PROPERTY(float cube_scale READ get_cube_scale WRITE set_cube_scale NOTIFY cube_scale_changed)
  float get_cube_scale() const;
  void set_cube_scale(const float scale);

signals:
  void fig_type_changed();
  void tilt_angle_changed();
  void cube_scale_changed();

private:
  QString m_fig_type;
  int m_tilt_angle;
  float m_cube_scale;
};


class Fbo_renderer : public QQuickFramebufferObject::Renderer
{

public:
  Fbo_renderer();
  ~Fbo_renderer();
  QOpenGLFramebufferObject* createFramebufferObject(const QSize &size) Q_DECL_OVERRIDE;
  void synchronize(QQuickFramebufferObject *item) Q_DECL_OVERRIDE;
  void render() Q_DECL_OVERRIDE;

private:
  Cube_renderer *m_cube;
};
#endif
