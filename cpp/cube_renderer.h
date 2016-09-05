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
  explicit Cube_renderer();
  ~Cube_renderer();
  void render();
  void initialize();
  void set_cube_updates(const QString &fig_type, const int tilt_angle, const float scale);

private:
  void create_geometry();
  void update_modelview();
  void load_correct_texture();

private:
   enum CUBE_CHARACTERS{VERTEX = 4, SIDES = 6};

private:
  QVector<QOpenGLTexture*> m_board_texture;
  QOpenGLShaderProgram *m_program;

  QMatrix4x4 modelview;
  QOpenGLBuffer m_buffer;

  QString m_name;

  float m_x_angle;
  float m_y_angle;
  float m_z_angle;

  GLfloat m_elem_size;
  QVector3D m_scale_vect;

  const int m_VERTEX_ATTRIBUTE;
  const int m_TEXCOORD_ATTRIBUTE;
};
#endif
