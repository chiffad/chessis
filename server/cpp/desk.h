#ifndef __MY_DESK_H__BVGTYVFYTKUBLIUBHIYFRTDVGFABDJUIWOMJ
#define __MY_DESK_H__BVGTYVFYTKUBLIUBHIYFRTDVGFABDJUIWOMJ

#include <memory>
#include <string>

#include "chess.h"
#include "client.h"


namespace logic
{

class desk_t : public board_t
{
public:
  desk_t(std::weak_ptr<sr::client_t> _1, std::weak_ptr<sr::client_t> _2);
  void make_moves_from_str(const std::string &str);
  bool is_contain_player(const std::weak_ptr<sr::client_t>& _1) const;
  const std::weak_ptr<const sr::client_t> get_first_player() const;
  const std::weak_ptr<const sr::client_t> get_second_player() const;

  void load_moves_from_file(const std::string &path);
  void write_moves_to_file(const std::string &path) const;

public:
   desk_t(const desk_t&) = delete;
   desk_t& operator=(const desk_t&) = delete;

private:
  const std::weak_ptr<sr::client_t> first_player;
  const std::weak_ptr<sr::client_t> second_player;
};

} // namespace logic


#endif // __MY_DESK_H__BVGTYVFYTKUBLIUBHIYFRTDVGFABDJUIWOMJ

