#pragma once
#include <type_traits>

namespace cpp2py {

  template <template <typename...> class TMPLT, typename T> struct is_instantiation_of : std::false_type {};
  template <template <typename...> class TMPLT, typename... U> struct is_instantiation_of<TMPLT, TMPLT<U...>> : std::true_type {};
  template <template <typename...> class gf, typename T>
  inline constexpr bool is_instantiation_of_v = is_instantiation_of<gf, std::decay_t<T>>::value;

} // namespace cpp2py
