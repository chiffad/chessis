#ifndef __MY_TYPELIST_H__UILGBAWLIDBAWYGDTAGFDWTYADNBUIGYUYUIHJFGH
#define __MY_TYPELIST_H__UILGBAWLIDBAWYGDTAGFDWTYADNBUIGYUYUIHJFGH

#include <type_traits>

template <typename... Ts>
struct typelist_t
{};

namespace my_tl
{

  template <size_t ind, typename T, class List>
  struct index_of_impl;

  template <size_t ind, typename T>  // The type T is not in the list
  struct index_of_impl <ind, T, typelist_t<>>
  {
    enum { value = -1 };
  };

  template <size_t ind, typename T, typename... Ts>  // The type is found
  struct index_of_impl <ind, T, typelist_t<T, Ts...>>
  {
    enum { value = ind };
  };

  template <size_t ind, typename T, typename H, typename... Ts>  // Recursion
  struct index_of_impl <ind, T, typelist_t<H, Ts...>>
  {
    enum { value = index_of_impl<ind + 1, T, typelist_t<Ts...>>::value };
  };

  // Wrapping to supply initial index 0
  template <typename T, class List>
  struct index_of;

  // Specializing for ind >= 0
  template <typename T, typename... Ts>
  struct index_of<T, typelist_t<Ts...>>
  {
    enum { value = index_of_impl<0, T, typelist_t<Ts...>>::value};
  };

} //namespace my_tl

#endif //__MY_TYPELIST_H__UILGBAWLIDBAWYGDTAGFDWTYADNBUIGYUYUIHJFGH
