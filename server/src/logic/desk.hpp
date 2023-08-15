#pragma once

#include "logic/chess.hpp"

#include <memory>
#include <string>

namespace server {
class client_t;
}

namespace logic {

class desk_t : public board_t
{
public:
  desk_t(const std::weak_ptr<const server::client_t> _1, const std::weak_ptr<const server::client_t> _2);
  ~desk_t();
  void make_moves_from_str(const std::string& str);
  bool is_contain_player(const std::weak_ptr<server::client_t>& _1) const;
  const std::weak_ptr<const server::client_t> get_opponent(const std::shared_ptr<const server::client_t>& _1) const;

  void load_moves_from_file(const std::string& path);
  void write_moves_to_file(const std::string& path) const;

public:
  desk_t(const desk_t&) = delete;
  desk_t& operator=(const desk_t&) = delete;

private:
  struct impl_t;
  std::unique_ptr<impl_t> impl_;
};

} // namespace logic
