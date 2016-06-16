#include <QPainter>
#include <QString>
#include <QChar>
#include <ctype.h>
#include <QModelIndex>
#include <iostream>
#include <fstream>
#include <sstream>
#include "headers/board_graphic.h"
#include "headers/udp_client.h"


Board_graphic::Board_graphic(QObject *parent) : QAbstractListModel(parent), _move_color(MOVE_COLOR_W),
                                                     _udp_connection_status(DISCONNECT), _is_message_from_server(false)
{
  _udp_client = new UDP_client();

  for(int i = 0; i < FIGURES_NUMBER; ++i)
    addFigure(Figure(MOVE_COLOR_W, 0, 0, true));

  addFigure(Board_graphic::Figure(HILIGHT_IM, 0, 0, false));
  addFigure(Board_graphic::Figure(HILIGHT_IM, 0, 0, false));
  update_coordinates();

  connect(_udp_client, SIGNAL(some_data_came()), this, SLOT(read_data_from_udp()));
}

Board_graphic::Figure::Figure(const QString& name, const int x, const int y, const bool visible)
    : _name(name), _x(x), _y(y), _visible(visible)
{
}

void Board_graphic::move(const unsigned x, const unsigned y, bool is_correct_coord)
{
  static bool is_from = true;
  if(is_from)
  {
    correct_figure_coord(_from,x,y,is_correct_coord);
      //update_hilight(from, FIRST_HILIGHT);
  }
  else
  {
    is_from = true;
    correct_figure_coord(_to, x, y, is_correct_coord);

    add_to_commands_stack(MOVE, move_coord_to_str(_from, _to));
   // update_hilight(to, SECOND_HILIGHT);
  }
}

void Board_graphic::back_move()
{
  qDebug()<<"====back_move";
  add_to_commands_stack(BACK_MOVE);
}

void Board_graphic::correct_figure_coord(Coord& coord, const unsigned x, const unsigned y, bool is_correct)
{
  coord.x = is_correct ? x : (x + IMG_MID) / CELL_SIZE;
  coord.y = is_correct ? y : (y + IMG_MID) / CELL_SIZE;
}

void Board_graphic::update_coordinates()
{
  for(int i = 0; i < rowCount() - HILIGHT_CELLS; ++i)
  {
    _figures_model[i].set_visible(false);
    emit_figure_changed(i);
  }

  int index = 0;
  Coord coord;
  for(coord.x = 0; coord.x < BOARD_SIZE; ++coord.x)
    for(coord.y = 0; coord.y < BOARD_SIZE; ++coord.y)
      if(_field[coord.x][coord.y] != FREE_FIELD)
      {
        QString fig_name_color = _field[coord.x][coord.y].isLower() ? "w_" : "b_";
        fig_name_color.append(_field[coord.x][coord.y]);

        _figures_model[index].set_coord(coord);
        _figures_model[index].set_name(fig_name_color);
        _figures_model[index].set_visible(true);

        emit_figure_changed(index);
        ++index;
      }
  update_move_color();
}

void Board_graphic::set_board_mask(const QString& mask)
{
  if(mask.size() != BOARD_SIZE * BOARD_SIZE)
  {
    qDebug()<<"mask is wrong!"<<mask;
    return;
  }

  int i = 0;
  for(int x = 0; x < BOARD_SIZE; ++x)
    for(int y = 0; y < BOARD_SIZE; ++y, ++i)
      _field[x][y] = mask[i];

qDebug()<<"mask: "; //temporary thing
  for(int x = 0, i = 0; x < BOARD_SIZE; ++x, ++i)
    for(int y = 0; y < BOARD_SIZE; ++y, ++i)
      qDebug()<<_field[x][y];
}

void Board_graphic::set_moves_history(const QString& history)
{
  _str_moves_history.clear();

  QString move;
  for(int i = 0; i < history.size(); ++i)
  {
    if(history[i].isLetter() && (move.isEmpty() || move[move.size() - 1].isNumber()))
      move.push_back(history[i]);
    else if(history[i].isNumber() && !move.isEmpty() && move[move.size() - 1].isLetter())
      move.push_back(history[i]);

    if(move.size() == NEED_SIMB_TO_MOVE)
    {
      move.insert(2, " - ");
      _str_moves_history.append(move);
      move.clear();
    }
  }
  emit moves_history_changed();
}

const QString Board_graphic::move_coord_to_str(const Coord& from, const Coord& to) const
{
  return (QChar(a_LETTER + from.x) + QString::number(BOARD_SIZE - from.y)
          + " - " + QChar(a_LETTER + to.x) + QString::number(BOARD_SIZE - to.y));
}

void Board_graphic::add_to_commands_stack(MESSAGE_TYPE type, const QString& content)
{
  qDebug()<<"====send_data_on_server_chess";

  QString message;
  message.setNum(type);
  message.append(content);

  _commands_stack.append(message);
}

void Board_graphic::update_hilight(const Coord& coord, HILIGHT hilight_index)
{
  _figures_model[hilight_index].set_visible(true);
  _figures_model[hilight_index].set_coord(coord);

  if(hilight_index == FIRST_HILIGHT)
  {
    _figures_model[SECOND_HILIGHT].set_visible(false);
    emit_figure_changed(SECOND_HILIGHT);
  }
  emit_figure_changed(hilight_index);
}

void Board_graphic::update_move_color()
{
  _move_color = (_commands_history.size() % 2 == 0) ? MOVE_COLOR_W : MOVE_COLOR_B;
  emit move_turn_color_changed();
}

void Board_graphic::emit_figure_changed(const unsigned INDEX)
{
  QModelIndex topLeft = index(INDEX, 0);
  QModelIndex bottomRight = index(INDEX, 0);
  emit dataChanged(topLeft, bottomRight);
}

bool Board_graphic::is_check_mate() const
{
  /*if(!_board->is_mate(_board->get_move_color()))
    return false;
*/
  //emit check_mate();
  //return true;
    return false; // temporary
}

void Board_graphic::start_new_game()
{
  qDebug()<<"====start_new_game";
  add_to_commands_stack(NEW_GAME);
}

void Board_graphic::go_to_history_index(const unsigned index)
{
  qDebug()<<"====go to history index: " <<index;
  add_to_commands_stack(GO_TO_HISTORY, QString::number(index));
}

void Board_graphic::run_command(const QString& command)
{
  qDebug()<<"===run_command: "<<command;
  _commands_history.append(command);

  const QString HELP_WORD = "help";
  const QString MOVE_WORD = "move";
  const QString SHOW_OPPONENT = "show opponent";
  const QString SHOW_ME = "show me";

  if(command == HELP_WORD)
  {
    qDebug()<<"help_word";
    add_to_command_history("For move type '" + MOVE_WORD + "' and coordinates(example: " + MOVE_WORD + "d2-d4).");
    add_to_command_history("To see opponent information, print '" + SHOW_OPPONENT + " .");
    add_to_command_history("To view your information, print '" + SHOW_ME + " .");
  }
  //else if(command == SHOW_OPPONENT)
    //send_data_on_server(OPPONENT_INF_REQUEST);
  //else if(command == SHOW_ME)
  //  send_data_on_server(MY_INF_REQUEST);
  else
  {
    QString command_copy = command;
    QString first_word;
    while(command_copy.size())
    {
      if(command_copy[0].isLetter())
        first_word.append(command_copy[0]);

      command_copy.remove(0,1);
      if(first_word.size() == 4)
        break;
    }
    if(first_word == MOVE_WORD)
    {
     // make_move_from_str(command_copy);
      qDebug()<<"move word";
    }
    else add_to_command_history("Unknown command ('" + HELP_WORD + "' for help).");
  }
}

void Board_graphic::add_to_command_history(const QString& str)
{
  _commands_history.append(str);
  emit commands_list_changed();
}

void Board_graphic::path_to_file(QString& path, bool is_moves_from_file)
{
  path.remove(0,7);
  qDebug()<<"===path_to_file: "<<path;
  is_moves_from_file ? read_moves_from_file(path) : write_moves_to_file(path);
}

void Board_graphic::write_moves_to_file(const QString& path)
{
  std::ofstream in_file(path.toUtf8().constData());
  for(int i = 0; i < _str_moves_history.size(); ++i)
  {
    in_file<<_str_moves_history[i].toUtf8().constData();
    in_file<<FREE_SPACE;
  }
  in_file.close();
}

void Board_graphic::read_moves_from_file(const QString& path)
{
  qDebug()<<"===read_move_from_file: "<<path;
  std::ifstream from_file(path.toUtf8().constData());
  std::ostringstream out;
  out<<from_file.rdbuf();

  std::string data_from_file;
  out.str().swap(data_from_file);

  from_file.close();

  start_new_game();
  std::cout<<"data_from_file std: "<<data_from_file<<std::endl;
  qDebug()<<QString::fromStdString(data_from_file);

}

void Board_graphic::set_connect_status(const QString& status)
{
  if(_udp_connection_status == status)
    return;
  _udp_connection_status = status;
  emit udp_connection_status_changed();
}

bool Board_graphic::is_new_command_appear() const
{
  return _commands_stack.size();
}

const QString Board_graphic::pull_first_command()
{
  QString command(_commands_stack.first());
  _commands_stack.removeFirst();
  return command;
}

QStringList Board_graphic::get_moves_history() const
{
  return _str_moves_history;
}

QStringList Board_graphic::get_commands_list() const
{
  return _commands_history;
}

QString Board_graphic::get_move_turn_color() const
{
  return _move_color;
}

QString Board_graphic::get_udp_connection_status() const
{
  return _udp_connection_status;
}

QChar Board_graphic::letter_return(const unsigned index) const
{
  return QChar(a_LETTER + index);
}

void Board_graphic::addFigure(const Figure &figure)
{
  beginInsertRows(QModelIndex(), rowCount(), rowCount());
  _figures_model << figure;
  endInsertRows();
}

int Board_graphic::rowCount(const QModelIndex & parent) const
{
  Q_UNUSED(parent);
  return _figures_model.count();
}

QVariant Board_graphic::data(const QModelIndex & index, int role) const
{
  if (index.row() < 0 || index.row() >= _figures_model.count())
    return QVariant();

  const Figure &figure = _figures_model[index.row()];
  if (role == NameRole)
    return figure.name();
  else if (role == XRole)
    return figure.x();
  else if (role == YRole)
    return figure.y();
  else if (role == VisibleRole)
    return figure.visible();
  return QVariant();
}

QHash<int, QByteArray> Board_graphic::roleNames() const
{
  QHash<int, QByteArray> roles;
  roles[NameRole] = "figure_name";
  roles[XRole] = "x_coord";
  roles[YRole] = "y_coord";
  roles[VisibleRole] = "figure_visible";
  return roles;
}
