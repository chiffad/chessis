#include "board_graphic.h"

#include <QChar>
#include <QModelIndex>
#include <QPainter>
#include <algorithm>
#include <ctype.h>
#include <fstream>
#include <iostream>
#include <spdlog/spdlog.h>
#include <sstream>

#include "helper.h"

namespace graphic {

board_graphic_t::board_graphic_t(const command_requested_callback_t& callback)
  : QAbstractListModel(nullptr)
  , move_color_(MOVE_COLOR_W)
  , udp_connection_status_("Disconnected")
  , check_mate_(false)
  , command_requested_callback_(callback)
{
  enum
  {
    HILIGHT_CELLS = 2,
    FIGURES_NUMBER = 32
  };
  const QString HILIGHT_IM = "hilight";

  for (int i = 0; i < FIGURES_NUMBER + HILIGHT_CELLS; ++i)
  {
    addFigure(figure_t(HILIGHT_IM, 0, 0, false));
  }
}

board_graphic_t::~board_graphic_t()
{}

void board_graphic_t::run_command(const QString& message, const int x1, const int y1, const int x2, const int y2)
{
  const QString HELP_WORD = "help";
  const QString MOVE_WORD = "move";
  const QString BACK_MOVE = "back";
  const QString SHOW_ME = "show me";
  const QString NEW_GAME = "new game";
  const QString HISTORY = "to history";
  const QString SHOW_OPPONENT = "show opponent";

  SPDLOG_DEBUG("run command={}; x1={}; y1={}; x1={}; y2={}", message, x1, y1, x2, y2);

  if (message == HELP_WORD)
  {
    SPDLOG_DEBUG("run comman=dhelp_word");
    add_to_command_history("1.For move, type '" + MOVE_WORD + "' and coordinates(example: " + MOVE_WORD + " d2-d4)" + "\n" + "2.For back move, type '" +
                           BACK_MOVE + "'" + "\n" + "3.For start new game, type '" + NEW_GAME + "'" + "\n" + "4.For go to history index, type '" + HISTORY +
                           "' and index" + "\n" + "5.To view opponent information, print '" + SHOW_OPPONENT + "'" + "\n" +
                           "6.To view your information, print '" + SHOW_ME + "'");
  }
  else if (message == SHOW_OPPONENT)
  {
    command_requested(msg::opponent_inf_t());
  }
  else if (message == SHOW_ME)
  {
    command_requested(msg::my_inf_t());
  }
  else if (message == NEW_GAME)
  {
    command_requested(msg::new_game_t());
  }
  else if (message == BACK_MOVE)
  {
    command_requested(msg::back_move_t());
  }
  else if (message == HISTORY)
  {
    msg::go_to_history_t gth;
    gth.index = x1 + 1;
    command_requested(gth);
    add_to_command_history("command: " + message + " " + QString::number(x1 + 1));
    return;
  }
  else if (message.contains(HISTORY))
  {
    msg::go_to_history_t gth;
    gth.index = message.mid(HISTORY.size() + 1).toInt();
    command_requested(gth);
  }

  else
  {
    const QString command(message.mid(0, message.indexOf(FREE_SPACE)));
    const QString command_content(message.mid(command.size()));

    if (command == MOVE_WORD)
    {
      msg::move_t move;
      if (!command_content.isEmpty())
      {
        move.data = command_content.toStdString();
        command_requested(move);
      }
      else if (x1 + x2 + y1 + y2 != 0)
      {
        move.data = coord_to_str(get_coord(x1, y1), get_coord(x2, y2)).toStdString();
        command_requested(move);
        add_to_command_history("command: " + message + " " + QString::fromStdString(move.data));
        return;
      }
      else add_to_command_history("Unknown command (type '" + HELP_WORD + "' for help).");
    }
    else add_to_command_history("Unknown command (type '" + HELP_WORD + "' for help).");
  }
  add_to_command_history("command: " + message);
}

Coord board_graphic_t::get_coord(const int x, const int y)
{
  if (x < 0 || y < 0 || x > (cell_width_ * CELL_NUM) || y > (cell_height_ * CELL_NUM))
  {
    SPDLOG_ERROR("Incorrect coord x={}; y={}", x, y);
    return Coord(x, y);
  }

  return Coord((x + (cell_width_ / 2)) / cell_width_, (y + (cell_height_ / 2)) / cell_height_);
}

void board_graphic_t::update_coordinates()
{
  const auto FREE_FIELD = '.';

  auto f_it = field_.begin();
  for (auto& fig_mod : figures_model_)
  {
    f_it = std::find_if(f_it, field_.end(), [&FREE_FIELD](auto const& i) { return i != FREE_FIELD; });
    if (f_it != field_.end())
    {
      fig_mod.set_coord(get_field_coord(field_.indexOf(*f_it, (f_it - field_.begin()))));
      fig_mod.set_name(QString(f_it->isLower() ? "w_" : "b_") + *f_it);
      fig_mod.set_visible(true);
      ++f_it;
    }
    else fig_mod.set_visible(false);
  }
}

Coord board_graphic_t::get_field_coord(const int i) const
{
  return Coord(i % CELL_NUM, i / CELL_NUM);
}

void board_graphic_t::set_board_mask(const QString& mask)
{
  if (mask.size() != CELL_NUM * CELL_NUM)
  {
    SPDLOG_ERROR("Wrong board mask size");
    return;
  }

  field_.clear();
  field_ = mask;

  update_coordinates();
}

void board_graphic_t::set_moves_history(const QString& history)
{
  str_moves_history_.clear();

  const int NEED_SIMB_TO_MOVE = 4;

  QString move;
  for (const auto& simb : history)
  {
    move.append(simb);

    if (move.size() == NEED_SIMB_TO_MOVE)
    {
      move.insert(2, " - ");
      str_moves_history_.append(move);
      move.clear();
    }
  }

  emit moves_history_changed();
}

void board_graphic_t::set_move_color(const int move_num)
{
  move_color_ = (move_num % 2 == 0) ? MOVE_COLOR_W : MOVE_COLOR_B;
  emit move_turn_color_changed();
}

const QString board_graphic_t::coord_to_str(const Coord& from, const Coord& to) const
{
  return (QChar(a_LETTER + from.x) + QString::number(CELL_NUM - from.y) + " - " + QChar(a_LETTER + to.x) + QString::number(CELL_NUM - to.y));
}

void board_graphic_t::update_hilight(const int move_num, const QString& history)
{
  enum HILIGHT
  {
    FIRST_HILIGHT = 32,
    SECOND_HILIGHT = 33
  };

  const int CHAR_IN_MOVE = 4;
  if (move_num && history.size() >= move_num * CHAR_IN_MOVE)
  {
    auto simb = history.begin();
    simb += (move_num - 1) * CHAR_IN_MOVE;
    Coord c;
    for (int i = 0; i < 2; ++i)
    {
      c.x = (*(simb++)).unicode() - a_LETTER;
      c.y = CELL_NUM - (*(simb++)).digitValue();
      HILIGHT ind = (i == 0) ? SECOND_HILIGHT : FIRST_HILIGHT;
      figures_model_[ind].set_visible(true);
      figures_model_[ind].set_coord(c);
    }
  }
}

void board_graphic_t::get_login(const QString& error_mess)
{
  //  set_login("asdasd" + (error_mess.isEmpty() ? QString() : QString("111111111111111")), "asd23");
  emit enter_login(error_mess);
}

void board_graphic_t::redraw_board()
{
  QModelIndex topLeft = index(0, 0);
  QModelIndex bottomRight = index(figures_model_.size() - 1, 0);
  emit dataChanged(topLeft, bottomRight);
}

bool board_graphic_t::is_check_mate() const
{
  return check_mate_;
}

void board_graphic_t::set_check_mate()
{
  check_mate_ = true;
  emit check_mate();
}

void board_graphic_t::add_to_command_history(const QString& str)
{
  commands_history_.append(str);
  emit commands_hist_changed();
}

void board_graphic_t::path_to_file(QString& path, bool is_moves_from_file)
{
  for (int i = path.indexOf("/"); !path[path.indexOf("/", i) + 1].isLetter(); ++i)
  {
    path.remove(0, i);
  }

  if (is_moves_from_file)
  {
    read_moves_from_file(path);
  }
  else
  {
    path += "/chess_hist.txt";
    write_moves_to_file(path);
  }
}

void board_graphic_t::write_moves_to_file(const QString& path)
{
  std::ofstream in_file(path.toStdString());
  if (!in_file.is_open())
  {
    SPDLOG_ERROR("Couldn't open file.");
    return;
  }
  for (auto& s : str_moves_history_)
  {
    in_file << s.toStdString();
    in_file << FREE_SPACE;
  }
  in_file.close();
}

void board_graphic_t::read_moves_from_file(const QString& path)
{
  std::ifstream from_file(path.toStdString());
  if (!from_file.is_open())
  {
    SPDLOG_ERROR("Couldn't open file={}", path);
    return;
  }

  std::string data_from_file(std::istream_iterator<char>(from_file), (std::istream_iterator<char>()));

  command_requested(msg::move_t(data_from_file));
}

void board_graphic_t::set_connect_status(const int status)
{
  switch (status)
  {
    case msg::id<msg::server_here_t>():
      if (udp_connection_status_ == "Disconnected")
      {
        udp_connection_status_ = "Connect";
      }
      break;
    case msg::id<msg::server_lost_t>(): udp_connection_status_ = "Disconnected"; break;
    case msg::id<msg::opponent_lost_t>(): udp_connection_status_ = "Opponent disconnected"; break;
    default: SPDLOG_ERROR("Unknown status={}", status); return;
  }
  emit udp_connection_status_changed();
}

bool board_graphic_t::set_login(const QString& login, const QString& pwd)
{
  for (auto i : login)
  {
    if (!i.isLetterOrNumber())
    {
      return false;
    }
  }

  for (auto i : pwd)
  {
    if (!i.isLetterOrNumber())
    {
      return false;
    }
  }

  command_requested(msg::login_t(login.toStdString(), pwd.toStdString()));
  return true;
}

void board_graphic_t::set_cell_size(const int width, const int height)
{
  SPDLOG_INFO("Cell size: width={}; height={}", width, height);
  cell_width_ = width;
  cell_height_ = height;
}

QStringList board_graphic_t::get_moves_history() const
{
  return str_moves_history_;
}

QStringList board_graphic_t::get_commands_hist() const
{
  return commands_history_;
}

int board_graphic_t::get_last_command_hist_ind() const
{
  return commands_history_.size() - 1;
}

QString board_graphic_t::get_move_turn_color() const
{
  return move_color_;
}

QString board_graphic_t::get_udp_connection_status() const
{
  return udp_connection_status_;
}

void board_graphic_t::addFigure(const figure_t& figure)
{
  beginInsertRows(QModelIndex(), rowCount(), rowCount());
  figures_model_ << figure;
  endInsertRows();
}

int board_graphic_t::rowCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);
  return figures_model_.count();
}

QVariant board_graphic_t::data(const QModelIndex& index, int role) const
{
  if (index.row() < 0 || index.row() >= figures_model_.count())
  {
    return QVariant();
  }

  const figure_t& figure = figures_model_[index.row()];
  if (role == NameRole)
  {
    return figure.name();
  }
  else if (role == XRole)
  {
    return figure.x();
  }
  else if (role == YRole)
  {
    return figure.y();
  }
  else if (role == VisibleRole)
  {
    return figure.visible();
  }
  return QVariant();
}

QHash<int, QByteArray> board_graphic_t::roleNames() const
{
  QHash<int, QByteArray> roles;
  roles[NameRole] = "figure_name";
  roles[XRole] = "x_coord";
  roles[YRole] = "y_coord";
  roles[VisibleRole] = "figure_visible";
  return roles;
}

} // namespace graphic
