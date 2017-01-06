#ifndef __MY_TYPELIST_H__UILGBAWLIDBAWYGDTAGFDWTYADNBUIGYUYUIHJFGH
#define __MY_TYPELIST_H__UILGBAWLIDBAWYGDTAGFDWTYADNBUIGYUYUIHJFGH

#include <type_traits>

template <typename... Ts>
struct typelist_t
{};

namespace my_tl
{

  template <size_t idx, typename T, class List>
  struct index_of_impl;

  template <size_t idx, typename T> /// The type T is not in the list
  struct index_of_impl <idx, T, typelist_t<>>
  {
    using type = std::integral_constant<int, -1>;
  };

  template <size_t idx, typename T, typename... Ts>    ///> The type is found
  struct index_of_impl <idx, T, typelist_t<T, Ts...>>
  {
    using type = std::integral_constant<int, idx>;
  };

  template <size_t idx, typename T, typename H, typename... Ts>  ///> Recursion
  struct index_of_impl <idx, T, typelist_t<H, Ts...>>
  {
    using type = typename index_of_impl<idx + 1, T, typelist_t<Ts...>>::type;
  };

  /// Wrapping to supply initial index 0
  template <typename T, class List>
  struct index_of;

  /// Specializing for idx >= 0
  template <typename T, typename... Ts>
  struct index_of<T, typelist_t<Ts...>>
  {
    using type = typename index_of_impl<0, T, typelist_t<Ts...>>::type;
    using value_type = typename type::value_type;
    static constexpr value_type value = type::value;
  };

} //namespace my_tl

#endif //__MY_TYPELIST_H__UILGBAWLIDBAWYGDTAGFDWTYADNBUIGYUYUIHJFGH
