#ifndef CUBERENDERER_H
#define CUBERENDERER_H

#include <QtGui/qvector3d.h>
#include <QtGui/qmatrix4x4.h>
#include <QtGui/qopenglshaderprogram.h>
#include <QtGui/qopenglfunctions.h>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>
#include <QVector>
#include <memory>

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
  void update_model_view(const float scale);
  void load_correct_texture(const QString &name);

private:
   enum{VERTEX_ATTRIBUTE = 0, TEXCOORD_ATTRIBUTE = 1};

private:
  QVector<std::shared_ptr<QOpenGLTexture>> m_board_texture;
  std::shared_ptr<QOpenGLShaderProgram> m_program;

  QMatrix4x4 m_model_view;
  QOpenGLBuffer m_buffer;

  float m_x_angle;
  float m_y_angle;
  float m_z_angle;

  QString m_name;
  QVector3D m_scale_vect;

  int m_vertex_attribute;
  int m_texCoord_attribute;
  int m_matrix_uniform;
};
#endif
