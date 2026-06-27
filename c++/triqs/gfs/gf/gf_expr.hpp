// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
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
 * @brief Provides the lazy expression templates that fuse arithmetic operations on Green's functions.
 */

#pragma once

#include "./gf.hpp"

#include "../../mesh/dlr_imtime.hpp"
#include "../../mesh/imtime.hpp"
#include "../../utility/concept_tools.hpp"
#include "../../utility/exceptions.hpp"
#include "../../utility/expression_template_tools.hpp"

#include <cmath>
#include <optional>
#include <ostream>
#include <type_traits>
#include <utility>

namespace triqs::gfs {

  /**
   * @addtogroup triqs-gfs-algebra
   * @{
   */

  using utility::is_in_ZRC;
  using utility::remove_rvalue_ref;

  // Implementation helpers for Green's function expression templates.
  namespace gfs_expr_tools {

    // Placeholder mesh type used for the scalar operands of an expression.
    using no_mesh_t = void *;

    // Wrapper that lets a scalar participate in a Green's function expression: it exposes a minimal
    // Green's-function-like interface (fake mesh, operator[]/operator()) that always returns the wrapped scalar.
    template <typename S> struct scalar_wrap {
      using mesh_t   = void;
      using target_t = void;

      // The wrapped scalar value.
      S s;

      // NOLINTNEXTLINE(bugprone-forwarding-reference-overload): scalar_wrap only ever wraps scalars, not gf operands
      template <typename T> scalar_wrap(T &&x) : s(std::forward<T>(x)) {}

      [[nodiscard]] no_mesh_t mesh() const { return {}; } // Fake for combine_mesh

      template <typename... Keys> S operator[](Keys &&...) const { return s; }

      template <typename... Args> inline S operator()(Args &&...) const { return s; }

      friend std::ostream &operator<<(std::ostream &sout, scalar_wrap const &expr) { return sout << expr.s; }
    };

    // Combine a mesh with a scalar operand: returns the mesh.
    template <typename Tag, typename M> M combine_mesh(M const &m, no_mesh_t) { return m; }

    // Combine a scalar operand with a mesh: returns the mesh.
    template <typename Tag, typename M> M combine_mesh(no_mesh_t, M const &m) { return m; }

    // Combine the meshes of the two operands of a binary expression (they must be equal; throws otherwise).
    template <typename Tag, typename M> M combine_mesh(M const &l, M const &r) {
      if (!(l == r))
        TRIQS_RUNTIME_ERROR << "Mesh mismatch: In Green Function Expression, the mesh of the 2 operands should be equal" << l << " vs " << r;
      return l;
    }

    // Combine the meshes for imaginary-time operands, accounting for the statistics of a product/quotient.
    template <typename Tag, nda::AnyOf<mesh::imtime, mesh::dlr_imtime> M> M combine_mesh(M const &l, M const &r) {

      if constexpr (std::is_same_v<Tag, utility::tags::multiplies> or std::is_same_v<Tag, utility::tags::divides>) {
        bool eq = (std::abs(l.beta() - r.beta()) < 1.e-15) and (l.size() == r.size());
        if (!eq) TRIQS_RUNTIME_ERROR << "Mesh mismatch: In Green Function Expression, the mesh of the 2 operands should be equal" << l << " vs " << r;

        // compute the stat of the product, divide.
        int s               = (int(l.statistic()) + int(r.statistic())) % 2;
        statistic_enum stat = (s == 0 ? Boson : Fermion);
        if constexpr (std::is_same_v<M, mesh::imtime>)
          return M{l.beta(), stat, l.size()};
        else // dlr_imtime
          return M{l.beta(), stat, l.w_max(), l.eps()};
      } else {
        if (!(l == r))
          TRIQS_RUNTIME_ERROR << "Mesh mismatch: In Green Function Expression, the mesh of the 2 operands should be equal" << l << " vs " << r;
        return l;
      }
    }

    // special case of prod of mesh
    namespace details {
      // Component-wise combination of two product meshes (helper for the product-mesh combine_mesh overload).
      template <typename Tag, typename... M, size_t... Is>
      mesh::prod<M...> combine_mesh_impl_cp(std::index_sequence<Is...>, mesh::prod<M...> const &l, mesh::prod<M...> const &r) {
        return {combine_mesh<Tag>(std::get<Is>(l), std::get<Is>(r))...};
      }
    } // namespace details

    // Combine two product meshes component by component.
    template <typename Tag, typename... M> mesh::prod<M...> combine_mesh(mesh::prod<M...> const &l, mesh::prod<M...> const &r) {
      return details::combine_mesh_impl_cp<Tag>(std::index_sequence_for<M...>{}, l, r);
    }

    // Functor combining the data shapes of the two operands of a binary expression (shapes must match; a scalar
    // operand takes the shape of the other operand).
    struct combine_shape {
      template <typename L, typename R> auto operator()(L &&l, R &&r) const {
        auto ls = std::forward<L>(l).data_shape();
        auto rs = std::forward<R>(r).data_shape();
        if (!(ls == rs)) TRIQS_RUNTIME_ERROR << "Shape mismatch in Green Function Expression: " << ls << " vs " << rs;
        return ls;
      }
      template <typename S, typename R> decltype(auto) operator()(scalar_wrap<S> const &, R &&r) const { return std::forward<R>(r).data_shape(); }
      template <typename S, typename L> decltype(auto) operator()(L &&l, scalar_wrap<S> const &) const { return std::forward<L>(l).data_shape(); }
    };

    // Node type of an expression operand: a scalar_wrap for scalars, else the operand itself.
    template <typename T>
    using node_t = std::conditional_t<utility::is_in_ZRC<T>::value, scalar_wrap<std::decay_t<T>>, typename remove_rvalue_ref<T>::type>;

    // Trait combining two (mesh or target) types into one, treating void as "unset" (mismatch yields void).
    template <typename A, typename B> struct _or_ {
      using type = void;
    };
    template <typename A> struct _or_<A, A> {
      using type = A;
    };
    template <typename A> struct _or_<void, A> {
      using type = A;
    };
    template <typename A> struct _or_<A, void> {
      using type = A;
    };
    template <> struct _or_<void, void> {
      using type = void;
    };

  } // namespace gfs_expr_tools

  /**
   * @brief Lazy expression node representing a binary operation between two Green's function operands.
   *
   * @details Models the `GreenFunction` concept, so expressions can be nested and assigned to a triqs::gfs::gf. The 
   * mesh and target are deduced from the operands and the operation is evaluated lazily, element by element, on access.
   *
   * @tparam Tag Operation tag (`+`, `-`, `*`, `/`).
   * @tparam L Type of the left operand (a Green's function or a wrapped scalar).
   * @tparam R Type of the right operand (a Green's function or a wrapped scalar).
   */
  template <typename Tag, typename L, typename R> struct gf_expr : TRIQS_CONCEPT_TAG_NAME(GreenFunction) {
    /// Decayed type of the left operand.
    using L_t = std::remove_reference_t<L>;

    /// Decayed type of the right operand.
    using R_t = std::remove_reference_t<R>;

    /// Mesh type of the expression, deduced from the two operands.
    using mesh_t = typename gfs_expr_tools::_or_<typename L_t::mesh_t, typename R_t::mesh_t>::type;

    /// Target type of the expression, deduced from the two operands.
    using target_t = typename gfs_expr_tools::_or_<typename L_t::target_t, typename R_t::target_t>::type;

    /// Regular (owning) type the expression evaluates to.
    using regular_t = gf<mesh_t, target_t>;

    static_assert(!std::is_same_v<mesh_t, void>, "Cannot combine two gf expressions with different variables");
    static_assert(!std::is_same_v<target_t, void>, "Cannot combine two gf expressions with different target");

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
    template <typename LL, typename RR> gf_expr(LL &&l_, RR &&r_) : l(std::forward<LL>(l_)), r(std::forward<RR>(r_)) {}

    private:
    mutable std::optional<mesh_t> _mesh;

    public:
    /**
     * @brief Get the mesh of the expression (computed lazily and cached).
     * @return A const reference to the combined mesh.
     */
    auto const &mesh() const {
      if (not _mesh) _mesh.emplace(gfs_expr_tools::combine_mesh<Tag>(l.mesh(), r.mesh()));
      return *_mesh;
    }

    /**
     * @brief Get the data shape of the expression.
     * @return The combined data shape of the two operands.
     */
    auto data_shape() const { return gfs_expr_tools::combine_shape()(l, r); }

    /**
     * @brief Evaluate the expression via subscript at the given keys.
     * 
     * @tparam Keys Types of the access keys.
     * @param keys Mesh points/indices.
     * @return The result of applying the operation to the operands' subscripted values.
     */
    template <typename... Keys> decltype(auto) operator[](Keys &&...keys) const {
      return utility::operation<Tag>()(l.operator[](std::forward<Keys>(keys)...), r.operator[](std::forward<Keys>(keys)...)); // Clang Fix
    }

    /**
     * @brief Evaluate the expression via call at the given arguments (one per mesh dimension).
     * 
     * @tparam Args Types of the evaluation arguments.
     * @return The result of applying the operation to the operands' evaluated values.
     */
    template <typename... Args> decltype(auto) operator()(Args &&...args) const {
      return utility::operation<Tag>()(l(std::forward<Args>(args)...), r(std::forward<Args>(args)...));
    }

    /// Stream output of the expression.
    friend std::ostream &operator<<(std::ostream &sout, gf_expr const &expr) {
      return sout << "(" << expr.l << " " << utility::operation<Tag>::name << " " << expr.r << ")";
    }
  };

  // -------------------------------------------------------------------
  /**
   * @brief Lazy expression node representing the unary minus of a Green's function operand.
   * @tparam L Type of the operand (a Green's function or a wrapped scalar).
   */
  template <typename L> struct gf_unary_m_expr : TRIQS_CONCEPT_TAG_NAME(GreenFunction) {
    /// Decayed type of the operand.
    using L_t = std::remove_reference_t<L>;

    /// Mesh type of the expression.
    using mesh_t = typename L_t::mesh_t;

    /// Target type of the expression.
    using target_t = typename L_t::target_t;

    /// Regular (owning) type the expression evaluates to.
    using regular_t = gf<mesh_t, target_t>;

    /// The operand.
    L l;

    /**
     * @brief Construct from the operand.
     * 
     * @tparam LL Forwarding type of the operand.
     * @param l_ Operand.
     */
    // NOLINTNEXTLINE(bugprone-forwarding-reference-overload): only constructs from an operand, not a copy/move source
    template <typename LL> gf_unary_m_expr(LL &&l_) : l(std::forward<LL>(l_)) {}

    /**
     * @brief Get the mesh of the expression.
     * @return The mesh of the operand.
     */
    decltype(auto) mesh() const { return l.mesh(); }

    /**
     * @brief Get the data shape of the expression.
     * @return The data shape of the operand.
     */
    auto data_shape() const { return l.data_shape(); }

    /// Evaluate the negated expression via subscript.
    template <typename... Keys> auto operator[](Keys &&...keys) const { return -l.operator[](std::forward<Keys>(keys)...); } // Clang Fix

    /// Evaluate the negated expression via call.
    template <typename... Args> auto operator()(Args &&...args) const { return -l(std::forward<Args>(args)...); }

    /// Stream output of the expression.
    friend std::ostream &operator<<(std::ostream &sout, gf_unary_m_expr const &expr) { return sout << '-' << expr.l; }
  };

  /**
   * @brief Trait to detect whether a type is a Green's function expression node.
   * @tparam T Type to test.
   */
  template <typename T> struct is_gf_expr : std::false_type {};

  // Specialization of triqs::gfs::is_gf_expr for triqs::gfs::gf_expr.
  template <typename Tag, typename L, typename R> struct is_gf_expr<gf_expr<Tag, L, R>> : std::true_type {};

  // Specialization of triqs::gfs::is_gf_expr for triqs::gfs::gf_unary_m_expr.
  template <typename L> struct is_gf_expr<gf_unary_m_expr<L>> : std::true_type {};

// -------------------------------------------------------------------
// Now we can define all the C++ operators ...
// NOLINTBEGIN(bugprone-macro-parentheses): OP is an operator token and cannot be parenthesized
// Define a binary arithmetic operator returning a lazy triqs::gfs::gf_expr node.
#define DEFINE_OPERATOR(TAG, OP, TRAIT1, TRAIT2)                                                                                                     \
  template <typename A1, typename A2>                                                                                                                \
    requires(TRAIT1<A1>::value and TRAIT2<A2>::value)                                                                                                \
  gf_expr<utility::tags::TAG, gfs_expr_tools::node_t<A1>, gfs_expr_tools::node_t<A2>> operator OP(A1 &&a1, A2 &&a2) {                                \
    return {std::forward<A1>(a1), std::forward<A2>(a2)};                                                                                             \
  }
  // NOLINTEND(bugprone-macro-parentheses)

  DEFINE_OPERATOR(plus, +, GreenFunction, GreenFunction);
  DEFINE_OPERATOR(minus, -, GreenFunction, GreenFunction);
  DEFINE_OPERATOR(multiplies, *, GreenFunction, GreenFunction);
  DEFINE_OPERATOR(multiplies, *, is_in_ZRC, GreenFunction);
  DEFINE_OPERATOR(multiplies, *, GreenFunction, is_in_ZRC);
  DEFINE_OPERATOR(divides, /, GreenFunction, GreenFunction);
  DEFINE_OPERATOR(divides, /, is_in_ZRC, GreenFunction);
  DEFINE_OPERATOR(divides, /, GreenFunction, is_in_ZRC);
#undef DEFINE_OPERATOR

  // the unary is special
  /// Unary minus operator returning a lazy triqs::gfs::gf_unary_m_expr node.
  template <typename A1>
    requires(GreenFunction<A1>::value)
  auto operator-(A1 &&a1) {
    return gf_unary_m_expr<gfs_expr_tools::node_t<A1>>{std::forward<A1>(a1)};
  }

  // Now the inplace operator. Because of expression template, there are useless for speed
  // we implement them trivially.

// Define in-place compound-assignment operators for Green's functions in terms of the binary operators.
#define DEFINE_OPERATOR(OP1, OP2)                                                                                                                    \
  template <typename Mesh, typename Target, typename T> void operator OP1(gf_view<Mesh, Target> g, T const &x) { g = g OP2 x; }                      \
  template <typename Mesh, typename Target, typename T> void operator OP1(gf<Mesh, Target> &g, T const &x) { g = g OP2 x; }

  DEFINE_OPERATOR(+=, +);
  DEFINE_OPERATOR(-=, -);
  DEFINE_OPERATOR(*=, *);
  DEFINE_OPERATOR(/=, /);

#undef DEFINE_OPERATOR

  // In-place matrix inversion

  // Invert, in place, the target matrix (last two dimensions) at each mesh point of a Green's function data array.
  template <typename A> void _gf_invert_data_in_place(A &a) {
    auto mesh_lengths = nda::stdutil::mpop<2>(a.indexmap().lengths());
    nda::for_each(mesh_lengths, [&a](auto &&...i) { nda::linalg::inv_in_place(make_matrix_view(a(i..., range::all, range::all))); });
  }

  // Python specific operator and definitions

  // definitions of operators for scalar / matrix with all triqs Gf mesh types
  // loop over mesh and apply operatations

  /// In-place addition of a matrix to every mesh point of a matrix-valued Green's function.
  template <Mesh M, nda::MemoryMatrix Mat> inline void operator+=(gf_view<M> g, Mat const &mat) {
    for (auto mp : g.mesh()) g[mp] += mat;
  }

  /// In-place subtraction of a matrix from every mesh point of a matrix-valued Green's function.
  template <Mesh M, nda::MemoryMatrix Mat> inline void operator-=(gf_view<M> g, Mat const &mat) {
    for (auto mp : g.mesh()) g[mp] -= mat;
  }

  /// In-place addition of a scalar (times the identity) to a matrix-valued Green's function.
  template <Mesh M> inline void operator+=(gf_view<M> g, dcomplex a) { g += make_regular(a * nda::eye<double>(g.target_shape()[0])); }

  /// In-place subtraction of a scalar (times the identity) from a matrix-valued Green's function.
  template <Mesh M> inline void operator-=(gf_view<M> g, dcomplex a) { g -= make_regular(a * nda::eye<double>(g.target_shape()[0])); }

  /// In-place addition of a scalar to a scalar-valued Green's function.
  template <Mesh M> inline void operator+=(gf_view<M, scalar_valued> g, dcomplex a) { g.data() += a; }

  /// In-place subtraction of a scalar from a scalar-valued Green's function.
  template <Mesh M> inline void operator-=(gf_view<M, scalar_valued> g, dcomplex a) { g.data() -= a; }

  /** @} */

} // namespace triqs::gfs
