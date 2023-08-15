#pragma once

#include "logic/board_logic.hpp"

#include <memory>
#include <string>

namespace server {
class client_t;
}

namespace logic {

class desk_t : public board_logic_t
{
public:
  desk_t(const std::weak_ptr<const server::client_t> _1, const std::weak_ptr<const server::client_t> _2);
  ~desk_t() override;

  desk_t(const desk_t&) = delete;
  desk_t& operator=(const desk_t&) = delete;

  bool contains_player(const std::weak_ptr<server::client_t>& _1) const;
  std::weak_ptr<const server::client_t> get_opponent(const std::shared_ptr<const server::client_t>& _1) const;

private:
  struct impl_t;
  std::unique_ptr<impl_t> impl_;
};

void make_moves_from_str(const std::string& str, board_logic_t& desk);
void load_moves_from_file(const std::string& path, board_logic_t& desk);
void write_moves_to_file(const std::string& path, const desk_t& desk);

} // namespace logic
