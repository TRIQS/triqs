// Copyright (c) 2023 Simons Foundation
// Copyright (c) 2023 Hugo U.R. Strand
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
// Authors: Olivier Parcollet, Hugo U. R. Strand, Nils Wentzell

/**
 * @file
 * @brief Provides expression templates for \f$ \mathbf{k} \f$-vectors.
 */

#pragma once

#include <nda/nda.hpp>

#include <cstdint>
#include <utility>

namespace triqs::mesh {

  /**
   * @addtogroup triqs-meshes-kexpr
   * @{
   */

  /**
   * @brief Unary minus \f$ \mathbf{k} \f$-vector expression.
   *
   * @tparam OP Operator tag.
   * @tparam L Operand type.
   */
  template <char OP, typename L>
    requires(OP == '-')
  struct k_expr_unary {
    // Operand to be negated.
    L l;

    /// Get the hash value of the mesh to which the operand belongs.
    [[nodiscard]] uint64_t mesh_hash() const { return l.mesh_hash(); };

    /// Get the reciprocal vector \f$ -\mathbf{k} \f$ (see triqs::mesh::brzone::mesh_point_t::value()).
    [[nodiscard]] auto value() const { return -l.value(); }

    /// Get the index of the reciprocal vector \f$ -\mathbf{k} \f$ (see triqs::mesh::brzone::mesh_point_t::index()).
    [[nodiscard]] auto index() const { return -l.index(); }
  };

  /**
   * @brief Binary \f$ \mathbf{k} \f$-vector expression.
   *
   * @tparam OP Operator tag.
   * @tparam L Left operand type (either a scalar or a triqs::mesh::brzone::mesh_point_t).
   * @tparam R Right operand type (a triqs::mesh::brzone::mesh_point_t).
   */
  template <char OP, typename L, typename R> struct k_expr {
    // Left hand side operand.
    L l;

    // Right hand side operand.
    R r;

    /**
     * @brief Construct a binary \f$ \mathbf{k} \f$-vector expression with the given operands.
     *
     * @param l1 Left hand side operand.
     * @param r1 Right hand side operand.
     */
    template <typename L1, typename R1> k_expr(L1 &&l1, R1 &&r1) : l{std::forward<L1>(l1)}, r{std::forward<R1>(r1)} {}

    /**
     * @brief Evaluate the expression template depending on the operator tag.
     *
     * @details The following expressions are supported:
     * - \f$ \mathbf{k}_1 + \mathbf{k}_2 \f$: Adding two k-vectors.
     * - \f$ \mathbf{k}_1 - \mathbf{k}_2 \f$: Subtracting two k-vectors.
     * - \f$ c \mathbf{k} \f$: Multiplying a k-vector by a scalar \f$ c \f$.
     *
     * @return Evaluated expression.
     */
    [[nodiscard]] auto value() const {
      if constexpr (OP == '+') {
        return nda::make_regular(l.value() + r.value());
      } else if constexpr (OP == '-') {
        return nda::make_regular(l.value() - r.value());
      } else {
        return nda::make_regular(l * r.value());
      }
    }

    /**
     * @brief Get the index of the \f$ \mathbf{k} \f$-vector corresponding to the evaluated expression.
     *
     * @details See value() for the supported expressions.
     *
     * @return Index of the evaluated expression.
     */
    [[nodiscard]] auto index() const {
      // check that the mesh hashes are the same
      if constexpr (requires { l.mesh_hash(); }) { EXPECTS(l.mesh_hash() == r.mesh_hash()) };
      if constexpr (OP == '+') {
        return l.index() + r.index();
      } else if constexpr (OP == '-') {
        return l.index() - r.index();
      } else {
        return l * r.index();
      }
    }

    /// Get the hash value of the mesh to which the right hand side operand belongs.
    [[nodiscard]] uint64_t mesh_hash() const { return r.mesh_hash(); };
  };

  /**
   * @brief Type trait to check if a type is a triqs::mesh::k_expr or triqs::mesh::k_expr_unary.
   * @tparam T Type to check.
   */
  template <typename T> constexpr bool is_k_expr = false;

  // Specialization of triqs::mesh::is_k_expr for triqs::mesh::k_expr.
  template <char OP, typename L, typename R> constexpr bool is_k_expr<k_expr<OP, L, R>> = true;

  // Specialization of triqs::mesh::is_k_expr for triqs::mesh::k_expr_unary.
  template <char OP, typename L> constexpr bool is_k_expr<k_expr_unary<OP, L>> = true;

  /** @} */

} // namespace triqs::mesh
