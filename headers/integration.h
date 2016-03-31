#ifndef PIECHART_H
#define PIECHART_H

#include <QtQuick/QQuickPaintedItem>
#include <QString>
#include <QAbstractListModel>
#include <QAbstractItemModel>
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

class CppIntegration : public QAbstractListModel
{
  Q_OBJECT
public:
  enum FigureRoles {
      NameRole = Qt::UserRole + 3,
      XRole = Qt::UserRole + 2,
      YRole = Qt::UserRole + 1,
      VisibleRole = Qt::UserRole
  };

  explicit CppIntegration(QObject *parent = 0);
  ~CppIntegration(){delete board;}

  void addFigure(const Figure &figure);
  int rowCount(const QModelIndex & parent = QModelIndex()) const;
  QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

protected:
  QHash<int, QByteArray> roleNames() const;

public:
  Q_PROPERTY(QString move_turn_color READ move_turn_color NOTIFY move_turn_color_changed)
  QString move_turn_color() const;

  Q_PROPERTY(QString convert_move_coord_in_letter READ convert_move_coord_in_letter NOTIFY convert_move_coord_in_letter_changed)
  QString convert_move_coord_in_letter() const;

signals:
  void move_turn_color_changed();
  void convert_move_coord_in_letter_changed();

public slots:
  void back_move();
  void move(const unsigned int x, const unsigned int y);
  QString letter_return(const int number);

private:
  int get_index(const Board::Coord& coord) const;
  void set_new_figure_coord(const Board::Coord& old_coord, const Board::Coord& new_coord);

private:
  QList<Figure> m_figures_model;
  Board* board;
  QString move_color;
  QString convert_coord_in_letter;
};

#endif

