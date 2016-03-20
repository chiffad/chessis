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
  Q_PROPERTY(bool back_move_signal READ back_move_signal NOTIFY back_move_signal_emmit)
  bool back_move_signal() const{return true;}

signals:
  void back_move_signal_emmit();

public slots:
  void back_move();
  bool move(unsigned int x, unsigned int y);
  bool is_free_field(unsigned int x, unsigned int y);
  unsigned int correct_figure_coord(unsigned int coord);

  const int prev_to_coord(QString selected_coord) const;
  const int prev_from_coord(QString selected_coord) const;

  int see(int x);
private:
  Board* board;
  Board::Coord coord;
};
#endif

