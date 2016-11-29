#ifndef __MY_DESK_H__BVGTYVFYTKUBLIUBHIYFRTDVGFABDJUIWOMJ
#define __MY_DESK_H__BVGTYVFYTKUBLIUBHIYFRTDVGFABDJUIWOMJ


#include "chess.h"
#include <string>


namespace logic
{

class desk_t : public board_t
{
public:
  desk_t(const unsigned first_p = 0, const unsigned second_p = 0);
  bool contain_player(const int ind) const;
  void make_moves_from_str(const std::string &str);
  void load_moves_from_file(const std::string &path);
  void write_moves_to_file(const std::string &path) const;

public:
   desk_t(const desk_t&) = delete;
   desk_t& operator=(const desk_t&) = delete;

private:
   const int _first_player_ind;
   const int _second_player_ind;
};

} // namespace logic


#endif // __MY_DESK_H__BVGTYVFYTKUBLIUBHIYFRTDVGFABDJUIWOMJ

