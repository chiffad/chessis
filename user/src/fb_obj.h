#ifndef __MY_FB_OBJ_H__VGYUKYFIUYVULBVUYFKLBYUFUYBYUKF
#define __MY_FB_OBJ_H__VGYUKYFIUYVULBVUYFKLBYUFUYBYUKF

#include <QtQuick/QQuickFramebufferObject>

namespace graphic {

class fb_obj_t : public QQuickFramebufferObject
{
  Q_OBJECT

public:
  fb_obj_t();
  ~fb_obj_t();
  Renderer* createRenderer() const Q_DECL_OVERRIDE;

  Q_PROPERTY(QString fig_type READ get_fig_type WRITE set_fig_type NOTIFY fig_type_changed)
  QString get_fig_type() const;
  void set_fig_type(const QString& name);

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
  QString fig_type;
  int tilt_angle;
  float cube_scale;
};

} // namespace graphic

#endif // __MY_FB_OBJ_H__VGYUKYFIUYVULBVUYFKLBYUFUYBYUKF
