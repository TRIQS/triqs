// Copyright (c) 2016-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2016-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2022 Simons Foundation
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You may obtain a copy of the License at
//     https://www.gnu.org/licenses/gpl-3.0.txt
//
// Authors: Michel Ferrero, Olivier Parcollet, Nils Wentzell

#pragma once
#include <triqs/utility/expression_template_tools.hpp>
namespace triqs {
  namespace gfs {

    using utility::is_in_ZRC;
    using utility::remove_rvalue_ref_t;

    namespace details_bgfs_expr {

      // a wrapper for scalars
      template <typename S> struct scalar_wrap {
        using mesh_t   = void;
        using target_t = void;
        S s;
        int size() const { return -1; }
        int size1() const { return -1; }
        int size2() const { return -1; }
        template <typename T> scalar_wrap(T &&x) : s(std::forward<T>(x)) {}
        template <typename... Keys> S operator[](Keys &&...keys) const { return s; }
        template <typename... Args> inline S operator()(Args &&...args) const { return s; }
        friend std::ostream &operator<<(std::ostream &sout, scalar_wrap const &expr) { return sout << expr.s; }
      };

      template <typename T> using node_t = std::conditional_t<utility::is_in_ZRC<T>::value, scalar_wrap<T>, remove_rvalue_ref_t<T>>;

      template <typename A, typename B> struct same_or_void {
        using type = void;
      };
      template <typename A> struct same_or_void<A, A> {
        using type = A;
      };
      template <typename A> struct same_or_void<void, A> {
        using type = A;
      };
      template <typename A> struct same_or_void<A, void> {
        using type = A;
      };
      template <> struct same_or_void<void, void> {
        using type = void;
      };

    } // namespace details_bgfs_expr

    template <typename Tag, typename L, typename R> struct bgf_expr : TRIQS_CONCEPT_TAG_NAME(BlockGreenFunction) {

      // just checking consistency
      using L_t      = std::decay_t<L>;
      using R_t      = std::decay_t<R>;
      using mesh_t   = typename details_bgfs_expr::same_or_void<typename L_t::mesh_t, typename R_t::mesh_t>::type;
      using target_t = typename details_bgfs_expr::same_or_void<typename L_t::target_t, typename R_t::target_t>::type;
      static_assert(!std::is_same<mesh_t, void>::value, "Cannot combine two gf expressions with different variables");
      static_assert(!std::is_same<target_t, void>::value, "Cannot combine two gf expressions with different target");
      static_assert((arity_of<L_t> == -1 xor arity_of<R_t> == -1) or (arity_of<R_t> == arity_of<L_t> and arity_of<R_t> > 0));

      static constexpr int arity = std::max(arity_of<L_t>, arity_of<R_t>);

      L l;
      R r;
      template <typename LL, typename RR> bgf_expr(LL &&l_, RR &&r_) : l(std::forward<LL>(l_)), r(std::forward<RR>(r_)) {}

      auto size() const { return std::max(l.size(), r.size()); }
      auto size1() const { return std::max(l.size1(), r.size1()); }
      auto size2() const { return std::max(l.size2(), r.size2()); }

      auto block_names() const {
        if constexpr (std::is_void_v<typename L_t::mesh_t>)
          return r.block_names();
        else
          return l.block_names();
      }

      template <typename... Keys> decltype(auto) operator[](Keys &&...keys) const {
        return utility::operation<Tag>()(l.operator[](std::forward<Keys>(keys)...), r.operator[](std::forward<Keys>(keys)...)); // Clang fix
      }
      template <typename... Args> decltype(auto) operator()(Args &&...args) const {
        return utility::operation<Tag>()(l(std::forward<Args>(args)...), r(std::forward<Args>(args)...));
      }
      friend std::ostream &operator<<(std::ostream &sout, bgf_expr const &expr) {
        return sout << "(" << expr.l << " " << utility::operation<Tag>::name << " " << expr.r << ")";
      }
    };

    // -------------------------------------------------------------------
    // a special case : the unary operator !
    template <typename L> struct bgf_unary_m_expr : TRIQS_CONCEPT_TAG_NAME(BlockGreenFunction) {
      using L_t      = std::decay_t<L>;
      using mesh_t   = typename L_t::mesh_t;
      using target_t = typename L_t::target_t;

      L l;
      template <typename LL> bgf_unary_m_expr(LL &&l_) : l(std::forward<LL>(l_)) {}

      auto size() const { return l.size(); }
      auto block_names() const { return l.block_names(); }

      template <typename... Keys> auto operator[](Keys &&...keys) const { return -l.operator[](std::forward<Keys>(keys)...); } // Clang fix
      template <typename... Args> auto operator()(Args &&...args) const { return -l(std::forward<Args>(args)...); }
      friend std::ostream &operator<<(std::ostream &sout, bgf_unary_m_expr const &expr) { return sout << '-' << expr.l; }
    };

// -------------------------------------------------------------------

// Now we can define all the C++ operators ...
#define DEFINE_OPERATOR(TAG, OP, TRAIT1, TRAIT2)                                                                                                     \
  template <typename A1, typename A2>                                                                                                                \
  std::enable_if_t<TRAIT1<A1>::value && TRAIT2<A2>::value,                                                                                           \
                   bgf_expr<utility::tags::TAG, details_bgfs_expr::node_t<A1>, details_bgfs_expr::node_t<A2>>>                                       \
  operator OP(A1 &&a1, A2 &&a2) {                                                                                                                    \
    return {std::forward<A1>(a1), std::forward<A2>(a2)};                                                                                             \
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
    template <typename A1> std::enable_if_t<BlockGreenFunction<A1>::value, bgf_unary_m_expr<details_bgfs_expr::node_t<A1>>> operator-(A1 &&a1) {
      return {std::forward<A1>(a1)};
    }

    // Now the inplace operator. Because of expression template, there are useless for speed
    // we implement them trivially.

#define DEFINE_OPERATOR(OP1, OP2)                                                                                                                    \
  template <typename Var, typename Target, typename T> void operator OP1(block_gf_view<Var, Target> g, T const &x) { g = g OP2 x; }                  \
  template <typename Var, typename Target, typename T> void operator OP1(block_gf<Var, Target> &g, T const &x) { g = g OP2 x; }

    DEFINE_OPERATOR(+=, +);
    DEFINE_OPERATOR(-=, -);
    DEFINE_OPERATOR(*=, *);
    DEFINE_OPERATOR(/=, /);

#undef DEFINE_OPERATOR
  } // namespace gfs
} // namespace triqs
