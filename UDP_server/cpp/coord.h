#ifndef __MY_COORD_H__BHJVCFCFGVAGKSBDVLNWWADVGVAUSKH
#define __MY_COORD_H__BHJVCFCFGVAGKSBDVLNWWADVGVAUSKH


namespace logic
{

struct coord_t
{
  coord_t(const unsigned X = 0, const unsigned Y = 0)
      : x(X), y(Y)
  {
  }

  bool operator==(const coord_t& rhs) const
  {
    return(x == rhs.x && y == rhs.y);
  }

  unsigned x;
  unsigned y;
};

} // namespace logic


#endif //__MY_COORD_H__BHJVCFCFGVAGKSBDVLNWWADVGVAUSKH
