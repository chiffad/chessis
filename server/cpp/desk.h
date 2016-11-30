#ifndef __MY_DESK_H__BVGTYVFYTKUBLIUBHIYFRTDVGFABDJUIWOMJ
#define __MY_DESK_H__BVGTYVFYTKUBLIUBHIYFRTDVGFABDJUIWOMJ


#include "chess.h"
#include <string>


namespace logic
{

class desk_t : public board_t
{
public:
  desk_t();
  void make_moves_from_str(const std::string &str);
  void load_moves_from_file(const std::string &path);
  void write_moves_to_file(const std::string &path) const;

public:
   desk_t(const desk_t&) = delete;
   desk_t& operator=(const desk_t&) = delete;
};

} // namespace logic


#endif // __MY_DESK_H__BVGTYVFYTKUBLIUBHIYFRTDVGFABDJUIWOMJ

