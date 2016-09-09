#include <QPainter>
#include <QPaintEngine>
#include <cmath>
#include <QGLWidget>
#include <QOpenGLTexture>
#include <QImage>
#include <QString>
#include <algorithm>
#include "cube_renderer.h"

Cube_renderer::Cube_renderer() : m_program(new QOpenGLShaderProgram), m_model_view(new QMatrix4x4),
                                 m_x_angle(0), m_y_angle(0), m_z_angle(0), m_scale_vect(1,1,1)
{
}

Cube_renderer::~Cube_renderer()
{
  for(auto &i : m_board_texture)
    delete i;
  delete m_program;
  delete m_model_view;
}

void Cube_renderer::initialize()
{
  qDebug()<<"Cube_renderer::initialize()";
  initializeOpenGLFunctions();

  create_geometry();

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  //glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  QOpenGLShader *vshader = new QOpenGLShader(QOpenGLShader::Vertex, m_program);
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

  QOpenGLShader *fshader = new QOpenGLShader(QOpenGLShader::Fragment, m_program);
  const char *fsrc =
      "uniform sampler2D texture;\n"
      "varying mediump vec4 texc;\n"
      "void main(void)\n"
      "{\n"
      "    gl_FragColor = texture2D(texture, texc.st);\n"
      "}\n";
  fshader->compileSourceCode(fsrc);

  m_program->addShader(vshader);
  m_program->addShader(fshader);
  m_program->bindAttributeLocation("vertex", VERTEX_ATTRIBUTE);
  m_program->bindAttributeLocation("texCoord", TEXCOORD_ATTRIBUTE);
  m_program->link();

  m_program->bind();
  m_program->setUniformValue("texture", 0);
}

void Cube_renderer::set_cube_updates(const QString &fig_name, const int tilt_angle, const float scale)
{
  if(m_name != fig_name)
  {
    m_x_angle = tilt_angle;
    load_correct_texture(fig_name);
    update_model_view(scale);
  }
}

void Cube_renderer::load_correct_texture(const QString &name)
{
  qDebug()<<"Cube_renderer::load_correct_textur: "<<name;

  const QString PATH_TO_IMG = "../chessis/res/img/";

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

  for(auto &i : m_board_texture)
    delete i;

  m_board_texture.clear();
  m_board_texture.append(new QOpenGLTexture(fase_im.mirrored()));
  m_board_texture.append(new QOpenGLTexture(side_im));
  m_scale_vect = QVector3D(1, 1, z_scale);
  m_name = name;
}

void Cube_renderer::update_model_view(const float scale)
{
  qDebug()<<"Cube_renderer::update_modelview()";
  m_scale_vect *= scale;
  delete m_model_view;

  m_model_view = new QMatrix4x4();
  m_model_view->rotate(m_x_angle, 1.0f, 0.0f, 0.0f);
  m_model_view->rotate(m_y_angle, 0.0f, 1.0f, 0.0f);
  m_model_view->rotate(m_z_angle, 0.0f, 0.0f, 1.0f);
  m_model_view->scale(m_scale_vect);
}

void Cube_renderer::render()
{
  create_geometry();

  //glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

  m_program->setUniformValue("matrix", *m_model_view);
  m_program->enableAttributeArray(VERTEX_ATTRIBUTE);
  m_program->enableAttributeArray(TEXCOORD_ATTRIBUTE);
  m_program->setAttributeBuffer(VERTEX_ATTRIBUTE, GL_FLOAT,   0                  , 3, 5 * sizeof(GLfloat));
  m_program->setAttributeBuffer(TEXCOORD_ATTRIBUTE, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));

  m_board_texture.first()->bind();

  const int SIDES = 6;
  const int VERTEX = 4;
  for(int i = 0; i < SIDES; ++i)
  {
    if(i == 1) m_board_texture.last()->bind();
    glDrawArrays(GL_TRIANGLE_FAN, i * VERTEX, VERTEX);
  }
}

void Cube_renderer::create_geometry()
{
  QVector<GLfloat> vert_data = {+1, -1, -1,   1, 1,   -1, -1, -1,   0,1,   -1, +1, -1,   0,0,   +1, +1, -1,   1,0,
                                +1, +1, -1,   1, 1,   -1, +1, -1,   0,1,   -1, +1, +1,   0,0,   +1, +1, +1,   1,0,
                                +1, -1, +1,   1, 1,   +1, -1, -1,   0,1,   +1, +1, -1,   0,0,   +1, +1, +1,   1,0,
                                -1, -1, -1,   1, 1,   -1, -1, +1,   0,1,   -1, +1, +1,   0,0,   -1, +1, -1,   1,0,
                                +1, -1, +1,   1, 1,   -1, -1, +1,   0,1,   -1, -1, -1,   0,0,   +1, -1, -1,   1,0,
                                -1, -1, +1,   1, 1,   +1, -1, +1,   0,1,   +1, +1, +1,   0,0,   -1, +1, +1,   1,0};
  m_buffer.create();
  m_buffer.bind();
  m_buffer.allocate(vert_data.constData(), vert_data.count() * sizeof(GLfloat));
}
