#include "board_graphic.h"

#include <QPainter>
#include <ctype.h>
#include <QModelIndex>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <QChar>

#include "helper.h"


using namespace graphic;

board_graphic_t::board_graphic_t()
    : QAbstractListModel(nullptr), _move_color(MOVE_COLOR_W),
      _udp_connection_status("Disconnected"), _is_check_mate(false)
{
  enum {HILIGHT_CELLS = 2, FIGURES_NUMBER = 32};
  const QString HILIGHT_IM = "hilight";

  for(int i = 0; i < FIGURES_NUMBER + HILIGHT_CELLS; ++i)
    { addFigure(figure_t(HILIGHT_IM, 0, 0, false)); }
}

void board_graphic_t::run_command(const QString& message, const int x1, const int y1, const int x2, const int y2)
{
  const QString HELP_WORD = "help";
  const QString MOVE_WORD = "move";
  const QString BACK_MOVE = "back";
  const QString SHOW_ME = "show me";
  const QString NEW_GAME = "new game";
  const QString HISTORY = "to history";
  const QString SHOW_OPPONENT = "show opponent";

  cl::helper::log("board_graphic_t::run_command: ", message, " x1: ", x1, "; y1: ", y1, "; x2: ", x2,  "; y2: ", y2);

  if(message == HELP_WORD)
  {
    cl::helper::log("run_comman: dhelp_word");
    add_to_command_history("1.For move, type '" + MOVE_WORD + "' and coordinates(example: " + MOVE_WORD + " d2-d4)" + "\n"+
    "2.For back move, type '" + BACK_MOVE + "'" + "\n"+
    "3.For start new game, type '" + NEW_GAME + "'" + "\n"+
    "4.For go to history index, type '" + HISTORY + "' and index" + "\n"+
    "5.To view opponent information, print '" + SHOW_OPPONENT + "'" + "\n"+
    "6.To view your information, print '" + SHOW_ME + "'");
  }
  else if(message == SHOW_OPPONENT)
    { add_to_messages_for_server(messages::OPPONENT_INF); }
  else if(message == SHOW_ME)
    { add_to_messages_for_server(messages::MY_INF); }
  else if(message == NEW_GAME)
    { add_to_messages_for_server(messages::NEW_GAME); }
  else if(message == BACK_MOVE)
    { add_to_messages_for_server(messages::BACK_MOVE); }
  else if(message == HISTORY)
  {
    messages::go_to_history_t gtp;
    gtp.hist_ind = x1+1;
    add_to_messages_for_server(messages::GO_TO_HISTORY, QString::fromStdString(gtp.to_json()));
    add_to_command_history("command: " + message + " " + QString::number(x1 + 1));
    return;
  }
  else if(message.contains(HISTORY))
  {
    messages::go_to_history_t gtp;
    gtp.hist_ind = message.mid(HISTORY.size() + 1).toInt();
    add_to_messages_for_server(messages::GO_TO_HISTORY, QString::fromStdString(gtp.to_json()));
  }

  else
  {
    const QString command(message.mid(0,message.indexOf(FREE_SPACE)));
    const QString command_content(message.mid(command.size()));

    if(command == MOVE_WORD)
    {
      messages::move_t move;
      if(!command_content.isEmpty())
      {
        move.data = command_content.toStdString();
        add_to_messages_for_server(messages::MOVE, QString::fromStdString(move.to_json()));
      }
      else if(x1 + x2 + y1 + y2 != 0)
      {
        move.data = coord_to_str(get_coord(x1, y1), get_coord(x2, y2)).toStdString();
        add_to_messages_for_server(messages::MOVE, QString::fromStdString(move.to_json()));
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
  if(x < 0 || y < 0 || x > (CELL_SIZE_X * CELL_NUM) || y > (CELL_SIZE_Y * CELL_NUM))
  {
    cl::helper::log("Warning! in get_coord: Incorrect coord");
    return Coord (x,y);
  }

  return Coord ((x + (CELL_SIZE_X / 2)) / CELL_SIZE_X,
                (y + (CELL_SIZE_Y / 2)) / CELL_SIZE_Y);
}

void board_graphic_t::update_coordinates()
{
  const auto FREE_FIELD = '.';

  auto f_it = _field.begin();
  for(auto &fig_mod : _figures_model)
  {
    f_it = std::find_if(f_it, _field.end(), [&FREE_FIELD](auto const &i) {return i != FREE_FIELD;});
    if(f_it != _field.end())
    {
      fig_mod.set_coord(get_field_coord(_field.indexOf(*f_it, (f_it - _field.begin()))));
      fig_mod.set_name(QString(f_it->isLower() ? "w_" : "b_") + *f_it);
      fig_mod.set_visible(true);
      ++f_it;
    }
    else fig_mod.set_visible(false);
  }
}

Coord board_graphic_t::get_field_coord(const int i) const
{
  return Coord (i % CELL_NUM, i / CELL_NUM);
}

void board_graphic_t::set_board_mask(const QString& mask)
{
  if(mask.size() != CELL_NUM * CELL_NUM)
  {
    cl::helper::log("Warning! in set_board_mask: Wrong board mask size");
    return;
  }

  _field.clear();
  _field = mask;

  update_coordinates();
}

void board_graphic_t::set_moves_history(const QString& history)
{
  _str_moves_history.clear();

  const int NEED_SIMB_TO_MOVE = 4;

  QString move;
  for(const auto &simb : history)
  {
    move.append(simb);

    if(move.size() == NEED_SIMB_TO_MOVE)
    {
      move.insert(2, " - ");
      _str_moves_history.append(move);
      move.clear();
    }
  }

  emit moves_history_changed();
}

void board_graphic_t::set_move_color(const int move_num)
{
  _move_color = (move_num % 2 == 0) ? MOVE_COLOR_W : MOVE_COLOR_B;
  emit move_turn_color_changed();
}

const QString board_graphic_t::coord_to_str(const Coord& from, const Coord& to) const
{
  return (QChar(a_LETTER + from.x) + QString::number(CELL_NUM - from.y)
          + " - " + QChar(a_LETTER + to.x) + QString::number(CELL_NUM - to.y));
}

void board_graphic_t::add_to_messages_for_server(const messages::MESSAGE mes_type, const QString& content)
{
  _messages_for_server.push_back(QString::number(mes_type) + FREE_SPACE + content);
}

void board_graphic_t::update_hilight(const int move_num, const QString& history)
{
  enum HILIGHT {FIRST_HILIGHT = 32, SECOND_HILIGHT = 33};

  const int CHAR_IN_MOVE = 4;
  if(move_num && history.size() >= move_num * CHAR_IN_MOVE)
  {
    auto simb = history.begin();
    simb += (move_num-1) * CHAR_IN_MOVE;
    Coord c;
    for(int i = 0; i < 2; ++i)
    {
      c.x = (*(simb++)).unicode() - a_LETTER;
      c.y = CELL_NUM - (*(simb++)).digitValue();
      HILIGHT ind = (i == 0) ? SECOND_HILIGHT : FIRST_HILIGHT;
      _figures_model[ind].set_visible(true);
      _figures_model[ind].set_coord(c);
    }
  }
}

void board_graphic_t::get_login(const QString& error_mess)
{
//  set_login("asdasd" + (error_mess.isEmpty() ? QString() : QString("111111111111111")));
  emit enter_login(error_mess);
}

void board_graphic_t::redraw_board()
{
  QModelIndex topLeft = index(0, 0);
  QModelIndex bottomRight = index(_figures_model.size() - 1, 0);
  emit dataChanged(topLeft, bottomRight);
}

bool board_graphic_t::is_check_mate() const
{
  return _is_check_mate;
}

void board_graphic_t::set_check_mate()
{
  _is_check_mate = true;
  emit check_mate();
}

void board_graphic_t::add_to_command_history(const QString& str)
{
  _commands_history.append(str);
  emit commands_hist_changed();
}

void board_graphic_t::path_to_file(QString& path, bool is_moves_from_file)
{
  for(int i = path.indexOf("/"); !path[path.indexOf("/", i) + 1].isLetter(); ++i)
    { path.remove(0,i); }

  if(is_moves_from_file)
    { read_moves_from_file(path); }
  else
  {
    path += "/chess_hist.txt";
    write_moves_to_file(path);
  }
}

void board_graphic_t::write_moves_to_file(const QString& path)
{
  std::ofstream in_file(path.toStdString());
  if(!in_file.is_open())
  {
    cl::helper::log("Warning! in write_moves_to_file: Couldn't open file.");
    return;
  }
  for(auto &s : _str_moves_history)
  {
    in_file<<s.toStdString();
    in_file<<FREE_SPACE;
  }
  in_file.close();
}

void board_graphic_t::read_moves_from_file(const QString& path)
{
  std::ifstream from_file(path.toStdString());
  if(!from_file.is_open())
  {
    cl::helper::log("Warning! read_moves_from_file: Couldn't open file.");
    return;
  }

  std::string data_from_file(std::istream_iterator<char>(from_file), (std::istream_iterator<char>()));

  add_to_messages_for_server(messages::MOVE, QString::fromStdString(data_from_file));
}

void board_graphic_t::set_connect_status(const int status)
{
  switch(status)
  {
    case messages::SERVER_HERE:
      if(_udp_connection_status == "Disconnected")
        { _udp_connection_status = "Connect"; }
      break;
    case messages::SERVER_LOST:
      _udp_connection_status = "Disconnected";
      break;
    case messages::OPPONENT_LOST:
      _udp_connection_status = "Opponent disconnected";
      break;
    default:
      cl::helper::log("Warning! in set_connect_status: Unknown status");
      return;
  }
  emit udp_connection_status_changed();
}

bool board_graphic_t::set_login(const QString& login)
{
  for(auto i : login)
  {
    if(!i.isLetterOrNumber())
      { return false; }
  }

  messages::login_t l;
  l.login = login.toStdString();
  add_to_messages_for_server(messages::LOGIN, QString::fromStdString(l.to_json()));
  return true;
}

bool board_graphic_t::is_message_appear() const
{
  return !_messages_for_server.empty();
}

const QString board_graphic_t::pull()
{
  QString command(_messages_for_server.front());
  _messages_for_server.erase(_messages_for_server.begin());
  return command;
}

QStringList board_graphic_t::get_moves_history() const
{
  return _str_moves_history;
}

QStringList board_graphic_t::get_commands_hist() const
{
  return _commands_history;
}

int board_graphic_t::get_last_command_hist_ind() const
{
  return _commands_history.size() - 1;
}

QString board_graphic_t::get_move_turn_color() const
{
  return _move_color;
}

QString board_graphic_t::get_udp_connection_status() const
{
  return _udp_connection_status;
}

void board_graphic_t::addFigure(const figure_t& figure)
{
  beginInsertRows(QModelIndex(), rowCount(), rowCount());
  _figures_model << figure;
  endInsertRows();
}

int board_graphic_t::rowCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);
  return _figures_model.count();
}

QVariant board_graphic_t::data(const QModelIndex& index, int role) const
{
  if (index.row() < 0 || index.row() >= _figures_model.count())
    { return QVariant(); }

  const figure_t &figure = _figures_model[index.row()];
  if (role == NameRole)
    { return figure.name(); }
  else if (role == XRole)
    { return figure.x(); }
  else if (role == YRole)
    { return figure.y(); }
  else if (role == VisibleRole)
    { return figure.visible(); }
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
