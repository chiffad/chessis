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
  Q_PROPERTY(QString move_turn_color READ move_turn_color WRITE set_move_turn_color NOTIFY move_turn_color_changed)
  QString move_turn_color() const {return move_color;}
  void set_move_turn_color(QString color);

signals:
  void move_turn_color_changed();

public slots:
  void back_move();
  bool move(unsigned int x, unsigned int y);
  bool is_free_field(unsigned int x, unsigned int y);
  unsigned int correct_figure_coord(unsigned int coord);

  const int prev_to_coord(QString selected_coord) const;
  const int prev_from_coord(QString selected_coord) const;
  QString figure_on_field_move_to();

  int see(int x);

private:
  Board* board;
  Board::Coord coord;
  QString move_color;
};
#endif

