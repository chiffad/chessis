#pragma once

#include "logic/boards_holder.hpp"
#include "logic/players_holder.hpp"

#include <list>
#include <map>
#include <optional>
#include <set>

namespace chess::logic {

class games_manager_t
{
public:
  explicit games_manager_t(io_service_t& io_service);
  board_logic_t& board(const board_logic_t::uuid_t& uuid);
  const board_logic_t& board(const board_logic_t::uuid_t& uuid) const;

  player_t& add_player(const player_t::uuid_t& uuid);
  bool count(const player_t::uuid_t& addr) const;
  player_t& player(const player_t::uuid_t& uuid);
  const player_t& player(const player_t::uuid_t& uuid) const;

  void start_game(const player_t::uuid_t& player_1, const player_t::uuid_t& player_2);
  std::optional<player_t::uuid_t> opponent(const player_t::uuid_t& player) const;
  std::optional<player_t::uuid_t> free_client(const player_t& not_this) const;
  std::optional<board_logic_t::uuid_t> board(const player_t& player) const;

  std::optional<player_t::uuid_t> free_player(const player_t& not_this) const;

private:
  void book_players(const player_t::uuid_t& player_1, const player_t::uuid_t& player_2, const board_logic_t::uuid_t& board_uuid);
  void unbook_player(const player_t::uuid_t& player);
  void remove_from_free_players(std::set<player_t::uuid_t> uuid);

private:
  players_holder_t players_;
  boards_holder_t boards_;

  std::list<player_t::uuid_t> free_players_;
  std::map<board_logic_t::uuid_t, std::set<player_t::uuid_t>> board_to_player_map_;
  std::map<player_t::uuid_t, board_logic_t::uuid_t> player_to_board_map_;
};

} // namespace chess::logic