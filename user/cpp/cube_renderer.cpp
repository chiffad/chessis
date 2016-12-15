#include "cube_renderer.h"


#include <QPainter>
#include <QPaintEngine>
#include <cmath>
#include <QGLWidget>
#include <QImage>
#include <algorithm>
#include <QtGui/qvector3d.h>
#include <QtGui/qmatrix4x4.h>
#include <QtGui/qopenglshaderprogram.h>
#include <QtGui/qopenglfunctions.h>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>
#include <QVector>


using namespace graphic;

struct Cube_renderer::impl_t : public QOpenGLFunctions
{
  impl_t();
  void render();
  void initialize();
  void set_cube_updates(const QString &fig_type, const int tilt_angle, const float scale);
  void create_geometry();
  void update_model_view(const float scale);
  void load_correct_texture(const QString &name);

  enum{VERTEX_ATTRIBUTE = 0, TEXCOORD_ATTRIBUTE = 1};

  QVector<std::shared_ptr<QOpenGLTexture>> board_texture;
  std::shared_ptr<QOpenGLShaderProgram> program;

  QMatrix4x4 model_view;
  QOpenGLBuffer buffer;

  float x_angle;
  float y_angle;
  float z_angle;

  QString fig_name;
  QVector3D scale_vect;

  int vertex_attribute;
  int texCoord_attribute;
  int matrix_uniform;
};

Cube_renderer::Cube_renderer()
    : impl(new impl_t)
{
}

Cube_renderer::~Cube_renderer()
{
}

void Cube_renderer::render()
{
  impl->render();
}

void Cube_renderer::initialize()
{
  impl->initialize();
}

void Cube_renderer::set_cube_updates(const QString &fig_type, const int tilt_angle, const float scale)
{
  impl->set_cube_updates(fig_type, tilt_angle, scale);
}

Cube_renderer::impl_t::impl_t()
    : program(std::make_shared<QOpenGLShaderProgram>()), model_view(QMatrix4x4()),
      x_angle(0), y_angle(0), z_angle(0), scale_vect(1,1,1)
{
}

void Cube_renderer::impl_t::initialize()
{
  initializeOpenGLFunctions();

  create_geometry();

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  //glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  QOpenGLShader *vshader = new QOpenGLShader(QOpenGLShader::Vertex, program.get());
  const char *vsrc =
      "attribute highp vec4 vertex;\n"
      "attribute mediump vec4 texCoord;\n"
      "varying mediump vec4 texc;\n"
      "uniform mediump mat4 matrix;\n"
      "void main(void)\n"
      "{\n"
      "    gl_Position = matrix * vertex;\n"
      "    texc = texCoord;\n"
      "}\n";
  vshader->compileSourceCode(vsrc);

  QOpenGLShader *fshader = new QOpenGLShader(QOpenGLShader::Fragment, program.get());
  const char *fsrc =
      "uniform sampler2D texture;\n"
      "varying mediump vec4 texc;\n"
      "void main(void)\n"
      "{\n"
      "    gl_FragColor = texture2D(texture, texc.st);\n"
      "}\n";
  fshader->compileSourceCode(fsrc);

  program->addShader(vshader);
  program->addShader(fshader);
  program->bindAttributeLocation("vertex", VERTEX_ATTRIBUTE);
  program->bindAttributeLocation("texCoord", TEXCOORD_ATTRIBUTE);
  program->link();

  program->bind();
  program->setUniformValue("texture", 0);
}

void Cube_renderer::impl_t::set_cube_updates(const QString &name, const int tilt_angle, const float scale)
{
  if(fig_name == name)
    { return; }

  x_angle = tilt_angle;
  load_correct_texture(name);
  update_model_view(scale);
}

void Cube_renderer::impl_t::load_correct_texture(const QString &name)
{
  const QString PATH_TO_IMG = "../res/img/";

  QImage fase_im(PATH_TO_IMG + name + ".png");
  QImage side_im(PATH_TO_IMG + *name.begin() + ".png");
  float z_scale = 4;

  if(name == "board")
  {
    fase_im.load(PATH_TO_IMG + "board.png");
    side_im.load(PATH_TO_IMG + "board_side.png");
    z_scale = 0.1;
  }

  else if(name == "hilight")
  {
    side_im = fase_im;
    z_scale = 0;
  }

  board_texture.clear();
  board_texture.append(std::make_shared<QOpenGLTexture>(fase_im.mirrored()));
  board_texture.append(std::make_shared<QOpenGLTexture>(side_im));
  scale_vect = QVector3D(1, 1, z_scale);
  fig_name = name;
}

void Cube_renderer::impl_t::update_model_view(const float scale)
{
  scale_vect *= scale;

  model_view = QMatrix4x4();
  model_view.rotate(x_angle, 1.0f, 0.0f, 0.0f);
  model_view.rotate(y_angle, 0.0f, 1.0f, 0.0f);
  model_view.rotate(z_angle, 0.0f, 0.0f, 1.0f);
  model_view.scale(scale_vect);
}

void Cube_renderer::impl_t::render()
{
  create_geometry();

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

  program->setUniformValue("matrix", model_view);
  program->enableAttributeArray(VERTEX_ATTRIBUTE);
  program->enableAttributeArray(TEXCOORD_ATTRIBUTE);
  program->setAttributeBuffer(VERTEX_ATTRIBUTE, GL_FLOAT,   0                  , 3, 5 * sizeof(GLfloat));
  program->setAttributeBuffer(TEXCOORD_ATTRIBUTE, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));

  board_texture.first()->bind();

  const int SIDES = 6;
  const int VERTEX = 4;
  for(int i = 0; i < SIDES; ++i)
  {
    if(i == 1)
      {board_texture.last()->bind(); }

    glDrawArrays(GL_TRIANGLE_FAN, i * VERTEX, VERTEX);
  }
}

void Cube_renderer::impl_t::create_geometry()
{
  QVector<GLfloat> vert_data = {+1, -1, -1,   1, 1,   -1, -1, -1,   0,1,   -1, +1, -1,   0,0,   +1, +1, -1,   1,0,
                                +1, +1, -1,   1, 1,   -1, +1, -1,   0,1,   -1, +1, +1,   0,0,   +1, +1, +1,   1,0,
                                +1, -1, +1,   1, 1,   +1, -1, -1,   0,1,   +1, +1, -1,   0,0,   +1, +1, +1,   1,0,
                                -1, -1, -1,   1, 1,   -1, -1, +1,   0,1,   -1, +1, +1,   0,0,   -1, +1, -1,   1,0,
                                +1, -1, +1,   1, 1,   -1, -1, +1,   0,1,   -1, -1, -1,   0,0,   +1, -1, -1,   1,0,
                                -1, -1, +1,   1, 1,   +1, -1, +1,   0,1,   +1, +1, +1,   0,0,   -1, +1, +1,   1,0};
  buffer.create();
  buffer.bind();
  buffer.allocate(vert_data.constData(), vert_data.count() * sizeof(GLfloat));
}
