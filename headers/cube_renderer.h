#ifndef CUBERENDERER_H
#define CUBERENDERER_H

#include <QtGui/qvector3d.h>
#include <QtGui/qmatrix4x4.h>
#include <QtGui/qopenglshaderprogram.h>
#include <QtGui/qopenglfunctions.h>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>
#include <QVector>

class Cube_renderer : protected QOpenGLFunctions
{
public:
  explicit Cube_renderer(const QString &fig_type, const int x_angle);
  ~Cube_renderer();
  void render();
  void initialize();
  void set_cube_updates(const QString &fig_type, const int tilt_angle);

private:
  void create_geometry();
  void update_modelview();
  void load_correct_textur(const QString &fig_type);

private:
   enum CUBE_CHARACTERS{VERTEX = 4, SIDES = 6};

private:
  QVector<QOpenGLTexture*> m_board_texture;
  QOpenGLShaderProgram *m_program;

  QMatrix4x4 modelview;
  QOpenGLBuffer m_buffer;

  float m_x_angle;
  float m_y_angle;
  float m_z_angle;
  QVector3D m_scale_vect;

  const int m_VERTEX_ATTRIBUTE;
  const int m_TEXCOORD_ATTRIBUTE;
};
#endif
