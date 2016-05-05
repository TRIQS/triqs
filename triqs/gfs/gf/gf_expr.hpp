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
 using utility::remove_rvalue_ref;

 namespace gfs_expr_tools {

  // a wrapper for scalars
  template <typename S> struct scalar_wrap {
   using variable_t = void;
   using target_t = void;
   S s;
   template <typename T> scalar_wrap(T &&x) : s(std::forward<T>(x)) {}
   S singularity() const { return s; }
   template <typename KeyType> S operator[](KeyType &&key) const { return s; }
   template <typename... Args> inline S operator()(Args &&... args) const { return s; }
   friend std::ostream &operator<<(std::ostream &sout, scalar_wrap const &expr) { return sout << expr.s; }
  };

  // Combine the two meshes of LHS and RHS : need to specialize where there is a scalar
  struct combine_mesh {
   template <typename L, typename R> auto operator()(L &&l, R &&r) const -> decltype(std::forward<L>(l).mesh()) {
    if (!(l.mesh() == r.mesh()))
     TRIQS_RUNTIME_ERROR << "Mesh mismatch: in Green Function Expression" << l.mesh() << " vs " << r.mesh();
    return std::forward<L>(l).mesh();
   }
   template <typename S, typename R> decltype(auto) operator()(scalar_wrap<S> const &, R &&r) const {
    return std::forward<R>(r).mesh();
   }
   template <typename S, typename L> decltype(auto) operator()(L &&l, scalar_wrap<S> const &) const {
    return std::forward<L>(l).mesh();
   }
  };

  // Same thing to get the data shape
  // NB : could be unified to one combine<F>, where F is a functor, but an easy usage requires polymorphic lambda ...
  struct combine_shape {
   template <typename L, typename R> auto operator()(L &&l, R &&r) const -> decltype(l.data_shape()) {
    if (!(l.data_shape() == r.data_shape()))
     TRIQS_RUNTIME_ERROR << "Shape mismatch in Green Function Expression: " << l.data_shape() << " vs " << r.data_shape();
    return l.data_shape();
   }
   template <typename S, typename R> decltype(auto) operator()(scalar_wrap<S> const &, R &&r) const { return r.data_shape(); }
   template <typename S, typename L> decltype(auto) operator()(L &&l, scalar_wrap<S> const &) const { return l.data_shape(); }
  };

  template <typename T>
  using node_t = std14::conditional_t<utility::is_in_ZRC<T>::value, scalar_wrap<T>, typename remove_rvalue_ref<T>::type>;

  template <typename A, typename B> struct _or_ { using type = void; };
  template <typename A> struct _or_<A, A> { using type = A; };
  template <typename A> struct _or_<void, A> { using type = A; };
  template <typename A> struct _or_<A, void> { using type = A; };
  template <> struct _or_<void, void> { using type = void; };

 } // gfs_expr_tools

 template <typename Tag, typename L, typename R> struct gf_expr : TRIQS_CONCEPT_TAG_NAME(ImmutableGreenFunction) {
  using L_t = typename std::remove_reference<L>::type;
  using R_t = typename std::remove_reference<R>::type;
  using variable_t = typename gfs_expr_tools::_or_<typename L_t::variable_t, typename R_t::variable_t>::type;
  using target_t = typename gfs_expr_tools::_or_<typename L_t::target_t, typename R_t::target_t>::type;
  static_assert(!std::is_same<variable_t, void>::value, "Cannot combine two gf expressions with different variables");
  static_assert(!std::is_same<target_t, void>::value, "Cannot combine two gf expressions with different target");

  L l;
  R r;
  template <typename LL, typename RR> gf_expr(LL &&l_, RR &&r_) : l(std::forward<LL>(l_)), r(std::forward<RR>(r_)) {}

  decltype(auto) mesh() const { return gfs_expr_tools::combine_mesh()(l, r); }
  decltype(auto) singularity() const { return utility::operation<Tag>()(l.singularity(), r.singularity()); }
  auto data_shape() const { return gfs_expr_tools::combine_shape()(l, r); }

  template <typename KeyType> decltype(auto) operator[](KeyType &&key) const {
   return utility::operation<Tag>()(l[std::forward<KeyType>(key)], r[std::forward<KeyType>(key)]);
  }
  template <typename... Args> decltype(auto) operator()(Args &&... args) const {
   return utility::operation<Tag>()(l(std::forward<Args>(args)...), r(std::forward<Args>(args)...));
  }
  friend std::ostream &operator<<(std::ostream &sout, gf_expr const &expr) {
   return sout << "(" << expr.l << " " << utility::operation<Tag>::name << " " << expr.r << ")";
  }
 };

 
 // -------------------------------------------------------------------
 // a special case : the unary operator !
 template <typename L> struct gf_unary_m_expr : TRIQS_CONCEPT_TAG_NAME(ImmutableGreenFunction) {
  using L_t = typename std::remove_reference<L>::type;
  using variable_t = typename L_t::variable_t;
  using target_t = typename L_t::target_t;

  L l;
  template <typename LL> gf_unary_m_expr(LL &&l_) : l(std::forward<LL>(l_)) {}

  decltype(auto) mesh() const { return l.mesh(); }
  decltype(auto) singularity() const { return l.singularity(); }
  auto data_shape() const { return l.data_shape(); }

  template <typename KeyType> auto operator[](KeyType &&key) const { return -l[key]; }
  template <typename... Args> auto operator()(Args &&... args) const { return -l(std::forward<Args>(args)...); }
  friend std::ostream &operator<<(std::ostream &sout, gf_unary_m_expr const &expr) { return sout << '-' << expr.l; }
 };


// -------------------------------------------------------------------
// Now we can define all the C++ operators ...
#define DEFINE_OPERATOR(TAG, OP, TRAIT1, TRAIT2)                                                                                 \
 template <typename A1, typename A2>                                                                                             \
 std14::enable_if_t<TRAIT1<A1>::value && TRAIT2<A2>::value,                                                                      \
                    gf_expr<utility::tags::TAG, gfs_expr_tools::node_t<A1>, gfs_expr_tools::node_t<A2>>>                         \
 operator OP(A1 &&a1, A2 &&a2) {                                                                                                 \
  return {std::forward<A1>(a1), std::forward<A2>(a2)};                                                                           \
 }

 DEFINE_OPERATOR(plus, +, ImmutableGreenFunction, ImmutableGreenFunction);
 DEFINE_OPERATOR(minus, -, ImmutableGreenFunction, ImmutableGreenFunction);
 DEFINE_OPERATOR(multiplies, *, ImmutableGreenFunction, ImmutableGreenFunction);
 DEFINE_OPERATOR(multiplies, *, is_in_ZRC, ImmutableGreenFunction);
 DEFINE_OPERATOR(multiplies, *, ImmutableGreenFunction, is_in_ZRC);
 DEFINE_OPERATOR(divides, /, ImmutableGreenFunction, ImmutableGreenFunction);
 DEFINE_OPERATOR(divides, /, is_in_ZRC, ImmutableGreenFunction);
 DEFINE_OPERATOR(divides, /, ImmutableGreenFunction, is_in_ZRC);
#undef DEFINE_OPERATOR

 // the unary is special
 template <typename A1>
 std14::enable_if_t<ImmutableGreenFunction<A1>::value, gf_unary_m_expr<gfs_expr_tools::node_t<A1>>> operator-(A1 &&a1) {
  return {std::forward<A1>(a1)};
 }

// Now the inplace operator. Because of expression template, there are useless for speed
// we implement them trivially.

#define DEFINE_OPERATOR(OP1, OP2)                                                                                                \
 template <typename Var, typename Target, typename T>                                                       \
 void operator OP1(gf_view<Var, Target> g, T const &x) {                                                             \
  g = g OP2 x;                                                                                                                   \
 }                                                                                                                               \
 template <typename Var, typename Target, typename T>                                                       \
 void operator OP1(gf<Var, Target> &g, T const &x) {                                                                 \
  g = g OP2 x;                                                                                                                   \
 }

 DEFINE_OPERATOR(+=, +);
 DEFINE_OPERATOR(-=, -);
 DEFINE_OPERATOR(*=, *);
 DEFINE_OPERATOR(/=, / );

#undef DEFINE_OPERATOR
}
} // namespace triqs::gf

