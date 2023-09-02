#include "controller/menu_layout.hpp"

#include <algorithm>
#include <ctype.h>
#include <fstream>
#include <iostream>
#include <spdlog/spdlog.h>
#include <sstream>

#include "helper.h"

namespace {
const char FREE_SPACE = ' ';

constexpr const char* const HELP_WORD = "help";
constexpr const char* const BACK_MOVE = "back";
constexpr const char* const MOVE_WORD = "move";
constexpr const char* const SHOW_ME = "show me";
constexpr const char* const NEW_GAME = "new game";
constexpr const char* const HISTORY = "to history";
constexpr const char* const SHOW_OPPONENT = "show opponent";
const QString COMMAND = "Command: ";
} // namespace

namespace controller {

menu_layout_t::menu_layout_t(const command_requested_callback_t& callback)
  : QObject()
  , white_move_turn_(true)
  , connection_status_("Disconnected")
  , command_requested_callback_(callback)
{}

menu_layout_t::~menu_layout_t() = default;

void menu_layout_t::update_game_info(const msg::game_inf_t& game_info)
{
  set_move_turn(game_info.move_num);
  set_moves_history(QString::fromStdString(game_info.moves_history));
}

void menu_layout_t::new_game()
{
  command_requested(msg::new_game_t{});
  add_to_command_history(COMMAND + NEW_GAME);
}

void menu_layout_t::back_move()
{
  command_requested(msg::back_move_t{});
  add_to_command_history(COMMAND + BACK_MOVE);
}

void menu_layout_t::go_to_history(const int hist_i)
{
  msg::go_to_history_t gth;
  gth.index = hist_i + 1;
  add_to_command_history(COMMAND + HISTORY + " " + QString::number(gth.index));
  command_requested(gth);
}

void menu_layout_t::run_command(const QString& message)
{
  SPDLOG_DEBUG("run command={}", message);

  if (message == NEW_GAME)
  {
    new_game();
    return;
  }
  if (message == BACK_MOVE)
  {
    back_move();
    return;
  }
  if (message.contains(HISTORY))
  {
    go_to_history(message.mid(strlen(HISTORY) + 1).toInt());
    return;
  }

  if (message == HELP_WORD)
  {
    add_to_command_history(QString("1.For move, type '") + MOVE_WORD + "' and coordinates(example: " + MOVE_WORD + " d2-d4)" + "\n" +
                           "2.For back move, type '" + BACK_MOVE + "'" + "\n" + "3.For start new game, type '" + NEW_GAME + "'" + "\n" +
                           "4.For go to history index, type '" + HISTORY + "' and index" + "\n" + "5.To view opponent information, print '" + SHOW_OPPONENT +
                           "'" + "\n" + "6.To view your information, print '" + SHOW_ME + "'");
  }
  else if (message == SHOW_OPPONENT) command_requested(msg::opponent_inf_t{});
  else if (message == SHOW_ME) command_requested(msg::my_inf_t{});
  else if (message.contains(MOVE_WORD))
  {
    const QString command(message.mid(0, message.indexOf(FREE_SPACE)));
    const QString command_content(message.mid(command.size()));
    if (!command_content.isEmpty())
    {
      msg::move_t move;
      move.data = command_content.toStdString();
      command_requested(std::move(move));
    }
    else
    {
      add_to_command_history("Unknown command '" + message + "' (type '" + HELP_WORD + "' for help).");
      return;
    }
  }
  else
  {
    add_to_command_history("Unknown command '" + message + "' (type '" + HELP_WORD + "' for help).");
    return;
  }
  add_to_command_history(COMMAND + message);
}

void menu_layout_t::set_moves_history(const QString& history)
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

void menu_layout_t::set_move_turn(const int move_num)
{
  white_move_turn_ = move_num % 2 == 0;
  emit white_move_turn_changed();
}

void menu_layout_t::add_to_command_history(const QString& str)
{
  commands_history_.append(str);
  emit commands_hist_changed();
}

void menu_layout_t::path_to_file(QString& path, bool is_moves_from_file)
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

void menu_layout_t::write_moves_to_file(const QString& path)
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

void menu_layout_t::read_moves_from_file(const QString& path)
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

void menu_layout_t::set_connect_status(const connection_status_t status)
{
  switch (status)
  {
    case connection_status_t::server_available:
      if (connection_status_ == "Disconnected")
      {
        connection_status_ = "Connect";
      }
      break;
    case connection_status_t::server_lost: connection_status_ = "Disconnected"; break;
    case connection_status_t::opponent_lost: connection_status_ = "Opponent disconnected"; break;
    default: SPDLOG_ERROR("Unknown status"); return;
  }
  emit connection_status_changed();
}

QStringList menu_layout_t::moves_history() const
{
  return str_moves_history_;
}

QStringList menu_layout_t::commands_hist() const
{
  return commands_history_;
}

int menu_layout_t::last_command_hist_ind() const
{
  return commands_history_.size() - 1;
}

bool menu_layout_t::white_move_turn() const
{
  return white_move_turn_;
}

QString menu_layout_t::connection_status() const
{
  return connection_status_;
}

} // namespace controller
