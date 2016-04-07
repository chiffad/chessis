#ifndef PIECHART_H
#define PIECHART_H

#include <QtQuick/QQuickPaintedItem>
#include <QString>
#include <QChar>
#include <QAbstractListModel>
#include <QStringList>
#include "headers/chess.h"

class ChessIntegration : public QAbstractListModel
{
  Q_OBJECT
public:
  class Figure;
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

public:
  //Q_PROPERTY(QString move_turn_color READ move_turn_color NOTIFY move_turn_color_changed) // work!
  //QString move_turn_color() const; // work!

  //Q_PROPERTY(QStringList moves_history READ moves_history NOTIFY moves_history_changed)
  //QStringList moves_history() const;

  Q_PROPERTY(bool is_check_mate READ is_check_mate NOTIFY check_mate)
  bool is_check_mate() const;

  Q_INVOKABLE void move(const unsigned x, const unsigned y);
  Q_INVOKABLE void back_move();
  //Q_INVOKABLE QChar letter_return();

signals:
  //void move_turn_color_changed(); // work!
  //void moves_history_changed();
  void check_mate();

private:
  void set_new_figure_coord(const Board::Coord& old_coord, const Board::Coord& new_coord, bool back_move = false);
  void correct_figure_coord(Board::Coord& coord, const unsigned x, const unsigned y);
  void emit_data_changed(const int INDEX);
  //void switch_move_color();// work!S
  //void add_to_history(const Board::Coord& coord_from, const Board::Coord& coord_to);
  int get_index(const Board::Coord& coord, int index = 0) const;

private:
  Board* board;
  QString m_move_color;
  QList<Figure> m_figures_model;
};

class ChessIntegration::Figure
{
public:
  Figure(const QString& name, const int x, const int y, const bool visible);

  QString name() const {return m_name;}
  int x() const {return m_x;}
  int y() const {return m_y;}
  bool visible() const {return m_visible;}

  void set_name(const QString& new_name) {m_name = new_name;}
  void set_coord(const Board::Coord& new_coord) {m_x = new_coord.x; m_y = new_coord.y;}
  void set_visible(const bool new_visible) {m_visible = new_visible;}

public:
  QString m_name;
  int m_x;
  int m_y;
  bool m_visible;
};
#endif

