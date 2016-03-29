#ifndef PIECHART_H
#define PIECHART_H

#include <QtQuick/QQuickPaintedItem>
#include <QObject>
#include <QString>
#include <QChar>
#include <QAbstractListModel>
#include <QStringList>
#include "headers/chess.h"

class Figure
{
public:
  Figure(const QString &name, const int x, const int y, const bool visible);

  QString name() const {return m_name;}
  int x() const {return m_x;}
  int y() const {return m_y;}
  int visible() const {return m_visible;}

private:
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
      NameRole = Qt::UserRole + 1,
      XRole,
      YRole,
      VisibleRole
  };

  explicit CppIntegration(QObject *parent = 0);
  ~CppIntegration(){delete board;}

  void addFigure(const Figure &figure);
  int rowCount(const QModelIndex & parent = QModelIndex()) const;
  QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

protected:
  QHash<int, QByteArray> roleNames() const;
private:
  QList<Figure> m_figure;
public:
  Q_PROPERTY(QString move_turn_color READ move_turn_color WRITE set_move_turn_color NOTIFY move_turn_color_changed)
  QString move_turn_color() const {return move_color;}
  void set_move_turn_color(QString color);

  Q_PROPERTY(QString w_move_in_letter READ w_move_in_letter WRITE set_w_move_in_letter NOTIFY w_move_in_letter_changed)
  QString w_move_in_letter() const {return w_move_letter;}
  void set_w_move_in_letter(QString c);

  Q_PROPERTY(QString b_move_in_letter READ b_move_in_letter WRITE set_b_move_in_letter NOTIFY b_move_in_letter_changed)
  QString b_move_in_letter() const {return b_move_letter;}
  void set_b_move_in_letter(QString c);
signals:
  void move_turn_color_changed();
  void w_move_in_letter_changed();
  void b_move_in_letter_changed();
public slots:
  void back_move();
  bool move(unsigned int x, unsigned int y);
  bool is_free_field(unsigned int x, unsigned int y);
  unsigned int correct_figure_coord(unsigned int coord);
  QChar letter_return(int number);

  const int prev_to_coord(QString selected_coord) const;
  const int prev_from_coord(QString selected_coord) const;
  QString figure_on_field_move_to();

  int see(int x);
private:
  Board* board;
  Board::Coord coord;
  QString move_color;
  QString w_move_letter;
  QString b_move_letter;
};
#endif

