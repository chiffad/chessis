#pragma once

#include "figure.h"

#include <QAbstractListModel>
#include <vector>

namespace controller {

class figures_model_t : public QAbstractListModel
{
public:
  explicit figures_model_t(QObject* parent = nullptr);
  figures_model_t(const figures_model_t&) = delete;
  figures_model_t& operator=(const figures_model_t&) = delete;
  ~figures_model_t() override;

  void update_figure(figure_t figure, size_t i);
  void add_figure(figure_t figure);

  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
  QHash<int, QByteArray> roleNames() const override;

private:
  enum FigureRoles
  {
    NameRole = Qt::UserRole + 3,
    XRole = Qt::UserRole + 2,
    YRole = Qt::UserRole + 1,
    VisibleRole = Qt::UserRole
  };

private:
  std::vector<figure_t> figures_model_;
};

} // namespace controller
