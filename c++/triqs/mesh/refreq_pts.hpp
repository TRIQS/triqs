// Copyright (c) 2019-2025 Simons Foundation
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
// Authors: Rok Zitko, Nils Wentzell

/**
 * @file
 * @brief Provides a real frequency mesh type from arbitrary sorted frequency points.
 */

#pragma once

#include "./mesh_iterator.hpp"
#include "./utils.hpp"
#include "../utility/exceptions.hpp"
#include "../utility/macros.hpp"

#include <fmt/format.h>
#include <h5/h5.hpp>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <initializer_list>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

namespace triqs::mesh {

  /**
   * @addtogroup triqs-meshes-real
   * @{
   */

  /**
   * @brief Real frequency mesh type from arbitrary sorted frequency points.
   *
   * @details A refreq_pts mesh satisfies the triqs::mesh::MeshWithValues concept and is defined by
   * a sorted vector of frequency values \f$ \{\omega_0, \omega_1, \ldots, \omega_{N-1}\} \f$ with
   * \f$ \omega_i < \omega_{i+1} \f$.
   *
   * Properties:
   * - Each mesh point is identified by a unique index \f$ n \in \{0, 1, \ldots, N-1\} \f$.
   * - An index \f$ n \f$ is mapped to the corresponding data index \f$ d \f$ by the identity
   *   function \f$ d(n) = n \f$ and vice versa.
   * - An index \f$ n \f$ is mapped to its corresponding value \f$ \omega_n \f$.
   * - An arbitrary value \f$ \omega \in [\omega_0, \omega_{N-1}] \f$ is mapped to the closest mesh
   *   point using binary search.
   *
   * @ref triqs-gfs containers that are based on this mesh store the function values at the discrete
   * frequency points and use linear interpolation to evaluate the function at an arbitrary frequency
   * (see triqs::mesh::evaluate(refreq_pts const &, auto const &, double) for details).
   *
   * @code
   * #include <fmt/base.h>
   * #include <triqs/mesh.hpp>
   *
   * int main() {
   *   // initialize a refreq_pts mesh from a vector of points
   *   triqs::mesh::refreq_pts m{std::vector<double>{-5.0, -1.0, 0.0, 1.0, 5.0}};
   *
   *   // loop over all mesh points and print their index, data index and value
   *   for (int i = 0; auto mp : m)
   *     fmt::println("mesh point #{}: index = {}, data index = {}, value = {}", i++, mp.index(), mp.data_index(), mp.value());
   * }
   * @endcode
   *
   * Output:
   *
   * ```
   * mesh point #0: index = 0, data index = 0, value = -5
   * mesh point #1: index = 1, data index = 1, value = -1
   * mesh point #2: index = 2, data index = 2, value = 0
   * mesh point #3: index = 3, data index = 3, value = 1
   * mesh point #4: index = 4, data index = 4, value = 5
   * ```
   */
  class C2PY_RENAME(MeshReFreqPts) refreq_pts {
    public:
    /// Value type.
    using value_t = double;

    /// Index type.
    using index_t = long;

    /// Data index type.
    using data_index_t = long;

    /**
     * @brief %Mesh point of a triqs::mesh::refreq_pts mesh.
     *
     * @details It stores the index \f$ n \f$, the data index \f$ d \f$, the hash value of the
     * parent mesh and the value \f$ \omega_n \f$ of the mesh point.
     *
     * Arithmetic operations are defined for mesh points and scalars. The operations are performed
     * between the value \f$ \omega_n \f$ of the mesh point and the given scalar.
     */
    class C2PY_IGNORE mesh_point_t {
      public:
      /// Parent mesh type.
      using mesh_t = refreq_pts;

      /// Default constructor leaves the mesh point uninitialized.
      mesh_point_t() = default;

      /**
       * @brief Construct a mesh point with a given index, data index, hash value and value.
       *
       * @param n Index \f$ n \f$ of the mesh point.
       * @param d Data index \f$ d \f$ of the mesh point.
       * @param mhash Hash value of the parent mesh.
       * @param w Value \f$ \omega_n \f$ of the mesh point.
       */
      mesh_point_t(long n, long d, uint64_t mhash, double w) : index_(n), data_index_(d), mesh_hash_(mhash), value_(w) {}

      /// Get the index \f$ n \f$ of the mesh point.
      [[nodiscard]] long index() const noexcept { return index_; }

      /// Get the data index \f$ d \f$ of the mesh point.
      [[nodiscard]] long data_index() const noexcept { return data_index_; }

      /// Get the value \f$ \omega_n \f$ of the mesh point.
      [[nodiscard]] double value() const noexcept { return value_; }

      /// Get the hash value of the parent mesh.
      [[nodiscard]] uint64_t mesh_hash() const noexcept { return mesh_hash_; }

      /// Implicit conversion to the value \f$ \omega_n \f$.
      operator double() const { return value_; } // NOLINT (implicit conversion intended)

      // Arithmetic operations with scalars.
#define IMPL_OP(OP)                                                                                                                                  \
  template <typename U> friend auto operator OP(mesh_point_t const &mp, U &&y) { return mp.value() OP std::forward<U>(y); }                          \
  template <typename U>                                                                                                                              \
    requires(not std::is_same_v<std::decay_t<U>, mesh_point_t>)                                                                                      \
  friend auto operator OP(U &&x, mesh_point_t const &mp) {                                                                                           \
    return std::forward<U>(x) OP mp.value();                                                                                                         \
  }
      IMPL_OP(+)
      IMPL_OP(-)
      IMPL_OP(*)
      IMPL_OP(/)
#undef IMPL_OP

      private:
      long index_         = 0;
      long data_index_    = 0;
      uint64_t mesh_hash_ = 0;
      double value_       = 0.0;
    };

    /// Default constructor creates an empty mesh.
    refreq_pts() = default;

    /**
     * @brief Construct a refreq_pts mesh from a sorted vector of frequency points.
     *
     * @param pts Sorted vector of frequency values.
     * @throws triqs::runtime_error if the vector is not sorted.
     */
    refreq_pts(std::vector<double> pts)
       : pts_(std::move(pts)),
         mesh_hash_(hash(std::accumulate(pts_.begin(), pts_.end(), 0.0), pts_.empty() ? 0.0 : pts_.front(), pts_.empty() ? 0.0 : pts_.back(),
                         static_cast<long>(pts_.size()))) {
      if (not std::ranges::is_sorted(pts_)) TRIQS_RUNTIME_ERROR << "refreq_pts mesh must be constructed with a sorted list of points";
    }

    /**
     * @brief Construct a refreq_pts mesh from an initializer list of frequency points.
     *
     * @param l Initializer list of frequency values.
     */
    C2PY_IGNORE refreq_pts(std::initializer_list<double> l) : refreq_pts(std::vector<double>(l)) {}

    /// Equal-to comparison operator.
    bool operator==(refreq_pts const &) const = default;

    /**
     * @brief Check if an index \f$ n \f$ is valid.
     *
     * @param n Index \f$ n \f$ to check.
     * @return True if \f$ 0 \leq n < N \f$, false otherwise.
     */
    [[nodiscard]] bool is_index_valid(index_t n) const noexcept { return 0 <= n and n < size(); }

    /**
     * @brief Check if a value \f$ \omega \f$ is within the mesh range.
     *
     * @param w Value to check.
     * @return True if \f$ \omega_0 \leq \omega \leq \omega_{N-1} \f$, false otherwise.
     */
    [[nodiscard]] bool is_value_valid(double w) const noexcept { return !pts_.empty() and pts_.front() <= w and w <= pts_.back(); }

    /**
     * @brief Map an index \f$ n \f$ to its corresponding data index \f$ d(n) = n \f$.
     *
     * @param n Index \f$ n \f$ to map.
     * @return Data index \f$ d(n) = n \f$.
     */
    [[nodiscard]] data_index_t to_data_index(index_t n) const noexcept {
      EXPECTS(is_index_valid(n));
      return n;
    }

    /**
     * @brief Map a value to the closest mesh point and return its data index.
     *
     * @param cmp triqs::mesh::closest_mesh_point_t containing the value to map.
     * @return Data index of the closest mesh point.
     */
    [[nodiscard]] C2PY_IGNORE data_index_t to_data_index(closest_mesh_point_t<double> const &cmp) const noexcept {
      return to_data_index(to_index(cmp));
    }

    /**
     * @brief Map a data index \f$ d \f$ to the corresponding index \f$ n(d) = d \f$.
     *
     * @param d Data index \f$ d \f$ to map.
     * @return Index \f$ n(d) = d \f$.
     */
    [[nodiscard]] index_t to_index(data_index_t d) const noexcept {
      EXPECTS(is_index_valid(d));
      return d;
    }

    /**
     * @brief Map a value to the closest mesh point and return its index using binary search.
     *
     * @param cmp triqs::mesh::closest_mesh_point_t containing the value to map.
     * @return Index of the closest mesh point.
     */
    [[nodiscard]] C2PY_IGNORE index_t to_index(closest_mesh_point_t<double> const &cmp) const noexcept {
      EXPECTS(is_value_valid(cmp.value));

      auto itr_r = std::ranges::lower_bound(pts_, cmp.value);
      long i_r   = itr_r - pts_.begin();

      if (i_r == 0) { return 0; }
      if (i_r == size()) { return size() - 1; }

      long i_l = i_r - 1;
      if (std::abs(cmp.value - pts_[i_l]) < std::abs(cmp.value - pts_[i_r]))
        return i_l;
      else
        return i_r;
    }

    /**
     * @brief Subscript operator to access a mesh point by its data index.
     *
     * @param d Data index of the mesh point.
     * @return mesh_point_t at the given data index.
     */
    [[nodiscard]] mesh_point_t operator[](long d) const noexcept { return (*this)(d); }

    /**
     * @brief Subscript operator to access the mesh point closest to a given value.
     *
     * @param cmp triqs::mesh::closest_mesh_point_t containing the value.
     * @return mesh_point_t of the closest mesh point.
     */
    [[nodiscard]] C2PY_IGNORE mesh_point_t operator[](closest_mesh_point_t<double> const &cmp) const noexcept { return (*this)[to_data_index(cmp)]; }

    /**
     * @brief Function call operator to access a mesh point by its index.
     *
     * @param n Index of the mesh point.
     * @return mesh_point_t at the given index.
     */
    [[nodiscard]] mesh_point_t operator()(index_t n) const noexcept {
      EXPECTS(is_index_valid(n));
      return {n, n, mesh_hash_, pts_[n]};
    }

    /**
     * @brief Map an index \f$ n \f$ to its corresponding value \f$ \omega_n \f$.
     *
     * @param n Index to map.
     * @return Value of the mesh point \f$ \omega_n \f$.
     */
    [[nodiscard]] double to_value(index_t n) const noexcept {
      EXPECTS(is_index_valid(n));
      return pts_[n];
    }

    /// Get the hash value of the mesh.
    [[nodiscard]] C2PY_PROPERTY_GET(mesh_hash) uint64_t mesh_hash() const noexcept { return mesh_hash_; }

    /// Get the size \f$ N \f$ of the mesh, i.e. the number of mesh points.
    [[nodiscard]] long size() const noexcept { return static_cast<long>(pts_.size()); }

    /// Get the vector of frequency point values.
    [[nodiscard]] C2PY_PROPERTY_GET(points) std::vector<double> const &points() const noexcept { return pts_; }

    /// Get an iterator to the beginning of the mesh.
    [[nodiscard]] auto begin() const { return mesh_iterator<refreq_pts>{.mesh_ptr = this, .data_index = 0}; }

    /// Get a const iterator to the beginning of the mesh.
    [[nodiscard]] auto cbegin() const { return begin(); }

    /// Get an iterator to the end of the mesh.
    [[nodiscard]] auto end() const { return mesh_iterator<refreq_pts>{.mesh_ptr = this, .data_index = size()}; }

    /// Get a const iterator to the end of the mesh.
    [[nodiscard]] auto cend() const { return end(); }

    /**
     * @brief Write a triqs::mesh::refreq_pts mesh to a `std::ostream`.
     *
     * @param sout `std::ostream` object.
     * @param m %Mesh to be written.
     * @return Reference to `std::ostream` object.
     */
    friend std::ostream &operator<<(std::ostream &sout, refreq_pts const &m) {
      return sout << fmt::format("Real frequency point mesh of size {}", m.size());
    }

    /**
     * @brief Serialize the mesh to a generic archive.
     * @param ar Archive to serialize to.
     */
    void serialize(auto &ar) const { ar & pts_ & mesh_hash_; }

    /**
     * @brief Deserialize the mesh from a generic archive.
     * @param ar Archive to deserialize from.
     */
    void deserialize(auto &ar) { ar & pts_ & mesh_hash_; }

    /// Get the HDF5 format tag.
    [[nodiscard]] static std::string hdf5_format() { return "MeshReFreqPts"; }

    /**
     * @brief Write a triqs::mesh::refreq_pts mesh to HDF5.
     *
     * @param g `h5::group` to be written to.
     * @param name Name of the subgroup.
     * @param m %Mesh object to be written.
     */
    friend void h5_write(h5::group g, std::string const &name, refreq_pts const &m) {
      h5::group gr = g.create_group(name);
      h5::write_hdf5_format(gr, m); // NOLINT (downcasting to base class)
      h5::write(gr, "points", m.pts_);
    }

    /**
     * @brief Read a triqs::mesh::refreq_pts mesh from HDF5.
     *
     * @param g `h5::group` to be read from.
     * @param name Name of the subgroup.
     * @param m %Mesh object to be read into.
     */
    friend void h5_read(h5::group g, std::string const &name, refreq_pts &m) {
      h5::group gr = g.open_group(name);
      h5::assert_hdf5_format(gr, m, true); // NOLINT (downcasting to base class)
      auto pts = h5::read<std::vector<double>>(gr, "points");
      m        = refreq_pts(std::move(pts));
    }

    private:
    std::vector<double> pts_;
    uint64_t mesh_hash_ = 0;
  };

  /**
   * @brief Linear interpolation of a function \f$ f \f$ defined on a triqs::mesh::refreq_pts mesh
   * at a real frequency \f$ \omega \f$.
   *
   * @details We find the bracketing mesh points using binary search and calculate
   * \f[
   *   f(\omega) \approx f_{i_l} \cdot w_l + f_{i_r} \cdot w_r
   * \f]
   * where \f$ i_l \f$ and \f$ i_r \f$ are the indices of the mesh points bracketing \f$ \omega \f$,
   * and \f$ w_l, w_r \f$ are the interpolation weights.
   *
   * @param m triqs::mesh::refreq_pts mesh.
   * @param f Callable object containing the function values at the mesh points.
   * @param w Real frequency at which to interpolate.
   * @return Linear interpolation of \f$ f(\omega) \f$.
   */
  inline auto evaluate(refreq_pts const &m, auto const &f, double w) {
    EXPECTS(m.is_value_valid(w));

    auto const &pts = m.points();
    auto itr_r      = std::ranges::lower_bound(pts, w);
    long i_r        = itr_r - pts.begin();

    if (i_r == 0) { return f(0); }
    if (i_r == m.size()) { return f(m.size() - 1); }

    long i_l = i_r - 1;

    double w_l = pts[i_l];
    double w_r = pts[i_r];
    double del = w_r - w_l;

    double wt_r = (w - w_l) / del;
    double wt_l = (w_r - w) / del;

    return f(i_l) * wt_l + f(i_r) * wt_r;
  }

  /** @} */

} // namespace triqs::mesh
