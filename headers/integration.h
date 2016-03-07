#ifndef PIECHART_H
#define PIECHART_H

#include <QtQuick/QQuickPaintedItem>
#include <QObject>
#include <QString>
#include "headers/chess.h"

class Integr : public QObject
{
  Q_OBJECT

public:
  explicit Integr(QObject *parent = 0);
  ~Integr(){delete board;}

public:
  Q_PROPERTY(int coord_x READ coord_x WRITE set_coord_x NOTIFY x_y_changed())
  int coord_x()const{return coord.x;}
  void set_coord_x(const int& val);

signals:
  void x_y_changed();

public slots:
  void back_move();
  bool move(unsigned int x, unsigned int y);
  //bool is_not_beaten(unsigned int x, unsigned int y, QString fig);
  unsigned int correct_img_coord(unsigned int coord);

private:
  Board* board;
  Board::Coord coord;
};
#endif

