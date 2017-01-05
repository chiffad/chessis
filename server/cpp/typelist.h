#ifndef __MY_TYPELIST_H__UILGBAWLIDBAWYGDTAGFDWTYADNBUIGYUYUIHJFGH
#define __MY_TYPELIST_H__UILGBAWLIDBAWYGDTAGFDWTYADNBUIGYUYUIHJFGH

 #define TYPELIST_1(T1) typelist_t<T1, my_tl::null_type_t>
 #define TYPELIST_2(T1, T2) typelist_t<T1, TYPELIST_1(T2)>
 #define TYPELIST_3(T1, T2, T3) typelist_t<T1, TYPELIST_2(T2, T3)>
 #define TYPELIST_4(T1, T2, T3, T4) typelist_t<T1, TYPELIST_3(T2, T3, T4)>
 #define TYPELIST_5(T1, T2, T3, T4, T5) typelist_t<T1, TYPELIST_4(T2, T3, T4, T5)>
 #define TYPELIST_6(T1, T2, T3, T4, T5, T6) typelist_t<T1, TYPELIST_5(T2, T3, T4, T5, T6)>
 #define TYPELIST_7(T1, T2, T3, T4, T5, T6, T7) typelist_t<T1, TYPELIST_6(T2, T3, T4, T5, T6, T7)>
 #define TYPELIST_8(T1, T2, T3, T4, T5, T6, T7, T8) typelist_t<T1, TYPELIST_7(T2, T3, T4, T5, T6, T7, T8)>
 #define TYPELIST_9(T1, T2, T3, T4, T5, T6, T7, T8, T9) typelist_t<T1, TYPELIST_8(T2, T3, T4, T5, T6, T7, T8, T9)>
 #define TYPELIST_10(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10) typelist_t<T1, TYPELIST_9(T2, T3, T4, T5, T6, T7, T8, T9, T10)>
 #define TYPELIST_11(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11) typelist_t<T1, TYPELIST_10(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11)>
 #define TYPELIST_12(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12) \
       typelist_t<T1, TYPELIST_11(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12)>
 #define TYPELIST_13(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13) \
       typelist_t<T1, TYPELIST_12(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13)>
 #define TYPELIST_14(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14) \
       typelist_t<T1, TYPELIST_13(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14)>
 #define TYPELIST_15(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15) \
       typelist_t<T1, TYPELIST_14(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15)>
 #define TYPELIST_16(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16) \
       typelist_t<T1, TYPELIST_15(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16)>
 #define TYPELIST_17(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17) \
       typelist_t<T1, TYPELIST_16(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17)>
 #define TYPELIST_18(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18) \
       typelist_t<T1, TYPELIST_17(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18)>
 #define TYPELIST_19(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19) \
       typelist_t<T1, TYPELIST_18(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19)>
 #define TYPELIST_20(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20) \
       typelist_t<T1, TYPELIST_19(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20)>

template<typename T, typename T1>
struct typelist_t
{
  typedef T head;
  typedef T1 Tail;
};

namespace my_tl
{
  template<typename T_list, typename T> struct index_of;
  
  struct null_type_t
  { };
  
  template<typename T>
  struct index_of<null_type_t, T>
    { enum { value = -1 }; };

  template<typename T, typename Tail>
  struct index_of<typelist_t<T, Tail>, T>
    { enum { value = 0 }; };
  
  template<typename Head, typename Tail, typename T>
  struct index_of<typelist_t<Head, Tail>, T>
  {
  private:
    enum { temp = index_of<Tail, T>::value };
  public:
    enum { value = temp == -1 ? -1 : 1 + temp };
  };
} //namespace my_tl

#endif //__MY_TYPELIST_H__UILGBAWLIDBAWYGDTAGFDWTYADNBUIGYUYUIHJFGH