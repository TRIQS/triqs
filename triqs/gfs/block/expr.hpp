/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012 by M. Ferrero, O. Parcollet
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
#include <triqs/utility/expression_template_tools.hpp>
namespace triqs {
namespace gfs {

 using utility::is_in_ZRC;
 using utility::remove_rvalue_ref_t;

 namespace details_bgfs_expr {

  // a wrapper for scalars
  template <typename S> struct scalar_wrap {
   using variable_t = void;
   using target_t = void;
   S s;
   int size() const { return -1; }
   template <typename T> scalar_wrap(T &&x) : s(std::forward<T>(x)) {}
   template <typename KeyType> S operator[](KeyType &&key) const { return s; }
   template <typename... Args> inline S operator()(Args &&... args) const { return s; }
   friend std::ostream &operator<<(std::ostream &sout, scalar_wrap const &expr) { return sout << expr.s; }
  };


  template <typename T> using node_t = std14::conditional_t<utility::is_in_ZRC<T>::value, scalar_wrap<T>, remove_rvalue_ref_t<T>>;

  template <typename A, typename B> struct same_or_void { using type = void; };
  template <typename A> struct same_or_void<A, A> { using type = A; };
  template <typename A> struct same_or_void<void, A> { using type = A; };
  template <typename A> struct same_or_void<A, void> { using type = A; };
  template <> struct same_or_void<void, void> { using type = void; };


 } // details

 template <typename Tag, typename L, typename R> struct bgf_expr : TRIQS_CONCEPT_TAG_NAME(BlockGreenFunction) {

  // just checking consistency
  using L_t = std14::decay_t<L>;
  using R_t = std14::decay_t<R>;
  using variable_t = typename details_bgfs_expr::same_or_void<typename L_t::variable_t, typename R_t::variable_t>::type;
  using target_t = typename details_bgfs_expr::same_or_void<typename L_t::target_t, typename R_t::target_t>::type;
  static_assert(!std::is_same<variable_t, void>::value, "Cannot combine two gf expressions with different variables");
  static_assert(!std::is_same<target_t, void>::value, "Cannot combine two gf expressions with different target");

  L l;
  R r;
  template <typename LL, typename RR> bgf_expr(LL &&l_, RR &&r_) : l(std::forward<LL>(l_)), r(std::forward<RR>(r_)) {}

  auto size() const { return std::max(l.size(), r.size()); }
  
  auto block_names() const { return l.block_names(); }

  template <typename K> decltype(auto) operator[](K &&key) const {
   return utility::operation<Tag>()(l[std::forward<K>(key)], r[std::forward<K>(key)]);
  }
  template <typename... Args> decltype(auto) operator()(Args &&... args) const {
   return utility::operation<Tag>()(l(std::forward<Args>(args)...), r(std::forward<Args>(args)...));
  }
  friend std::ostream &operator<<(std::ostream &sout, bgf_expr const &expr) {
   return sout << "(" << expr.l << " " << utility::operation<Tag>::name << " " << expr.r << ")";
  }
 };

 // -------------------------------------------------------------------
 // a special case : the unary operator !
 template <typename L> struct bgf_unary_m_expr : TRIQS_CONCEPT_TAG_NAME(BlockGreenFunction) {
  using L_t = std14::decay_t<L>;
  using variable_t = typename L_t::variable_t;
  using target_t = typename L_t::target_t;

  L l;
  template <typename LL> bgf_unary_m_expr(LL &&l_) : l(std::forward<LL>(l_)) {}

  auto size() const { return l.size(); }
  auto block_names() const { return l.block_names(); }

  template <typename KeyType> auto operator[](KeyType &&key) const { return -l[key]; }
  template <typename... Args> auto operator()(Args &&... args) const { return -l(std::forward<Args>(args)...); }
  friend std::ostream &operator<<(std::ostream &sout, bgf_unary_m_expr const &expr) { return sout << '-' << expr.l; }
 };

// -------------------------------------------------------------------

// Now we can define all the C++ operators ...
#define DEFINE_OPERATOR(TAG, OP, TRAIT1, TRAIT2)                                                                                 \
 template <typename A1, typename A2>                                                                                             \
 std14::enable_if_t<TRAIT1<A1>::value && TRAIT2<A2>::value,                                                                      \
                    bgf_expr<utility::tags::TAG, details_bgfs_expr::node_t<A1>, details_bgfs_expr::node_t<A2>>>                  \
 operator OP(A1 &&a1, A2 &&a2) {                                                                                                 \
  return {std::forward<A1>(a1), std::forward<A2>(a2)};                                                                           \
 }

 DEFINE_OPERATOR(plus, +, BlockGreenFunction, BlockGreenFunction);
 DEFINE_OPERATOR(minus, -, BlockGreenFunction, BlockGreenFunction);
 DEFINE_OPERATOR(multiplies, *, BlockGreenFunction, BlockGreenFunction);
 DEFINE_OPERATOR(multiplies, *, is_in_ZRC, BlockGreenFunction);
 DEFINE_OPERATOR(multiplies, *, BlockGreenFunction, is_in_ZRC);
 DEFINE_OPERATOR(divides, /, BlockGreenFunction, BlockGreenFunction);
 DEFINE_OPERATOR(divides, /, is_in_ZRC, BlockGreenFunction);
 DEFINE_OPERATOR(divides, /, BlockGreenFunction, is_in_ZRC);
#undef DEFINE_OPERATOR

 // the unary is special
 template <typename A1>
 std14::enable_if_t<BlockGreenFunction<A1>::value, bgf_unary_m_expr<details_bgfs_expr::node_t<A1>>> operator-(A1 &&a1) {
  return {std::forward<A1>(a1)};
 }

// Now the inplace operator. Because of expression template, there are useless for speed
// we implement them trivially.

#define DEFINE_OPERATOR(OP1, OP2)                                                                                                \
 template <typename Var, typename Target, typename T> void operator OP1(block_gf_view<Var, Target> g, T const &x) {              \
  g = g OP2 x;                                                                                                                   \
 }                                                                                                                               \
 template <typename Var, typename Target, typename T> void operator OP1(block_gf<Var, Target> &g, T const &x) { g = g OP2 x; }

 DEFINE_OPERATOR(+=, +);
 DEFINE_OPERATOR(-=, -);
 DEFINE_OPERATOR(*=, *);
 DEFINE_OPERATOR(/=, / );

#undef DEFINE_OPERATOR
}
} // namespace triqs::gf

