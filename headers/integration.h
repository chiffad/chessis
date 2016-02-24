#ifndef PIECHART_H
#define PIECHART_H

#include <QtQuick/QQuickPaintedItem>
#include <QObject>
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

  Q_PROPERTY(int coord_y READ coord_y WRITE set_coord_y NOTIFY x_y_changed())
  int coord_y()const {return coord.y;}
  void set_coord_y(const int& val);

signals:
  void x_y_changed();

public slots:
  void back_move();
  bool move(int x, int y);

private:
  Board* board;
  Board::Coord coord;
};
#endif

