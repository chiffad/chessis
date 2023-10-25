#include "controller/figures_model.hpp"

#include "helper.h"
#include <spdlog/spdlog.h>

namespace chess::controller {

figures_model_t::figures_model_t(QObject* parent)
  : QAbstractListModel(parent)
{}

figures_model_t::~figures_model_t() = default;

void figures_model_t::update_figure(figure_t figure, const size_t i)
{
  if (i >= figures_model_.size())
  {
    SPDLOG_ERROR("Trying to set figure index={}, but size={}", i, figures_model_.size());
    return;
  }

  figures_model_[i] = std::move(figure);
  const QModelIndex idx = index(i, 0);
  emit dataChanged(idx, idx);
}

void figures_model_t::add_figure(figure_t figure)
{
  beginInsertRows(QModelIndex(), rowCount(), rowCount());
  figures_model_.push_back(std::move(figure));
  endInsertRows();
}

int figures_model_t::rowCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);
  return figures_model_.size();
}

QVariant figures_model_t::data(const QModelIndex& index, int role) const
{
  if (index.row() < 0 || index.row() >= figures_model_.size())
  {
    return QVariant();
  }

  const figure_t& figure = figures_model_[index.row()];
  switch (role)
  {
    case NameRole: return figure.name();
    case XRole: return figure.coord().x;
    case YRole: return figure.coord().y;
    case VisibleRole: return figure.visible();
  }
  return QVariant();
}

QHash<int, QByteArray> figures_model_t::roleNames() const
{
  QHash<int, QByteArray> roles;
  roles[NameRole] = "figure_name";
  roles[XRole] = "figure_x";
  roles[YRole] = "figure_y";
  roles[VisibleRole] = "figure_visible";
  return roles;
}

} // namespace chess::controller
