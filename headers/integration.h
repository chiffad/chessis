#ifndef PIECHART_H
#define PIECHART_H

#include <QtQuick/QQuickPaintedItem>
#include <QString>
#include <QChar>
#include <QAbstractListModel>
#include <QStringList>
#include <vector>
#include "headers/chess.h"

class ChessIntegration : public QAbstractListModel
{
  Q_OBJECT
public:
  class Figure;
  enum HILIGHT {HILIGHT_CELLS = 2 , FIRST_HILIGHT = 32, SECOND_HILIGHT = 33};
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

  Q_PROPERTY(QStringList moves_history READ moves_history NOTIFY moves_history_changed)//work
  QStringList moves_history() const;//work

  //Q_PROPERTY(bool is_check_mate READ is_check_mate NOTIFY check_mate)//test need
  //bool is_check_mate() const;//test need

  Q_INVOKABLE void move(const unsigned x, const unsigned y, bool new_move = true);
  //Q_INVOKABLE void back_move(); //work!
  Q_INVOKABLE QChar letter_return(const int index) const;
  //Q_INVOKABLE void start_new_game();//work!
  Q_INVOKABLE void go_to_history_index(const unsigned index);//!!

signals:
  //void move_turn_color_changed(); // work!
  void moves_history_changed();//work
  //void check_mate(); // test need

private:
  void update_coordinates();//work
  //void switch_move_color();// work!
  void emit_data_changed(const int INDEX); //work
  void correct_figure_coord(Board::Coord& coord, const unsigned x, const unsigned y);//work
  void update_hilight(const Board::Coord& coord, HILIGHT hilight_index, bool visible);//work
  void add_to_history(const Board::Coord& coord_from, const Board::Coord& coord_to);//test need
  void add_move_to_history_copy(const Board::Coord& coord_from, const Board::Coord& coord_to); //!!

private:
  Board* board;
  QString m_move_color;
  QStringList m_moves_history;
  QList<Figure> m_figures_model;

  struct Copy_of_history_moves
  {
    Board::Coord from;
    Board::Coord to;
  };
  std::vector<Copy_of_history_moves> history_copy;
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
