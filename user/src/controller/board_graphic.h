#pragma once

#include <QAbstractListModel>
#include <QHash>
#include <QModelIndex>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QtQuick/QQuickPaintedItem>
#include <functional>
#include <vector>

#include "client/messages.h"
#include "coord.h"
#include "figure.h"

namespace controller {

class board_graphic_t : public QAbstractListModel
{
  Q_OBJECT
  Q_PROPERTY(bool is_check_mate READ is_check_mate NOTIFY check_mate)

public:
  using command_requested_callback_t = std::function<void(std::string)>;

public:
  explicit board_graphic_t(const command_requested_callback_t& callback);
  board_graphic_t(const board_graphic_t&) = delete;
  board_graphic_t& operator=(const board_graphic_t&) = delete;
  ~board_graphic_t() override;

  int rowCount(const QModelIndex& parent = QModelIndex()) const;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

  bool is_check_mate() const;
  void set_check_mate();
  void update_hilight(const int move_num, const QString& history);
  void redraw_board();
  void set_board_mask(const QString& mask);

  Q_INVOKABLE void run_command(const QString& message, const int x1 = 0, const int y1 = 0, const int x2 = 0, const int y2 = 0);
  Q_INVOKABLE void set_cell_size(int width, int height);

signals:
  void check_mate();

protected:
  QHash<int, QByteArray> roleNames() const;

private:
  enum FigureRoles
  {
    NameRole = Qt::UserRole + 3,
    XRole = Qt::UserRole + 2,
    YRole = Qt::UserRole + 1,
    VisibleRole = Qt::UserRole
  };

private:
  void addFigure(const figure_t& figure);
  void update_coordinates();
  coord_t get_field_coord(const int i) const;
  const QString coord_to_str(const coord_t& from, const coord_t& to) const;
  coord_t get_coord(const int x, const int y);

  template<typename T>
  inline void command_requested(T&& command)
  {
    command_requested_callback_(msg::prepare_for_send(std::forward<T>(command)));
  }

private:
  bool check_mate_;
  QList<figure_t> figures_model_;
  QString field_;
  unsigned cell_width_ = 0;
  unsigned cell_height_ = 0;

  command_requested_callback_t command_requested_callback_;
};

} // namespace controller