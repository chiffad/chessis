#ifndef PIECHART_H
#define PIECHART_H

#include <QtQuick/QQuickPaintedItem>
#include <QString>
#include <QAbstractListModel>
#include <QMouseEvent>
#include "headers/chess.h"

class Figure
{
public:
  Figure(const QString& name, const int x, const int y, const bool visible);

  QString name() const {return m_name;}
  int x() const {return m_x;}
  int y() const {return m_y;}
  int visible() const {return m_visible;}

  void set_name(const QString& new_name) {m_name = new_name;}
  void set_coord(const Board::Coord& new_coord) {m_x = new_coord.x; m_y = new_coord.y;}
  void set_visible(const bool new_visible) {m_visible = new_visible;}

public:
  QString m_name;
  int m_x;
  int m_y;
  bool m_visible;
};

class ChessIntegration : public QAbstractListModel
{
  Q_OBJECT
public:
  enum FigureRoles {
      NameRole = Qt::UserRole + 3,
      XRole = Qt::UserRole + 2,
      YRole = Qt::UserRole + 1,
      VisibleRole = Qt::UserRole
  };

  explicit ChessIntegration(QObject *parent = 0);
  ~ChessIntegration(){delete board;}

  void addFigure(const Figure &figure);
  int rowCount(const QModelIndex & parent = QModelIndex()) const;
  QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

protected:
  QHash<int, QByteArray> roleNames() const;
  void mouse_event(const QMouseEvent* event);

public:
  Q_PROPERTY(QString move_turn_color READ move_turn_color NOTIFY move_turn_color_changed)
  QString move_turn_color() const;

 // Q_PROPERTY(QString history_move READ history_move NOTIFY history_move_changed)
 // QString history_move() const;

signals:
  void move_turn_color_changed();
 // void history_move_changed();

private:
  void back_move();
  void move(const unsigned int x, const unsigned int y);
  int get_index(const Board::Coord& coord) const;
  void set_new_figure_coord(const Board::Coord& old_coord, const Board::Coord& new_coord, bool back_move = false);
  void correct_figure_coord(Board::Coord& coord, const unsigned int x, const unsigned int y);
  void emit_data_changed(const int INDEX);
  //void add_to_moves_history(const Board::Coord& coord_from, const Board::Coord& coord_to);
  void switch_move_color();

private:
  Board* board;
  QString m_move_color;
  QString m_history_move;
  QList<Figure> m_figures_model;
};
#endif
