/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011-2014 by O. Parcollet
 *
 * TRIQS is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * TRIQS. If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/
#pragma once
#include "./map.hpp"
#include "./mem_layout.hpp"
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/repeat_from_to.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>

namespace triqs {
namespace arrays {

#define FORCEINLINE __inline__ __attribute__((always_inline))

 // ------------------------- foreach -----------------------------------------------------
 namespace indexmaps {
  namespace cuboid {

   // using foreach_int_type=size_t ;
   using foreach_int_type = std::ptrdiff_t;
// better to be signed here : 1) on some machine/compiler, it is a lot faster !
// When used with clef auto assign, e.g. A(i_,j_) = i -2*j, one needs signed arithmetics

#define AUX_FOR(X) for (ind[X] = 0; ind[X] < l[X]; ++ind[X])
#define AUX_C(z, P, unused) for (foreach_int_type i##P = 0; i##P < l[P]; ++i##P)
#define AUX_F(z, P, RANK) AUX_FOR(RANK - P)
#define AUX_Dynamical(z, P, unused) AUX_FOR(ml[P])
#define AUX_Custom1(z, P, NNN) constexpr int p##P = permutations::apply(traversal_order_perm, P);
#define AUX_Custom2(z, P, unused) AUX_FOR(p##P)
#define AUX3(z, p, unused) BOOST_PP_COMMA_IF(p) ind[p]
#define AUX3C(z, p, unused) BOOST_PP_COMMA_IF(p) i##p
#define IMPL(z, RR, unused)                                                                                                      \
 template <typename FntType>                                                                                                     \
 FORCEINLINE void foreach_impl(_traversal_c, domain_t<RR> const& dom, memory_layout_t<RR> const& ml, FntType F) {                  \
  const mini_vector<foreach_int_type, RR> l(dom.lengths());                                                                      \
  BOOST_PP_REPEAT(RR, AUX_C, nil) { F(BOOST_PP_REPEAT(RR, AUX3C, nil)); }                                                        \
 }                                                                                                                               \
 template <typename FntType>                                                                                                     \
 FORCEINLINE void foreach_impl(_traversal_fortran, domain_t<RR> const& dom, memory_layout_t<RR> ml, FntType F) {                   \
  foreach_int_type ind[RR];                                                                                                      \
  const mini_vector<foreach_int_type, RR> l(dom.lengths());                                                                      \
  BOOST_PP_REPEAT(RR, AUX_F, BOOST_PP_DEC(RR)) { F(BOOST_PP_REPEAT(RR, AUX3, nil)); }                                            \
 }                                                                                                                               \
 template <typename FntType>                                                                                                     \
 FORCEINLINE void foreach_impl(_traversal_dynamical, domain_t<RR> const& dom, memory_layout_t<RR> ml, FntType F) {                 \
  foreach_int_type ind[RR];                                                                                                      \
  const mini_vector<foreach_int_type, RR> l(dom.lengths());                                                                      \
  BOOST_PP_REPEAT(RR, AUX_Dynamical, nil) { F(BOOST_PP_REPEAT(RR, AUX3, nil)); }                                                 \
 }                                                                                                                               \
 template <typename FntType, int... Is>                                                                                          \
 FORCEINLINE void foreach_impl(_traversal_custom<Is...>, domain_t<RR> const& dom, memory_layout_t<RR> ml, FntType F) {             \
  constexpr ull_t traversal_order_perm = permutations::permutation(Is...);                                                       \
  BOOST_PP_REPEAT(RR, AUX_Custom1, BOOST_PP_DEC(RR));                                                                            \
  foreach_int_type ind[RR];                                                                                                      \
  const mini_vector<foreach_int_type, RR> l(dom.lengths());                                                                      \
  BOOST_PP_REPEAT(RR, AUX_Custom2, nil) { F(BOOST_PP_REPEAT(RR, AUX3, nil)); }                                                   \
 }
   BOOST_PP_REPEAT_FROM_TO(1, ARRAY_NRANK_MAX, IMPL, nil);
#undef IMPL
#undef AUX_C
#undef AUX_F
#undef AUX_FOR
#undef AUX_Dynamical
#undef AUX_Custom
#undef AUX_Custom1
#undef AUX3
#undef AUX3C
  }
 }

 /// Get the traversal order
 template <typename A, typename Enable = void> struct _get_traversal_order {
  using traversal_order_t = _traversal_c;
  static memory_layout_t<A::domain_type::rank> invoke(A const& a) {
   return memory_layout_t<A::domain_type::rank>{};
  }
 };
 template <typename A> struct _get_traversal_order<A, typename A::indexmap_type::has_traversal_order_tag> {
  using traversal_order_t = typename A::traversal_order_t;
  static memory_layout_t<A::domain_type::rank> invoke(A const& a) { return a.indexmap().memory_layout(); }
 };

 /// --------------- FOREACH on the domain------------------------
 //// Internal : only in non default constructible init of array
 template <typename T, typename Function>
 FORCEINLINE void _foreach_on_indexmap(T const& x, Function const& F) {
  indexmaps::cuboid::foreach_impl(_traversal_c{}, x.domain(), x.memory_layout(), F);
 }

 /// --------------- FOREACH ------------------------
 template <typename T, typename Function>
 FORCEINLINE std14::enable_if_t<ImmutableCuboidArray<T>::value> foreach(T const& x, Function const& F) {
  using S = _get_traversal_order<T>;
#ifndef TRIQS_ARRAYS_FOREACH_C_OR_DYNAMICAL
  indexmaps::cuboid::foreach_impl(typename S::traversal_order_t{}, x.domain(), S::invoke(x), F);
#else
  if (ml.is_c()) {
   indexmaps::cuboid::foreach_impl(_traversal_c{}, x.domain(), S::invoke(x), F);
  } else {
   indexmaps::cuboid::foreach_impl(_traversal_dynamical{}, x.domain(), S::invoke(x), F);
  }
#endif
 }

/// --------------- ASSIGN FOREACH ------------------------
 template <typename T, typename Function>
 std14::enable_if_t<MutableCuboidArray<T>::value> assign_foreach(T& x, Function const& f) {
  using S = _get_traversal_order<T>;
  indexmaps::cuboid::foreach_impl(typename S::traversal_order_t{}, x.domain(), S::invoke(x),
                                  [&x, &f](auto const&... args) { x(args...) = f(args...); });
 }
}
} // namespace
