// Copyright (c) 2016-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2016-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2023 Simons Foundation
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

/**
 * @file
 * @brief Provides the lazy expression templates that fuse arithmetic operations on block Green's functions.
 */

#pragma once

#include "./block_gf_view.hpp"

#include "../../utility/concept_tools.hpp"
#include "../../utility/expression_template_tools.hpp"

#include <algorithm>
#include <ostream>
#include <type_traits>
#include <utility>

namespace triqs::gfs {

  /**
   * @addtogroup triqs-gfs-algebra
   * @{
   */

  using utility::is_in_ZRC;
  using utility::remove_rvalue_ref_t;

  // Implementation helpers for block Green's function expression templates.
  namespace details_bgfs_expr {

    // Wrapper that lets a scalar participate in a block expression: it exposes a minimal block-Green's-function-like
    // interface (operator[]/operator()) that always returns the wrapped scalar.
    template <typename S> struct scalar_wrap {
      using mesh_t   = void;
      using target_t = void;

      // The wrapped scalar value.
      S s;

      int size() const { return -1; }
      int size1() const { return -1; }
      int size2() const { return -1; }

      // NOLINTNEXTLINE(bugprone-forwarding-reference-overload): scalar_wrap only ever wraps scalars, not block-gf operands
      template <typename T> scalar_wrap(T &&x) : s(std::forward<T>(x)) {}

      template <typename... Keys> S operator[](Keys &&...) const { return s; }

      template <typename... Args> inline S operator()(Args &&...) const { return s; }

      friend std::ostream &operator<<(std::ostream &sout, scalar_wrap const &expr) { return sout << expr.s; }
    };

    // Node type of an expression operand: a scalar_wrap for scalars, else the operand itself.
    template <typename T> using node_t = std::conditional_t<utility::is_in_ZRC<T>::value, scalar_wrap<T>, remove_rvalue_ref_t<T>>;

    // Trait combining two (mesh or target) types into one, treating void as "unset" (mismatch yields void).
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

  /**
   * @brief Lazy expression node representing a binary operation between two block Green's function operands.
   *
   * @details Models the `BlockGreenFunction` concept, so expressions can be nested and assigned to a
   * triqs::gfs::block_gf. The mesh and target are deduced from the operands and the operation is evaluated lazily,
   * block by block, on access.
   *
   * @tparam Tag Operation tag (`+`, `-`, `*`, `/`).
   * @tparam L Type of the left operand (a block Green's function or a wrapped scalar).
   * @tparam R Type of the right operand (a block Green's function or a wrapped scalar).
   */
  template <typename Tag, typename L, typename R> struct bgf_expr : TRIQS_CONCEPT_TAG_NAME(BlockGreenFunction) {

    /// Decayed type of the left operand.
    using L_t = std::decay_t<L>;

    /// Decayed type of the right operand.
    using R_t = std::decay_t<R>;

    /// Mesh type of the expression, deduced from the two operands.
    using mesh_t = typename details_bgfs_expr::same_or_void<typename L_t::mesh_t, typename R_t::mesh_t>::type;

    /// Target type of the expression, deduced from the two operands.
    using target_t = typename details_bgfs_expr::same_or_void<typename L_t::target_t, typename R_t::target_t>::type;

    static_assert(!std::is_same<mesh_t, void>::value, "Cannot combine two gf expressions with different variables");
    static_assert(!std::is_same<target_t, void>::value, "Cannot combine two gf expressions with different target");
    static_assert(((arity_of<L_t> == -1) xor (arity_of<R_t> == -1)) or (arity_of<R_t> == arity_of<L_t> and arity_of<R_t> > 0));

    /// Block arity of the expression (1 for triqs::gfs::block_gf, 2 for triqs::gfs::block2_gf).
    static constexpr int arity = std::max(arity_of<L_t>, arity_of<R_t>);

    /// Left operand.
    L l;

    /// Right operand.
    R r;

    /**
     * @brief Construct from the two operands.
     *
     * @tparam LL Forwarding type of the left operand.
     * @tparam RR Forwarding type of the right operand.
     * @param l_ Left operand.
     * @param r_ Right operand.
     */
    template <typename LL, typename RR> bgf_expr(LL &&l_, RR &&r_) : l(std::forward<LL>(l_)), r(std::forward<RR>(r_)) {}

    /**
     * @brief Get the number of blocks of the expression.
     * @return The larger block count of the two operands.
     */
    auto size() const { return std::max(l.size(), r.size()); }

    /**
     * @brief Get the number of blocks along the first dimension (for a block2 expression).
     * @return The larger first-dimension block count of the two operands.
     */
    auto size1() const { return std::max(l.size1(), r.size1()); }

    /**
     * @brief Get the number of blocks along the second dimension (for a block2 expression).
     * @return The larger second-dimension block count of the two operands.
     */
    auto size2() const { return std::max(l.size2(), r.size2()); }

    /**
     * @brief Get the block names of the expression.
     * @return The block names taken from the block Green's function operand.
     */
    auto block_names() const {
      if constexpr (std::is_void_v<typename L_t::mesh_t>)
        return r.block_names();
      else
        return l.block_names();
    }

    /**
     * @brief Evaluate the expression via subscript at the given keys.
     *
     * @tparam Keys Types of the access keys.
     * @param keys Block indices.
     * @return The result of applying the operation to the operands' subscripted values.
     */
    template <typename... Keys> decltype(auto) operator[](Keys &&...keys) const {
      return utility::operation<Tag>()(l.operator[](std::forward<Keys>(keys)...), r.operator[](std::forward<Keys>(keys)...)); // Clang fix
    }

    /**
     * @brief Evaluate the expression via call at the given arguments.
     *
     * @tparam Args Types of the evaluation arguments.
     * @return The result of applying the operation to the operands' evaluated values.
     */
    template <typename... Args> decltype(auto) operator()(Args &&...args) const {
      return utility::operation<Tag>()(l(std::forward<Args>(args)...), r(std::forward<Args>(args)...));
    }

    /// Stream output of the expression.
    friend std::ostream &operator<<(std::ostream &sout, bgf_expr const &expr) {
      return sout << "(" << expr.l << " " << utility::operation<Tag>::name << " " << expr.r << ")";
    }
  };

  // -------------------------------------------------------------------
  /**
   * @brief Lazy expression node representing the unary minus of a block Green's function operand.
   * @tparam L Type of the operand (a block Green's function or a wrapped scalar).
   */
  template <typename L> struct bgf_unary_m_expr : TRIQS_CONCEPT_TAG_NAME(BlockGreenFunction) {
    /// Decayed type of the operand.
    using L_t = std::decay_t<L>;

    /// Mesh type of the expression.
    using mesh_t = typename L_t::mesh_t;

    /// Target type of the expression.
    using target_t = typename L_t::target_t;

    /// The operand.
    L l;

    /**
     * @brief Construct from the operand.
     *
     * @tparam LL Forwarding type of the operand.
     * @param l_ Operand.
     */
    // NOLINTNEXTLINE(bugprone-forwarding-reference-overload): only constructs from an operand, not a copy/move source
    template <typename LL> bgf_unary_m_expr(LL &&l_) : l(std::forward<LL>(l_)) {}

    /**
     * @brief Get the number of blocks of the expression.
     * @return The number of blocks of the operand.
     */
    auto size() const { return l.size(); }

    /**
     * @brief Get the block names of the expression.
     * @return The block names of the operand.
     */
    auto block_names() const { return l.block_names(); }

    /// Evaluate the negated expression via subscript.
    template <typename... Keys> auto operator[](Keys &&...keys) const { return -l.operator[](std::forward<Keys>(keys)...); } // Clang fix

    /// Evaluate the negated expression via call.
    template <typename... Args> auto operator()(Args &&...args) const { return -l(std::forward<Args>(args)...); }

    /// Stream output of the expression.
    friend std::ostream &operator<<(std::ostream &sout, bgf_unary_m_expr const &expr) { return sout << '-' << expr.l; }
  };

// -------------------------------------------------------------------
// Now we can define all the C++ operators ...
// NOLINTBEGIN(bugprone-macro-parentheses): OP is an operator token and cannot be parenthesized
// Define a binary arithmetic operator returning a lazy triqs::gfs::bgf_expr node.
#define DEFINE_OPERATOR(TAG, OP, TRAIT1, TRAIT2)                                                                                                     \
  template <typename A1, typename A2>                                                                                                                \
  std::enable_if_t<TRAIT1<A1>::value && TRAIT2<A2>::value,                                                                                           \
                   bgf_expr<utility::tags::TAG, details_bgfs_expr::node_t<A1>, details_bgfs_expr::node_t<A2>>>                                       \
  operator OP(A1 &&a1, A2 &&a2) {                                                                                                                    \
    return {std::forward<A1>(a1), std::forward<A2>(a2)};                                                                                             \
  }
  // NOLINTEND(bugprone-macro-parentheses)

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
  /// Unary minus operator returning a lazy triqs::gfs::bgf_unary_m_expr node.
  template <typename A1>
    requires(BlockGreenFunction<A1>::value)
  auto operator-(A1 &&a1) {
    return bgf_unary_m_expr<details_bgfs_expr::node_t<A1>>{std::forward<A1>(a1)};
  }

  // Now the inplace operator. Because of expression template, there are useless for speed
  // we implement them trivially.

// Define in-place compound-assignment operators for block Green's functions in terms of the binary operators.
#define DEFINE_OPERATOR(OP1, OP2)                                                                                                                    \
  template <typename Mesh, typename Target, typename Layout, int Arity, typename T>                                                                  \
  void operator OP1(block_gf_view<Mesh, Target, Layout, Arity> g, T const &x) {                                                                      \
    g = g OP2 x;                                                                                                                                     \
  }                                                                                                                                                  \
  template <typename Mesh, typename Target, typename Layout, int Arity, typename T>                                                                  \
  void operator OP1(block_gf<Mesh, Target, Layout, Arity> &g, T const &x) {                                                                          \
    g = g OP2 x;                                                                                                                                     \
  }

  DEFINE_OPERATOR(+=, +);
  DEFINE_OPERATOR(-=, -);
  DEFINE_OPERATOR(*=, *);
  DEFINE_OPERATOR(/=, /);

#undef DEFINE_OPERATOR

  /** @} */

} // namespace triqs::gfs
