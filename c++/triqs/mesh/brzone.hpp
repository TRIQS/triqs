// Copyright (c) 2016-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2016-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2023 Simons Foundation
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
// Authors: Thomas Ayral, Philipp Dumitrescu, Dominik Kiese, Olivier Parcollet, Hugo U. R. Strand, Nils Wentzell

/**
 * @file
 * @brief Provides a mesh type for Brillouin zones.
 */

#pragma once

#include "./k_expr.hpp"
#include "./mesh_iterator.hpp"
#include "./utils.hpp"
#include "../lattice/brillouin_zone.hpp"

#include <fmt/ranges.h>
#include <h5/h5.hpp>
#include <nda/nda.hpp>

#include <array>
#include <cmath>
#include <cstdint>
#include <concepts>
#include <iostream>
#include <limits>
#include <mutex>
#include <optional>
#include <ranges>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

namespace triqs::mesh {

  /**
   * @ingroup triqs-meshes-utils
   * @brief Bring triqs::lattice::brillouin_zone into the triqs::mesh namespace.
   */
  using lattice::brillouin_zone;

  /**
   * @ingroup triqs-meshes-lattice
   * @brief Brillouin zone mesh type.
   *
   * @details A Brillouin zone (BZ) mesh satisfies the triqs::mesh::MeshWithValues concept and is defined by an
   * underlying triqs::lattice::brillouin_zone and the number of mesh points in the primitive reciprocal unit cell along
   * each of the three dimensions, \f$ N_1 \f$, \f$ N_2 \f$ and \f$ N_3 \f$. The periodic boundary conditions (PBC) in
   * real space (see triqs::mesh::cyclat) translate in reciprocal space to \f$ f(\mathbf{k}) = f(\mathbf{k} + 
   * \mathbf{G}^{\mathbf{m}}) \f$, where \f$ \mathbf{G}^{\mathbf{m}} = \sum_{i=1}^3 \mathbf{b}_i m_i = \mathbf{B} 
   * \mathbf{m} \f$ is a reciprocal lattice vector, \f$ \mathbf{b}_i \f$ is a reciprocal lattice basis vector in 
   * direction \f$ i \f$ and \f$ \mathbf{m} = (m_1, m_2, m_3) \f$ with \f$ m_i \in \mathbb{Z} \f$. 
   *
   * It has the following properties:
   *
   * - Each mesh point is identified by
   *   - an unique index in the first BZ \f$ \mathbf{n} = (n_1, n_2, n_3) \f$, where \f$ 0 \leq n_i < N_i \f$, and
   *   - an infinite set of indices due to the periodic boundary conditions, i.e. \f$ \{ \tilde{\mathbf{n}} = \mathbf{n}
   *   + \mathbf{N} \mathbf{m} : \mathbf{N} \mathbf{m} = (N_1 m_1, N_2 m_2, N_3 m_3) \in \mathbb{Z}^3 \} \f$.
   * - The size of the mesh is \f$ N = N_1 \, N_2 \, N_3 \f$, i.e. the total number of mesh points in the BZ.
   * - An index \f$ \mathbf{n} \f$ is mapped to the corresponding data index \f$ d \f$ by the function \f$ d(\mathbf{n})
   * = n_3 + N_3 (n_2 + N_2 n_1) = n_3 + n_2 N_3 + n_1 N_2 N_3 \f$. The inverse map is \f$ \mathbf{n}(d) = (\lfloor d /
   * s_1 \rfloor, \lfloor (d \mod s_1) / s_2 \rfloor, (d \mod s_1) \mod s_2 ) \f$, where \f$ s_1 = N_2 N_3 \f$ and \f$
   * s_2 = N_3 \f$.
   * - An index \f$ \mathbf{n} \f$ is mapped to the corresponding value (\f$ \mathbf{k} \f$-vector) \f$ \mathbf{k}
   * (\mathbf{n}) \equiv \mathbf{k}^{\mathbf{n}} = \sum_{i=1}^3 \mathbf{b}_i n_i / N_i = \tilde{\mathbf{B}} \mathbf{n} 
   * \f$, where \f$ \mathbf{b}_i \f$ is the reciprocal lattice basis vector in direction \f$ i \f$. The inverse map is 
   * then \f$ \mathbf{n}(\mathbf{k}^{\mathbf{n}}) = \tilde{\mathbf{B}}^{-1} \mathbf{k}^{\mathbf{n}} = \mathbf{n} \f$.
   *
   * @ref triqs-gfs containers that are based on a BZ mesh store the function values at the discrete \f$ \mathbf{k} 
   * \f$-points \f$ \mathbf{k}^{\mathbf{n}} \f$, i.e. \f$ f_{\mathbf{n}} = f(\mathbf{k}^{\mathbf{n}}) \f$.
   * Because of the PBC, the container only has to store values for indices with \f$ 0 \leq n_i < N_i \f$. To evaluate
   * the function 
   * - at an arbitrary \f$ \mathbf{k} \f$-vector, \f$ \mathbf{k} \f$ is first mapped to the BZ using PBC and then 
   * trilinear interpolation is performed (see triqs::mesh::brzone::evaluate(brzone const &, auto const &, V const &) 
   * for details).
   * - at one of the \f$ \mathbf{k}^{\mathbf{n}} \f$, it is first mapped to the BZ using PBC and then the corresponding
   * function value is returned (see triqs::mesh::brzone::evaluate(brzone const &, auto const &, index_t const &) for 
   * details).
   *
   * @code
   * #include <fmt/ranges.h>
   * #include <triqs/mesh.hpp>
   * 
   * #include <numbers>
   * 
   * int main() {
   *   using std::numbers::pi;
   * 
   *   // initialize a mesh on a cubic BZ with a = 2pi and a 2x2x3 k-point grid
   *   triqs::mesh::brzone m{triqs::mesh::brillouin_zone{triqs::mesh::bravais_lattice{nda::eye<double>(3) * 2 * pi}}, {2, 2, 3}};
   * 
   *   // loop over all mesh points and print their index, data index and value (k-point)
   *   for (int i = 0; auto mp : m) {
   *     fmt::println("mesh point #{}: index = {}, data index = {}, value = {::.4f}", i++, mp.index(), mp.data_index(), mp.value());
   *   }
   * }
   * @endcode
   *
   * Output:
   *
   * ```
   * mesh point #0: index = [0, 0, 0], data index = 0, value = [0.0000, 0.0000, 0.0000]
   * mesh point #1: index = [0, 0, 1], data index = 1, value = [0.0000, 0.0000, 0.3333]
   * mesh point #2: index = [0, 0, 2], data index = 2, value = [0.0000, 0.0000, 0.6667]
   * mesh point #3: index = [0, 1, 0], data index = 3, value = [0.0000, 0.5000, 0.0000]
   * mesh point #4: index = [0, 1, 1], data index = 4, value = [0.0000, 0.5000, 0.3333]
   * mesh point #5: index = [0, 1, 2], data index = 5, value = [0.0000, 0.5000, 0.6667]
   * mesh point #6: index = [1, 0, 0], data index = 6, value = [0.5000, 0.0000, 0.0000]
   * mesh point #7: index = [1, 0, 1], data index = 7, value = [0.5000, 0.0000, 0.3333]
   * mesh point #8: index = [1, 0, 2], data index = 8, value = [0.5000, 0.0000, 0.6667]
   * mesh point #9: index = [1, 1, 0], data index = 9, value = [0.5000, 0.5000, 0.0000]
   * mesh point #10: index = [1, 1, 1], data index = 10, value = [0.5000, 0.5000, 0.3333]
   * mesh point #11: index = [1, 1, 2], data index = 11, value = [0.5000, 0.5000, 0.6667]
   * ```
   */
  class brzone {
    public:
    /// Value type.
    using value_t = brillouin_zone::value_t;

    /// Index type
    using index_t = std::array<long, 3>;

    /// Data index type.
    using data_index_t = long;

    private:
    brillouin_zone bz_        = {};
    std::array<long, 3> dims_ = {0, 0, 0};
    long size_                = 0;
    long stride1 = 1, stride0 = 1;
    nda::matrix<double> units_     = nda::eye<double>(3);
    nda::matrix<double> units_inv_ = nda::eye<double>(3);
    uint64_t _mesh_hash            = 0;

    public:
    /// Default constructor constructs an empty mesh.
    brzone() = default;

    /**
     * @brief Construct a Brillouin zone mesh with the given number of mesh points.
     *
     * @param bz triqs::lattice::brillouin_zone object representing the underlying BZ.
     * @param dims Number of mesh points along each of the three dimensions, i.e. \f$ (N_1, N_2, N_3) \f$.
     */
    brzone(brillouin_zone const &bz, std::array<long, 3> const &dims)
       : bz_(bz),
         dims_(dims),
         size_(nda::stdutil::product(dims)),
         stride1(dims_[2]),
         stride0(dims_[1] * dims_[2]),
         units_(nda::linalg::inv(1.0 * nda::diag(dims)) * bz.units()),
         units_inv_(nda::linalg::inv(units_)),
         _mesh_hash(hash(nda::sum(bz.units()), dims[0], dims[1], dims[2])) {}

    /**
     * @brief Construct a Brillouin zone mesh with the given periodization matrix.
     * 
     * @note At the moment, only diagonal periodization matrices are supported. They should contain the number of mesh
     * points along each of the three dimensions on the diagonal.
     *
     * @param bz triqs::lattice::brillouin_zone object representing the underlying Bravais lattice.
     * @param M \f$ 3 \times 3 \f$ periodization matrix.
     */
    brzone(brillouin_zone const &bz, nda::matrix<long> const &M) : brzone(bz, std::array{M(0, 0), M(1, 1), M(2, 2)}) {
      EXPECTS((M.shape() == std::array{3l, 3l}));
      EXPECTS(nda::is_matrix_diagonal(M));
    }

    /**
     * @brief Construct a Brillouin zone mesh with the same number of mesh points in each direction.
     *
     * @param bz triqs::lattice::brillouin_zone object representing the underlying Bravais lattice.
     * @param n Number of mesh points along each of the three dimensions.
     */
    brzone(brillouin_zone const &bz, long n) : brzone(bz, std::array{n, (bz.ndim() >= 2 ? n : 1l), (bz.ndim() >= 3 ? n : 1)}) {}

    /// Equal-to comparison operator compares the hash values.
    bool operator==(brzone const &m) const { return mesh_hash() == m.mesh_hash(); }

    /// Not-equal-to comparison operator compares the hash values.
    bool operator!=(brzone const &m) const { return !(operator==(m)); }

    /// Get the hash value of the mesh.
    [[nodiscard]] uint64_t mesh_hash() const { return _mesh_hash; }

    /// Get the size \f$ N \f$ of the mesh, i.e. the total number of mesh points in the first BZ.
    [[nodiscard]] long size() const { return size_; }

    /// Get the number of mesh points in each of the three dimensions.
    [[nodiscard]] auto const &dims() const { return dims_; }

    /// Get the matrix \f$ \tilde{\mathbf{B}}^T \f$ containing the scaled reciprocal basis vectors in its rows.
    [[nodiscard]] auto units() const { return nda::matrix_const_view<double>{units_}; }

    /// Get the underlying Brillouin zone.
    [[nodiscard]] auto const &bz() const noexcept { return bz_; }

    /**
     * @brief Map an arbitrary index \f$ \tilde{\mathbf{n}} \f$ to the unique index \f$ \mathbf{n} \f$ in the first BZ.
     *
     * @param n_tilde Index \f$ \tilde{\mathbf{n}} \f$ to map back to the first BZ.
     * @return Corresponding index \f$ \mathbf{n} \f$ in the first BZ such that \f$ \tilde{\mathbf{n}} = \mathbf{n} +
     * \mathbf{N} \mathbf{m} \f$.
     */
    [[nodiscard]] index_t index_modulo(index_t const &n_tilde) const {
      return {positive_modulo(n_tilde[0], dims_[0]), positive_modulo(n_tilde[1], dims_[1]), positive_modulo(n_tilde[2], dims_[2])};
    }

    /**
     * @brief %Mesh point of a triqs::mesh::brzone mesh.
     * 
     * @details It stores the index \f$ \mathbf{n} \f$, data index \f$ d(\mathbf{n}) \f$, a pointer to and the hash
     * value of the parent mesh, and the value \f$ \mathbf{k}^{\mathbf{n}} \f$.
     */
    struct mesh_point_t {
      /// Parent mesh type.
      using mesh_t = brzone;

      private:
      std::array<long, 3> _index            = {0, 0, 0};
      brzone const *_m_ptr                  = nullptr;
      long _data_index                      = 0;
      uint64_t _mesh_hash                   = 0;
      mutable std::optional<value_t> _value = {};
      mutable std::mutex value_mutex        = {};

      public:
      /// Default constructor leaves the mesh point uninitialized.
      mesh_point_t() = default;

      /**
       * @brief Construct a mesh point with a given index \f$ \mathbf{n} \f$, pointer to the Brillouin zone mesh the 
       * mesh point belongs to and data index \f$ d \f$. 
       *
       * @param n Index \f$\mathbf{n} \f$ of the mesh point.
       * @param m_ptr Pointer to a triqs::mesh::brzone object.
       * @param d Data index \f$ d \f$ of the mesh point.
       */
      mesh_point_t(std::array<long, 3> const &n, brzone const *m_ptr, long d)
         : _index(n), _m_ptr(m_ptr), _data_index(d), _mesh_hash(m_ptr->mesh_hash()) {}

      /// Copy constructor to handle the presence of the `std::mutex` object correctly.
      mesh_point_t(mesh_point_t const &mp)
         : _index(mp._index), _m_ptr(mp._m_ptr), _data_index(mp._data_index), _mesh_hash(mp._mesh_hash), _value(mp._value) {}

      /// Get the index \f$ \mathbf{n} \f$ of the mesh point.
      [[nodiscard]] mesh_t::index_t index() const { return _index; }

      /// Get the data index \f$ d \f$ of the mesh point.
      [[nodiscard]] long data_index() const { return _data_index; }

      /// Get the reciprocal vector \f$ \mathbf{k}^\mathbf{n} \f$ corresponding to the mesh point.
      [[nodiscard]] value_t const &value() const {
        if (_value)
          return *_value;
        else {
          auto guard = std::lock_guard{value_mutex};
          if (_value)
            return *_value;
          else
            return *(_value = _m_ptr->to_value(_index));
        }
      }

      /// Get the hash value of the parent mesh.
      [[nodiscard]] uint64_t mesh_hash() const noexcept { return _mesh_hash; }

      /// Conversion to the value type of the parent mesh.
      [[nodiscard]] operator value_t() const { return value(); }

      /**
       * @brief Get the coordinate \f$ k^{\mathbf{n}}_i \f$ of the corresponding reciprocal vector \f$
       * \mathbf{k}^{\mathbf{n}} \f$.
       *
       * @deprecated Use operator[]() instead.
       */
      [[deprecated("() is deprecated for a brzone::mesh_point_t. Use [] instead")]] double operator()(int d) const { return value()[d]; }

      /**
       * @brief Get the coordinate \f$ k^{\mathbf{n}}_i \f$ of the corresponding reciprocal vector \f$
       * \mathbf{k}^{\mathbf{n}} \f$.
       *
       * @param i Dimension index \f$ i \f$.
       * @return Reciprocal vector coordinate \f$ k^{\mathbf{n}}_i \f$.
       */
      [[nodiscard]] double operator[](int i) const { return value()[i]; }

      /**
       * @brief Write triqs::mesh::brzone::mesh_point_t to a `std::ostream`.
       *
       * @param sout `std::ostream` object.
       * @param mp %Mesh point to be written.
       * @return Reference to `std::ostream` object.
       */
      friend std::ostream &operator<<(std::ostream &sout, mesh_point_t const &mp) { return sout << mp.value(); }
    };

    /**
     * @brief Check if an index \f$ \mathbf{n} \f$ is valid, i.e. corresponds to a \f$ \mathbf{k}^\mathbf{n} \f$ in the
     * first BZ.
     *
     * @param n Index \f$ \mathbf{n} \f$ to check.
     * @return True if \f$ 0 \leq n_i < N_i \; \forall i \f$, false otherwise.
     */
    [[nodiscard]] bool is_index_valid(index_t const &n) const noexcept {
      for (auto i : nda::range(3))
        if (n[i] < 0 or n[i] >= dims_[i]) return false;
      return true;
    }

    /**
     * @brief Map an index \f$ \mathbf{n} \f$ to its corresponding data index \f$ d(\mathbf{n}) \f$.
     *
     * @param n Index \f$ \mathbf{n} \f$ to map.
     * @return Data index \f$ d(\mathbf{n}) = n_3 + N_3 (n_2 + N_2 n_1) \f$.
     */
    [[nodiscard]] data_index_t to_data_index(index_t const &n) const {
      EXPECTS(is_index_valid(n));
      return n[0] * stride0 + n[1] * stride1 + n[2];
    }

    /**
     * @brief Map a \f$ \mathbf{k} \f$-vector \f$ \mathbf{k}^{\mathbf{n}} \f$ to its data index \f$ 
     * d(\mathbf{k}^{\mathbf{n}}) \f$.
     *
     * @tparam V \f$ \mathbf{k} \f$-vector or expression type.
     * @param cmp triqs::mesh::closest_mesh_point_t containing \f$ \mathbf{k}^{\mathbf{n}} \f$.
     * @return Data index \f$ d(\mathbf{k}^{\mathbf{n}}) = n_3 + N_3 (n_2 + N_2 n_1) \f$.
     */
    template <typename V> [[nodiscard]] data_index_t to_data_index(closest_mesh_point_t<V> const &cmp) const {
      return to_data_index(closest_index(cmp.value));
    }

    /**
     * @brief Map an unary \f$ \mathbf{k} \f$-expression to a data index \f$ d \f$.
     *
     * @tparam OP Operator tag.
     * @tparam L Operand type.
     * @param ex triqs::mesh::k_expr_unary object.
     * @return Data index corresponding to the \f$ \mathbf{k} \f$-point after evaluating the expression and mapping it
     * back to the first BZ.
     */
    template <char OP, typename L> [[nodiscard]] data_index_t to_data_index(k_expr_unary<OP, L> const &ex) const {
      EXPECTS(_mesh_hash == ex.mesh_hash());
      return to_data_index(index_modulo(ex.index()));
    }

    /**
     * @brief Map a binary \f$ \mathbf{k} \f$-expression to a data index \f$ d \f$.
     *
     * @tparam OP Operator tag.
     * @tparam L Left operand type.
     * @tparam R Right operand type.
     * @param ex triqs::mesh::k_expr object.
     * @return Data index corresponding to the \f$ \mathbf{k} \f$-point after evaluating the expression and mapping it
     * back to the first BZ.
     */
    template <char OP, typename L, typename R> [[nodiscard]] data_index_t to_data_index(k_expr<OP, L, R> const &ex) const {
      EXPECTS(_mesh_hash == ex.mesh_hash());
      return to_data_index(index_modulo(ex.index()));
    }

    /**
     * @brief Map a data index \f$ d \in \{0, 1, \ldots, N-1\} \f$ to the corresponding index \f$ \mathbf{n}(d) \f$.
     *
     * @param d Data index \f$ d \f$ to map.
     * @return Index \f$ \mathbf{n}(d) = (\lfloor d / s_1 \rfloor, \lfloor (d \mod s_1) / s_2 \rfloor, (d \mod s_1) 
     * \mod s_2 ) \f$.
     */
    [[nodiscard]] index_t to_index(data_index_t d) const {
      EXPECTS(0 <= d and d < size());
      long i0 = d / stride0;
      long r0 = d % stride0;
      long i1 = r0 / stride1;
      long i2 = r0 % stride1;
      return {i0, i1, i2};
    }

    /**
     * @brief Map a given \f$ \mathbf{k} \f$-vector or expression to the closest \f$ \mathbf{k}^{\mathbf{n}} \f$ in the 
     * first BZ and return its index \f$ \mathbf{n} \f$.
     *
     * @tparam V \f$ \mathbf{k} \f$-vector or expression type.
     * @param k \f$ \mathbf{k} \f$-vector or expression to map.
     * @return Index \f$ \mathbf{n} \f$ of the closest \f$ \mathbf{k}^{\mathbf{n}} \f$ in the first BZ.
     */
    template <typename V>
      requires(is_k_expr<V> or std::ranges::contiguous_range<V> or nda::ArrayOfRank<V, 1>)
    [[nodiscard]] index_t closest_index(V const &k) const {

      if constexpr (is_k_expr<V>) {
        return closest_index(k.value());
      } else {
        // calculate k in the brzone basis
        auto ks      = nda::stack_vector<double, 3>{k[0], k[1], k[2]};
        auto k_units = nda::transpose(units_inv_) * ks;
        auto n       = nda::stack_vector<long, 3>(nda::floor(k_units));

        // calculate position relative to neighbors in mesh
        auto w = k_units - n;

        // prepare result container and distance measure
        auto dst = std::numeric_limits<double>::infinity();

        // check flatness along mesh dimensions
        long r1 = std::min(dims_[0], 2l);
        long r2 = std::min(dims_[1], 2l);
        long r3 = std::min(dims_[2], 2l);

        // find nearest neighbor by comparing distances
        nda::stack_vector<long, 3> res;
        for (auto const &[i1, i2, i3] : itertools::product_range(r1, r2, r3)) {
          auto iv   = nda::stack_vector<long, 3>{i1, i2, i3};
          auto dstp = nda::linalg::norm(nda::transpose(units_) * (w - iv));

          // update result when distance is smaller than current
          if (dstp < dst) {
            dst = dstp;
            res = n + iv;
          }
        }

        // fold back to brzone mesh (nearest neighbor could be out of bounds)
        return index_modulo({res[0], res[1], res[2]});
      }
    }

    /**
     * @brief Map a given \f$ \mathbf{k} \f$-vector or expression to the closest \f$ \mathbf{k}^{\mathbf{n}} \f$ in the 
     * first BZ and return its index \f$ \mathbf{n} \f$.
     *
     * @tparam V \f$ \mathbf{k} \f$-vector or expression type.
     * @param cmp triqs::mesh::closest_mesh_point_t containing the \f$ \mathbf{k} \f$-vector or expression.
     * @return Index \f$ \mathbf{n} \f$.
     */
    template <typename V> [[nodiscard]] index_t to_index(closest_mesh_point_t<V> const &cmp) const { return closest_index(cmp.value); }

    /**
     * @brief Subscript operator to access a mesh point by its data index \f$ d \in \{0, 1, \ldots, N-1\} \f$.
     *
     * @param d Data index \f$ d \f$ of the mesh point.
     * @return mesh_point_t with the index \f$ \mathbf{n}(d) = (\lfloor d / s_1 \rfloor, \lfloor (d \mod s_1) / s_2 
     * \rfloor, (d \mod s_1) \mod s_2 ) \f$, data index \f$ d \f$ and a pointer to the current mesh.
     */
    [[nodiscard]] mesh_point_t operator[](long d) const { return {to_index(d), this, d}; }

    /**
     * @brief Subscript operator to access a mesh point by a \f$ \mathbf{k} \f$-point \f$ \mathbf{k}^{\mathbf{n}} \f$
     * contained in a triqs::mesh::closest_mesh_point_t.
     *
     * @param cmp triqs::mesh::closest_mesh_point_t containing the \f$ \mathbf{k} \f$-point.
     * @return mesh_point_t with the index \f$ \mathbf{n} \f$, data index \f$ d(\mathbf{n}) = d(\mathbf{n}) = n_3 + N_3 
     * (n_2 + N_2 n_1) \f$ and a pointer to the current mesh.
     */
    [[nodiscard]] mesh_point_t operator[](closest_mesh_point_t<value_t> const &cmp) const { return (*this)[this->to_data_index(cmp)]; }

    /**
     * @brief Function call operator to access a mesh point by its index \f$ \mathbf{n} \f$.
     *
     * @param n Index \f$ \mathbf{n} \f$ of the mesh point.
     * @return mesh_point_t with the index \f$ \mathbf{n} \f$, data index \f$ d(\mathbf{n}) = d(\mathbf{n}) = n_3 + N_3 
     * (n_2 + N_2 n_1) \f$ and a pointer to the current mesh.
     */
    [[nodiscard]] mesh_point_t operator()(index_t const &n) const { return {n, this, to_data_index(n)}; }

    /**
     * @brief Map an index \f$ \mathbf{n} \f$ to its corresponding \f$ \mathbf{k} \f$-point \f$ \mathbf{k}^{\mathbf{n}} 
     * \f$.
     *
     * @param n Index \f$ \mathbf{n} \f$ to map.
     * @return `nda::vector<double>` \f$ \mathbf{k}^{\mathbf{n}} \f$.
     */
    [[nodiscard]] value_t to_value(index_t const &n) const {
      EXPECTS(is_index_valid(n));
      return nda::transpose(units_)(nda::range::all, nda::range(bz_.ndim())) * nda::basic_array_view{n}(nda::range(bz_.ndim()));
    }

    /**
     * @brief Write a triqs::mesh::brzone mesh to a `std::ostream`.
     *
     * @param sout `std::ostream` object.
     * @param m %Mesh to be written.
     * @return Reference to `std::ostream` object.
     */
    friend std::ostream &operator<<(std::ostream &sout, brzone const &m) {
      return sout << "Brillouin zone mesh with linear dimensions " << m.dims() << "\n -- units = " << m.units() << "\n -- brillouin_zone: " << m.bz();
    }

    /// Get an iterator to the beginning of the mesh.
    [[nodiscard]] auto begin() const { return mesh_iterator<brzone>{.mesh_ptr = this, .data_index = 0}; }

    /// Get a const iterator to the beginning of the mesh.
    [[nodiscard]] auto cbegin() const { return begin(); }

    /// Get an iterator to the end of the mesh.
    [[nodiscard]] auto end() const { return mesh_iterator<brzone>{.mesh_ptr = this, .data_index = size()}; }

    /// Get a const iterator to the end of the mesh.
    [[nodiscard]] auto cend() const { return end(); }

    /**
     * @brief Serialize the mesh to a generic archive.
     * @param ar Archive to serialize to.
     */
    void serialize(auto &ar) const { ar & bz_ & dims_ & size_ & stride1 & stride0 & units_ & units_inv_ & _mesh_hash; }

    /**
     * @brief Deserialize the mesh from a generic archive.
     * @param ar Archive to deserialize from.
     */
    void deserialize(auto &ar) { ar & bz_ & dims_ & size_ & stride1 & stride0 & units_ & units_inv_ & _mesh_hash; }

    /// Get the HDF5 format tag.
    [[nodiscard]] static std::string hdf5_format() { return "MeshBrillouinZone"; }

    /**
     * @brief Write a triqs::mesh::brzone mesh to HDF5.
     *
     * @param g `h5::group` to be written to.
     * @param name Name of the subgroup.
     * @param m %Mesh object to be written.
     */
    friend void h5_write(h5::group g, std::string const &name, brzone const &m) {
      h5::group gr = g.create_group(name);
      h5::write_hdf5_format(gr, m); // NOLINT (downcasting to base class)
      h5::write(gr, "dims", m.dims_);
      h5::write(gr, "brillouin_zone", m.bz_);
    }

    /**
     * @brief Read a triqs::mesh::brzone mesh from HDF5.
     *
     * @param g `h5::group` to be read from.
     * @param name Name of the subgroup.
     * @param m %Mesh object to be read into.
     */
    friend void h5_read(h5::group g, std::string const &name, brzone &m) {
      h5::group gr = g.open_group(name);
      h5::assert_hdf5_format(gr, m, true);

      std::array<long, 3> dims{};
      if (gr.has_key("dims")) {
        h5::read(gr, "dims", dims);
      } else {
        // for backward compatibility
        auto M = h5::read<matrix<long>>(gr, "periodization_matrix");
        dims   = {M(0, 0), M(1, 1), M(2, 2)};
      }

      brillouin_zone bz{};
      if (gr.has_key("brillouin_zone")) {
        h5::read(gr, "brillouin_zone", bz);
      } else if (gr.has_key("bz")) {
        // for backward compatibility
        h5::read(gr, "bz", bz);
      } else {
        std::cout << "WARNING: Reading old MeshBrillouinZone without BrillouinZone\n";
      }

      m = brzone(bz, dims);
    }

    /**
     * @brief Evaluate a function \f$ f \f$ defined on a triqs::mesh::brzone mesh at the given index \f$ \tilde{
     * \mathbf{n}} \f$.
     * 
     * @details The index is first mapped to the first BZ using triqs::mesh::brzone::index_modulo and then it is used
     * to access the correct function value \f$ f_{\tilde{\mathbf{n}}} = f_{\mathbf{n}} \f$.
     *
     * @param m triqs::mesh::brzone mesh.
     * @param f Callable object \f$ f \f$ containing the function values \f$ f_{\mathbf{n}} = f(\mathbf{k}^{\mathbf{n}}) 
     * \f$ at the mesh points.
     * @param n_tilde Index \f$ \tilde{\mathbf{n}} \f$ at which to evaluate the function.
     * @return Function value \f$ f_{\mathbf{n}} \f$.
     */
    friend auto evaluate(brzone const &m, auto const &f, index_t const &n_tilde) { return f(m.index_modulo(n_tilde)); }

    private:
    // Helper struct to evaluate a function at an arbitrary k-vector.
    struct brzone1d {
      long dim;
    };

    // Helper function to do linear interpolation between k-points.
    friend auto evaluate(brzone1d const &m, auto const &f, double vi) {
      long i   = static_cast<long>(std::floor(vi));
      double w = vi - double(i);
      return (1 - w) * f(positive_modulo(i, m.dim)) + w * f(m.dim == 1 ? 0 : positive_modulo(i + 1, m.dim));
    }

    /**
     * @brief Trilinear interpolation of a function \f$ f \f$ defined on a triqs::mesh::brzone mesh at a given \f$ 
     * \mathbf{k} \f$-vector or expression.
     * 
     * @details It first maps the \f$ \mathbf{k} \f$-vector back to the first BZ and then performs trilinear 
     * interpolation of the function \f$ f \f$ in the volume spanned by the \f$ \mathbf{k}^{\mathbf{n}} \f$-points that
     * enclose the mapped \f$ \mathbf{k} \f$-vector.
     *
     * @tparam V \f$ \mathbf{k} \f$-vector or expression type.
     * @param m triqs::mesh::brzone mesh.
     * @param f Callable object \f$ f \f$ containing the function values \f$ f_{\mathbf{n}} = f(\mathbf{k}^{\mathbf{n}}) 
     * \f$ at the mesh points.
     * @param k \f$ \mathbf{k} \f$-vector or expression at which to evaluate the function.
     * @return Trilinear interpolation of \f$ f(\mathbf{k}) \f$.
     */
    template <typename V>
      requires(std::ranges::contiguous_range<V> or nda::ArrayOfRank<V, 1> or is_k_expr<V>)
    friend auto evaluate(brzone const &m, auto const &f, V const &k) {
      if constexpr (is_k_expr<V>)
        return evaluate(m, f, k.value());
      else {
        auto v_index      = make_regular(transpose(m.units_inv_) * nda::basic_array_view{k});
        auto g            = [&f](long x, long y, long z) { return f(typename brzone::index_t{x, y, z}); };
        auto [d0, d1, d2] = m.dims();
        return evaluate(std::tuple{brzone1d{d0}, brzone1d{d1}, brzone1d{d2}}, g, v_index[0], v_index[1], v_index[2]);
      }
    }
  };

  /**
   * @addtogroup triqs-meshes-kexpr
   * @{
   */

  /**
   * @brief Concept for a Brillouin zone mesh point.
   *
   * @details A type `T` is a Brillouin zone mesh point
   * - if triqs::mesh::is_k_expr is true for `T` or
   * - if it is a triqs::mesh::brzone::mesh_point_t.
   *
   * @tparam T Type to check.
   */
  template <typename T>
  concept BzMeshPoint = is_k_expr<std::decay_t<T>> or std::is_same_v<std::decay_t<T>, brzone::mesh_point_t>;

  /**
   * @brief Lazy unary minus for triqs::mesh::BzMeshPoint objects.
   *
   * @tparam L triqs::mesh::BzMeshPoint type.
   * @param l Operand to negate.
   * @return triqs::mesh::k_expr_unary expression representing the negation of a BZ mesh point.
   */
  template <BzMeshPoint L> k_expr_unary<'-', L> operator-(L &&l) { return {std::forward<L>(l)}; }

  /**
   * @brief Lazy addition of two triqs::mesh::BzMeshPoint objects.
   *
   * @tparam L triqs::mesh::BzMeshPoint type.
   * @tparam R triqs::mesh::BzMeshPoint type.
   * @param l Left hand side operand.
   * @param r Right hand side operand.
   * @return triqs::mesh::k_expr object representing the addition of two BZ mesh points.
   */
  template <BzMeshPoint L, BzMeshPoint R> k_expr<'+', L, R> operator+(L &&l, R &&r) { return {std::forward<L>(l), std::forward<R>(r)}; }

  /**
   * @brief Lazy subtraction of two triqs::mesh::BzMeshPoint objects.
   *
   * @tparam L triqs::mesh::BzMeshPoint type.
   * @tparam R triqs::mesh::BzMeshPoint type.
   * @param l Left hand side operand.
   * @param r Right hand side operand.
   * @return triqs::mesh::k_expr object representing the subtraction of two BZ mesh points.
   */
  template <BzMeshPoint L, BzMeshPoint R> k_expr<'-', L, R> operator-(L &&l, R &&r) { return {std::forward<L>(l), std::forward<R>(r)}; }

  /**
   * @brief Lazy multiplication of a triqs::mesh::BzMeshPoint object with a scalar.
   *
   * @tparam Int Integral type.
   * @tparam R triqs::mesh::BzMeshPoint type.
   * @param l Left hand side operand, an integer.
   * @param r Right hand side operand, a BZ mesh point.
   * @return triqs::mesh::k_expr object representing the multiplication of a scalar and a BZ mesh point.
   */
  template <std::integral Int, BzMeshPoint R> k_expr<'*', Int, R> operator*(Int l, R &&r) { return {l, std::forward<R>(r)}; }

  /** @} */

  // Check mesh concepts.
  static_assert(Mesh<brzone>);
  static_assert(MeshWithValues<brzone>);

} // namespace triqs::mesh
