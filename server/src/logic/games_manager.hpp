#pragma once

#include "logic/boards_holder.hpp"
#include "logic/players_holder.hpp"

#include <map>
#include <optional>
#include <set>

namespace chess::logic {

class games_manager_t
{
public:
  explicit games_manager_t(io_service_t& io_service);

  board_logic_t::uuid_t start_game(const client_uuid_t& player_1, const client_uuid_t& player_2);

  board_logic_t& board(const board_logic_t::uuid_t& uuid);
  const board_logic_t& board(const board_logic_t::uuid_t& uuid) const;

  player_t& add_player(const client_uuid_t& uuid);
  bool count(const client_uuid_t& addr) const;
  player_t& player(const client_uuid_t& uuid);
  const player_t& player(const client_uuid_t& uuid) const;

  std::optional<client_uuid_t> opponent_uuid(const client_uuid_t& player) const;
  std::optional<client_uuid_t> free_player_uuid(const client_uuid_t& not_this) const;
  std::optional<board_logic_t::uuid_t> board_uuid(const client_uuid_t& player_uuid) const;

private:
  void book_players(const client_uuid_t& player_1, const client_uuid_t& player_2, const board_logic_t::uuid_t& board_uuid);
  void unbook_player(const client_uuid_t& player);

private:
  players_holder_t players_;
  boards_holder_t boards_;

  std::set<client_uuid_t> free_players_;
  std::map<board_logic_t::uuid_t, std::set<client_uuid_t>> board_to_player_map_;
  std::map<client_uuid_t, board_logic_t::uuid_t> player_to_board_map_;
};

} // namespace chess::logic
