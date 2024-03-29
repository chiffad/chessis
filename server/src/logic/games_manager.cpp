#include "logic/games_manager.hpp"

#include <spdlog/spdlog.h>

namespace logic {

games_manager_t::games_manager_t(io_service_t& io_service)
  : players_{}
  , boards_{}
{}

board_logic_t& games_manager_t::board(const board_logic_t::uuid_t& uuid)
{
  return boards_.at(uuid);
}

const board_logic_t& games_manager_t::board(const board_logic_t::uuid_t& uuid) const
{
  return boards_.at(uuid);
}

player_t::uuid_t games_manager_t::add_player(const endpoint_t& addr)
{
  const auto uuid = players_.add(addr);
  free_players_.push_back(uuid);
  return uuid;
}

std::optional<player_t*> games_manager_t::player(const endpoint_t& addr)
{
  const auto it = players_.find(addr);
  if (it == players_.end()) return std::nullopt;

  return &it->second;
}

player_t& games_manager_t::player(const player_t::uuid_t& uuid)
{
  return players_.at(uuid);
}

const player_t& games_manager_t::player(const player_t::uuid_t& uuid) const
{
  return players_.at(uuid);
}

std::optional<player_t::uuid_t> games_manager_t::free_player(const player_t& not_this) const
{
  SPDLOG_DEBUG("Free players number={}", free_players_.size());
  for (const auto& cl : free_players_)
  {
    if (cl != not_this.uuid()) return cl;
  }

  return std::nullopt;
}

std::optional<board_logic_t::uuid_t> games_manager_t::board(const player_t& player) const
{
  const auto it = player_to_board_map_.find(player.uuid());
  if (it == player_to_board_map_.end()) return std::nullopt;

  return it->second;
}

void games_manager_t::start_game(const player_t::uuid_t& player_1, const player_t::uuid_t& player_2)
{
  unbook_player(player_1);
  unbook_player(player_2);

  const auto board_uuid = boards_.add();
  SPDLOG_INFO("Start game beween player={} and player={}; board={}", player_1, player_2, board_uuid);

  const bool white_player = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() % 2 == 0;
  players_.at(player_1).set_playing_white(white_player);
  players_.at(player_2).set_playing_white(!white_player);

  book_players(player_1, player_2, board_uuid);
}

std::optional<player_t::uuid_t> games_manager_t::opponent(const player_t::uuid_t& player_uuid) const
{
  const auto player_2_board_it = player_to_board_map_.find(player_uuid);
  if (player_2_board_it == player_to_board_map_.end()) return std::nullopt;

  const auto board_2_player_it = board_to_player_map_.find(player_2_board_it->second);
  if (board_2_player_it == board_to_player_map_.end()) return std::nullopt;

  for (const auto& el : board_2_player_it->second)
  {
    if (el != player_uuid) return el;
  }

  return std::nullopt;
}

void games_manager_t::book_players(const player_t::uuid_t& player_1, const player_t::uuid_t& player_2, const board_logic_t::uuid_t& board_uuid)
{
  player_to_board_map_[player_1] = board_uuid;
  player_to_board_map_[player_2] = board_uuid;
  board_to_player_map_[board_uuid] = {player_1, player_2};
  remove_from_free_players({player_1, player_2});
}

void games_manager_t::unbook_player(const player_t::uuid_t& player)
{
  const auto player_2_board_it = player_to_board_map_.find(player);
  if (player_2_board_it == player_to_board_map_.end()) return;

  free_players_.push_back(player);

  const auto board_2_player_it = board_to_player_map_.find(player_2_board_it->second);
  player_to_board_map_.erase(player_2_board_it);

  if (board_2_player_it == board_to_player_map_.end()) return;

  board_2_player_it->second.erase(player);
  if (board_2_player_it->second.empty())
  {
    boards_.erase(player_2_board_it->second);
    board_to_player_map_.erase(board_2_player_it);
  }
}

void games_manager_t::remove_from_free_players(std::set<player_t::uuid_t> uuids)
{
  for (auto it = free_players_.begin(); !uuids.empty() && it != free_players_.end();)
  {
    if (uuids.count(*it))
    {
      uuids.erase(*it);
      it = free_players_.erase(it);
      continue;
    }

    ++it;
  }
}

} // namespace logic