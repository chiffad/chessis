#include <QPainter>
#include <QPaintEngine>
#include <cmath>
#include <QGLWidget>
#include <QOpenGLTexture>
#include <QImage>
#include <QString>
#include <algorithm>
#include "cube_renderer.h"

Cube_renderer::Cube_renderer() : m_program(new QOpenGLShaderProgram), m_x_angle(0), m_y_angle(0), m_z_angle(0),
                                 m_elem_size(1), m_scale_vect(1,1,1)
{
  update_modelview();
}

Cube_renderer::~Cube_renderer()
{
  for(auto &i : m_board_texture)
    delete i;
  delete m_program;
}

void Cube_renderer::initialize()
{
  qDebug()<<"Cube_renderer::initialize()";
  initializeOpenGLFunctions();

  create_geometry();

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  //glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  //glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1

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
  m_program->bindAttributeLocation("vertex", PROGRAM_VERTEX_ATTRIBUTE);
  m_program->bindAttributeLocation("texCoord", PROGRAM_TEXCOORD_ATTRIBUTE);
  m_program->link();

  m_program->bind();
  m_program->setUniformValue("texture", 0);
}

void Cube_renderer::set_cube_updates(const QString &fig_name, const int tilt_angle, const float scale)
{
//    qDebug()<<"Cube_renderer::set_cube_updates()";

  //if(m_name != fig_name)
  //{

    m_x_angle = tilt_angle;
    m_elem_size = scale;
    m_name = fig_name;
    load_correct_texture();
    update_modelview();

  //}
}

void Cube_renderer::load_correct_texture()
{
  qDebug()<<"Cube_renderer::load_correct_textur: "<<m_name;

  const QString PATH_TO_IMG = "chessis/res/img/";

  QImage fase_im(PATH_TO_IMG + m_name + ".png");
  QImage side_im(PATH_TO_IMG + *m_name.begin() + ".png");
  float z_scale = 2;

  if(m_name == "board")
  {
    fase_im.load(PATH_TO_IMG + "board.png");
    side_im.load(PATH_TO_IMG + "board_side.png");
    z_scale = 0.1;
  }

  else if(m_name == "hilight")
  {
    side_im = fase_im;
    z_scale = 0;
  }

 /* for(auto i : m_board_texture)
    if(i->isBound())
    {
      i->release();
    }*/

  qDebug()<<"!!!!!"<<m_board_texture.size();
  for(auto &i : m_board_texture)
    delete i;

  qDebug()<<"!!!!!"<<m_board_texture.size();
  m_board_texture.clear();

  qDebug()<<"!!!!!"<<m_board_texture.size();
  m_board_texture.append(new QOpenGLTexture(fase_im.mirrored()));
  m_board_texture.append(new QOpenGLTexture(side_im));
  m_scale_vect = QVector3D(1, 1, z_scale);
}

void Cube_renderer::update_modelview()
{
  qDebug()<<"Cube_renderer::update_modelview()";
  modelView.rotate(m_x_angle, 1.0f, 0.0f, 0.0f);
  modelView.rotate(m_y_angle, 0.0f, 1.0f, 0.0f);
  modelView.rotate(m_z_angle, 0.0f, 0.0f, 1.0f);
  modelView.scale(m_scale_vect);
  //modelview.translate(0.0f, 0.0f, 0.0f);
}

void Cube_renderer::render()
{
  create_geometry();

  //glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

  m_program->setUniformValue("matrix", modelView);
  m_program->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
  m_program->enableAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE);
  m_program->setAttributeBuffer(PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT,   0                  , 3, 5 * sizeof(GLfloat));
  m_program->setAttributeBuffer(PROGRAM_TEXCOORD_ATTRIBUTE, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));

  m_board_texture.first()->bind();

  for(int i = 0; i < SIDES; ++i)
  {
    if(i == 1) m_board_texture.last()->bind();

    glDrawArrays(GL_TRIANGLE_FAN, i * VERTEX, VERTEX);
  }
}

void Cube_renderer::create_geometry()
{
  const GLfloat s = m_elem_size;
  QVector<GLfloat> vert_data = {+s, -s, -s,   1, 1,   -s, -s, -s,   0,1,   -s, +s, -s,   0,0,   +s, +s, -s,   1,0,
                                +s, +s, -s,   1, 1,   -s, +s, -s,   0,1,   -s, +s, +s,   0,0,   +s, +s, +s,   1,0,
                                +s, -s, +s,   1, 1,   +s, -s, -s,   0,1,   +s, +s, -s,   0,0,   +s, +s, +s,   1,0,
                                -s, -s, -s,   1, 1,   -s, -s, +s,   0,1,   -s, +s, +s,   0,0,   -s, +s, -s,   1,0,
                                +s, -s, +s,   1, 1,   -s, -s, +s,   0,1,   -s, -s, -s,   0,0,   +s, -s, -s,   1,0,
                                -s, -s, +s,   1, 1,   +s, -s, +s,   0,1,   +s, +s, +s,   0,0,   -s, +s, +s,   1,0};
  m_buffer.create();
  m_buffer.bind();
  m_buffer.allocate(vert_data.constData(), vert_data.count() * sizeof(GLfloat));
}
