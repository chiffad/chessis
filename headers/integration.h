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
  //Q_PROPERTY(bool is_visible READ is_visible WRITE set_visible NOTIFY visible_changed)
  //bool is_visible() const{return figure_visible;}
  //void set_visible(bool visible);

signals:
  //void visible_changed();

public slots:
  //void back_move();
  bool move(unsigned int x, unsigned int y);
  bool is_free_field(unsigned int x, unsigned int y);
  bool is_the_same_coord(unsigned int x_candidate, unsigned int y_candidate,
                         unsigned int x, unsigned int y);
  unsigned int correct_figure_coord(unsigned int coord);

private:
  Board* board;
  Board::Coord coord;
};
#endif

