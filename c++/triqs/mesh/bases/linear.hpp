// Copyright (c) 2013-2015 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2015 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2019-2023 Simons Foundation
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
// Authors: Philipp Dumitrescu, Michel Ferrero, Henri Menke, Olivier Parcollet, Nils Wentzell

/**
 * @file
 * @brief Provides a linear mesh type with equally spaced mesh points on an interval.
 */

#pragma once

#include "../utils.hpp"
#include "../mesh_iterator.hpp"

#include <h5/h5.hpp>
#include <itertools/itertools.hpp>
#include <nda/nda.hpp>

#include <algorithm>
#include <concepts>
#include <cstdint>
#include <string>
#include <type_traits>
#include <utility>

namespace triqs::mesh::details {

  /**
   * @ingroup triqs-meshes-other
   * @brief Linear mesh type.
   *
   * @details This class serves as a CRTP base class for other mesh types and satisfies the triqs::mesh::MeshWithValues
   * concept. It is defined by its size \f$ N \geq 0 \f$ and the interval \f$ [a, b] \f$ with \f$ a \leq b \f$. The mesh
   * points are equally spaced on the interval, i.e. the distance between two consecutive mesh points (step size) is
   * constant.
   *
   * A linear mesh has the following properties:
   *
   * - Each mesh point is identified by an unique index \f$ n \in \{0, 1, \ldots, N-1\} \f$.
   * - An index \f$ n \f$ is mapped to the corresponding data index \f$ d \f$ by the identity function \f$ d(n) = n \f$
   * and vice versa.
   * - An index \f$ n \f$ is mapped to the corresponding value \f$ m \f$ by the linear function \f$ m(n) = a + n \cdot
   * \Delta \f$ sucht that \f$ m(0) = a \f$ and \f$ m(N-1) = b \f$. The step size of the mesh is \f$ \Delta = \frac{b -
   * a}{N - 1} \f$ for \f$ N > 1 \f$, otherwise it is undefined. For implementation purposes, we set the step size and
   * its inverse to zero for \f$ N = 1 \f$.
   * - An arbitrary value \f$ x \in [a, b] \f$ is mapped to the closest mesh point with index \f$ n \f$ by the function
   * \f$ n(x) = \left\lfloor \frac{x - a}{\Delta} + 0.5 \right\rfloor \f$.
   *
   * See also triqs::mesh::imtime for an example of a derived classes.
   * 
   * @tparam M %Mesh type that inherits from this base class.
   * @tparam T Value type of the mesh points.
   */
  template <typename M, typename T>
    requires std::totally_ordered<T>
  class linear {
    public:
    /// Value type.
    using value_t = T;

    /// Index type.
    using index_t = long;

    /// Data index type.
    using data_index_t = long;

    /**
     * @brief %Mesh point of a triqs::mesh::details::linear mesh.
     * 
     * @details It stores the index \f$ n \f$, the data index \f$ d \f$, the hash value of the parent mesh and the
     * value \f$ m \f$ of the mesh point.
     * 
     * Arithmetic operations are defined for mesh points and scalars of the underlying value type. The operations are
     * performed between the value \f$ m \f$ of the mesh point and the given scalar.
     */
    class mesh_point_t {
      public:
      /// Parent mesh type.
      using mesh_t = M;

      /// Default constructor leaves the mesh point uninitialized.
      mesh_point_t() = default;

      /**
       * @brief Construct a mesh point with a given index \f$ n \f$, data index \f$ d \f$, hash value of the parent 
       * mesh and value \f$ m \f$.
       *
       * @param n Index \f$ n \f$ of the mesh point.
       * @param d Data index \f$ d \f$ of the mesh point.
       * @param mhash Hash value of the parent mesh.
       * @param m Value \f$ m \f$ of the mesh point.
       */
      mesh_point_t(long n, long d, uint64_t mhash, double m) : _index(n), _data_index(d), _mesh_hash(mhash), _value(m) {}

      /// Get the index \f$ n \f$ of the mesh point.
      [[nodiscard]] long index() const { return _index; }

      /// Get the data index \f$ d \f$ of the mesh point.
      [[nodiscard]] long data_index() const { return _data_index; }

      /// Get the value \f$ m \f$ of the mesh point.
      [[nodiscard]] value_t value() const { return _value; }

      /// Get the hash value of the parent mesh.
      [[nodiscard]] uint64_t mesh_hash() const noexcept { return _mesh_hash; }

      /// Conversion to the value type of the parent mesh.
      operator value_t() const { return _value; }

#define IMPL_OP(OP)                                                                                                                                  \
  /** @brief Binary `OP` operation for a linear::mesh_point_t and some type `U`. */                                                                  \
  template <typename U> friend auto operator OP(mesh_point_t const &mp, U &&y) { return mp.value() OP std::forward<U>(y); }                          \
  /** @brief Binary `OP` operation for some type `U` and a linear::mesh_point_t. */                                                                  \
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
      long _index         = 0;
      long _data_index    = 0;
      uint64_t _mesh_hash = 0;
      value_t _value      = {};
    };

    /**
     * @brief Construct a linear mesh on the interval \f$ [a, b] \f$ of a given size \f$ N \geq 0 \f$.
     *
     * @param a Lower bound \f$ a \f$ of the interval.
     * @param b Upper bound \f$ b \f$ of the interval.
     * @param N Size of the mesh.
     */
    linear(value_t a = 0, value_t b = 1, long N = 2)
       : L(N),
         xmin(a),
         xmax(b),
         delta_x(L == 1 ? 0. : (b - a) / (L - 1)),
         delta_x_inv{delta_x == 0.0 ? std::numeric_limits<double>::infinity() : 1. / delta_x},
         _mesh_hash(hash(L, xmin, xmax)) {
      EXPECTS(a <= b);
    }

    /// Equal-to comparison operator compares the size \f$ N \f$ of the meshes and the interval \f$ [a, b] \f$.
    bool operator==(linear const &) const = default;

    /// Not-equal-to comparison operator compares the size \f$ N \f$ of the meshes and the interval \f$ [a, b] \f$.
    bool operator!=(linear const &) const = default;

    /**
     * @brief Check if an index \f$ n \f$ is valid.
     *
     * @param n Index \f$ n \f$ to check.
     * @return True if \f$ 0 \leq n < N \f$, false otherwise.
     */
    [[nodiscard]] bool is_index_valid(index_t n) const noexcept { return 0 <= n and n < L; }

    private:
    // Check if a value is valid.
    [[nodiscard]] bool is_value_valid(value_t m) const noexcept { return xmin <= m and m <= xmax; }

    public:
    /**
     * @brief Map an index \f$ n \in \{0, 1, \ldots, N-1\} \f$ to its corresponding data index \f$ d(n) \f$.
     *
     * @param n Index \f$ n \f$ to map.
     * @return Data index \f$ d(n) = n \f$.
     */
    [[nodiscard]] data_index_t to_data_index(index_t n) const noexcept {
      EXPECTS(is_index_valid(n));
      return n;
    }

    /**
     * @brief Map a value \f$ x \in [a, b] \f$ to the closest mesh point and return its data index \f$ d(x) \f$.
     *
     * @param cmp triqs::mesh::closest_mesh_point_t containing the value \f$ x \f$ to map.
     * @return Data index \f$ d(x) = \left\lfloor \frac{x - a}{\Delta} + 0.5 \right\rfloor \f$.
     */
    [[nodiscard]] index_t to_data_index(closest_mesh_point_t<value_t> const &cmp) const noexcept {
      EXPECTS(is_value_valid(cmp.value));
      return to_data_index(to_index(cmp));
    }

    /**
     * @brief Map a data index \f$ d \in \{0, 1, \ldots, N-1\} \f$ to the corresponding index \f$ n(d) \f$.
     *
     * @param d Data index \f$ d \f$ to map.
     * @return Index \f$ n(d) = d \f$.
     */
    [[nodiscard]] index_t to_index(data_index_t d) const noexcept {
      EXPECTS(is_index_valid(d));
      return d;
    }

    /**
     * @brief Map a value \f$ x \in [a, b] \f$ to the closest mesh point and return its index \f$ n(x) \f$.
     *
     * @param cmp triqs::mesh::closest_mesh_point_t containing the value \f$ x \f$ to map.
     * @return Index \f$ n(x) = \left\lfloor \frac{x - a}{\Delta} + 0.5 \right\rfloor \f$.
     */
    [[nodiscard]] index_t to_index(closest_mesh_point_t<value_t> const &cmp) const noexcept {
      EXPECTS(is_value_valid(cmp.value));
      return static_cast<index_t>((cmp.value - xmin) * delta_x_inv + 0.5);
    }

    /**
     * @brief Subscript operator to access a mesh point by its data index \f$ d \in \{0, 1, \ldots, N-1\} \f$.
     *
     * @param d Data index \f$ d \f$ of the mesh point.
     * @return mesh_point_t with the index \f$ n(d) = d \f$, data index \f$ d \f$, hash value of the current mesh and
     * value \f$ m(d) = a + d \cdot \Delta \f$.
     */
    [[nodiscard]] mesh_point_t operator[](long d) const noexcept { return (*this)(d); }

    /**
     * @brief Subscript operator to access a mesh point by a value \f$ x \f$ contained in a
     * triqs::mesh::closest_mesh_point_t.
     *
     * @param cmp triqs::mesh::closest_mesh_point_t containing the value \f$ x \f$.
     * @return mesh_point_t with the index \f$ n(x) = \left\lfloor \frac{x - a}{\Delta} + 0.5 \right\rfloor \f$,
     * data index \f$ d(x) = n(x) \f$, hash value of the current mesh and value \f$ m(x) = a + n(x) \cdot \Delta \f$.
     */
    [[nodiscard]] mesh_point_t operator[](closest_mesh_point_t<value_t> const &cmp) const noexcept { return (*this)[this->to_data_index(cmp)]; }

    /**
     * @brief Function call operator to access a mesh point by its index \f$ n \in \{0, 1, \ldots, N-1\} \f$.
     *
     * @param n Index \f$ n \f$ of the mesh point.
     * @return mesh_point_t with the index \f$ n \f$, data index \f$ d(n) = n \f$, hash value of the current mesh and
     * value \f$ m(n) = a + n \cdot \Delta \f$.
     */
    [[nodiscard]] mesh_point_t operator()(index_t n) const noexcept {
      EXPECTS(is_index_valid(n));
      return {n, n, _mesh_hash, to_value(n)};
    }

    /**
     * @brief Map an index \f$ n \in \{0, 1, \ldots, N-1\} \f$ to its corresponding value \f$ m(n) \f$.
     *
     * @param n Index \f$ n \f$ to map.
     * @return Value of the mesh point \f$ m(n) = a + n \cdot \Delta \f$.
     */
    [[nodiscard]] value_t to_value(index_t n) const noexcept {
      EXPECTS(is_index_valid(n));
      if (L == 1) return xmin;
      double wr  = double(n) / (L - 1);
      double res = xmin * (1 - wr) + xmax * wr;
      return res;
    }

    /// Get the hash value of the mesh.
    [[nodiscard]] uint64_t mesh_hash() const noexcept { return _mesh_hash; }

    /// Get the size \f$ N \f$ of the mesh, i.e. the number of mesh points.
    [[nodiscard]] long size() const noexcept { return L; }

    /// Get the step size \f$ \Delta \f$ of the mesh, i.e. the distance between two consecutive mesh points.
    [[nodiscard]] value_t delta() const noexcept { return delta_x; }

    /// Get the inverse of the step size of the mesh, i.e. \f$ 1 / \Delta \f$.
    [[nodiscard]] value_t delta_inv() const noexcept { return delta_x_inv; }

    /// Get the first index of the mesh, i.e. \f$ 0 \f$.
    [[nodiscard]] long first_index() const { return 0; }

    /// Get the last index of the mesh, i.e. \f$ N - 1 \f$.
    [[nodiscard]] long last_index() const { return L - 1; }

    /// Get an iterator to the beginning of the mesh.
    [[nodiscard]] auto begin() const { return mesh_iterator<linear<M, T>>{.mesh_ptr = this, .data_index = 0}; }

    /// Get a const iterator to the beginning of the mesh.
    [[nodiscard]] auto cbegin() const { return begin(); }

    /// Get an iterator to the end of the mesh.
    [[nodiscard]] auto end() const { return mesh_iterator<linear<M, T>>{.mesh_ptr = this, .data_index = size()}; }

    /// Get a const iterator to the end of the mesh.
    [[nodiscard]] auto cend() const { return end(); }

    /**
     * @brief Serialize the mesh to a generic archive.
     * @param ar Archive to serialize to.
     */
    void serialize(auto &ar) const { ar & L & xmin & xmax & delta_x & delta_x_inv & _mesh_hash; }

    /**
     * @brief Deserialize the mesh from a generic archive.
     * @param ar Archive to deserialize from.
     */
    void deserialize(auto &ar) { ar & L & xmin & xmax & delta_x & delta_x_inv & _mesh_hash; }

    protected:
    /**
     * @brief Write the mesh to HDF5.
     *
     * @param g `h5::group` to be written to.
     * @param name Name of the subgroup.
     * @param format Format string to be written.
     */
    void h5_write_impl(h5::group g, std::string const &name, const char *format) const {
      h5::group gr = g.create_group(name);
      h5::write_hdf5_format_as_string(gr, format); // NOLINT (downcasting to base class)
      h5::write(gr, "min", this->xmin);
      h5::write(gr, "max", this->xmax);
      h5::write(gr, "size", this->size());
    }

    /**
     * @brief Read the mesh from HDF5.
     *
     * @param g `h5::group` to be read from.
     * @param name Name of the subgroup.
     * @param exp_format Expected format string.
     */
    void h5_read_impl(h5::group g, std::string const &name, const char *exp_format) {
      h5::group gr = g.open_group(name);
      h5::assert_hdf5_format_as_string(gr, exp_format, true);
      auto a  = h5::read<value_t>(gr, "min");
      auto b  = h5::read<value_t>(gr, "max");
      auto sz = h5::read<long>(gr, "size");
      *this   = linear(a, b, sz);
    }

    public:
    /**
     * @brief Linear interpolation of a function \f$ f \f$ defined on a triqs::mesh::details::linear mesh at a value \f$ 
     * x \in [a, b] \f$.
     *
     * @details We calculate
     * \f[
     *   f(x) \approx f_n * (1 - w) + f_{n + 1} * w \; ,
     * \f]
     * where \f$ n(x) = \mathrm{min} \left\{ \left\lfloor \frac{x - a}{\Delta} \right\rfloor, N - 2 \right\} \f$ is the
     * index of the left mesh point used for the interpolation, \f$ w = \frac{x - m(n)}{\Delta} \f$ and \f$ f_n \f$ is
     * the function value at the mesh point with index \f$ n \f$.
     *
     * @param f Callable object \f$ f \f$ containing the function values \f$ f_n \f$ at the mesh points.
     * @param x Value \f$ x \f$ at which to interpolate the function.
     * @return Linear interpolation of \f$ f(x) \f$.
     */
    auto evaluate(auto const &f, double x) const {
      EXPECTS(this->is_value_valid(x) and this->size() > 1);
      x        = std::max(x, this->xmin);
      double a = (x - this->xmin) * this->delta_inv();
      long i   = std::min(static_cast<long>(a), this->size() - 2);
      double w = std::min(a - i, 1.0); //NOLINT
      return (1 - w) * f(i) + w * f(i + 1);
    }

    protected:
    long L;
    value_t xmin, xmax, delta_x;
    double delta_x_inv;
    size_t _mesh_hash = 0;
  };

} // namespace triqs::mesh::details
