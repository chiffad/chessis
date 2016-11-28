#ifndef __MY_DESK_H__BVGTYVFYTKUBLIUBHIYFRTDVGFABDJUIWOMJ
#define __MY_DESK_H__BVGTYVFYTKUBLIUBHIYFRTDVGFABDJUIWOMJ


#include "chess.h"
#include <string>


namespace logic
{

class Desk : public board_t
{
public:
  Desk(const unsigned first_p, const unsigned second_p);
  bool contain_player(const int ind) const;
  void make_moves_from_str(const std::string &str);
  void load_moves_from_file(const std::string &path);
  void write_moves_to_file(const std::string &path) const;

public:
   Desk(const Desk&) = delete;
   Desk& operator=(const Desk&) = delete;

private:
   const int _first_player_ind;
   const int _second_player_ind;
};

} // namespace logic


#endif // __MY_DESK_H__BVGTYVFYTKUBLIUBHIYFRTDVGFABDJUIWOMJ

