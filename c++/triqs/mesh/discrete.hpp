// Copyright (c) 2022-2023 Simons Foundation
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
// Authors: Olivier Parcollet, Nils Wentzell

/**
 * @file
 * @brief Provides a discrete mesh type.
 */

#pragma once

#include "./concepts.hpp"
#include "./mesh_iterator.hpp"

#include <h5/h5.hpp>
#include <itertools/itertools.hpp>
#include <nda/nda.hpp>

#include <cstdint>
#include <iostream>
#include <string>

namespace triqs::mesh {

  /**
   * @ingroup triqs-meshes-other
   * @brief Discrete mesh type.
   *
   * @details A discrete mesh satisfies the triqs::mesh::Mesh concept. It is defined by its size \f$ N \geq 0 \f$ and
   * has the following properties:
   *
   * - Each mesh point is identified by a unique index \f$ n \in \{0, 1, \ldots, N-1\} \f$.
   * - An index \f$ n \f$ is mapped to the corresponding data index \f$ d \f$ by the identity function \f$ d(n) = n \f$
   * and vice versa.
   * - There is no explicit value associated with a mesh point, i.e. it does not satisfy the
   * triqs::mesh::MeshWithValues concept.
   *
   * @code
   * #include <fmt/base.h>
   * #include <triqs/mesh.hpp>
   * 
   * int main() {
   *   // initialize a discrete mesh with 5 points
   *   triqs::mesh::discrete m{5};
   * 
   *   // loop over all mesh points and print their index and data index
   *   for (int i = 0; auto mp : m) fmt::println("mesh point #{}: index = {}, data index = {}", i++, mp.index(), mp.data_index());
   * }
   * @endcode
   *
   * Output:
   *
   * ```
   * mesh point #0: index = 0, data index = 0
   * mesh point #1: index = 1, data index = 1
   * mesh point #2: index = 2, data index = 2
   * mesh point #3: index = 3, data index = 3
   * mesh point #4: index = 4, data index = 4
   * ```
   */
  struct discrete {
    /// Index type.
    using index_t = long;

    /// Data index type.
    using data_index_t = long;

    private:
    long L_;
    uint64_t _mesh_hash = 0;

    public:
    /**
     * @brief Construct a discrete mesh of a given size \f$ N \geq 0 \f$.
     * @param N Size of the mesh.
     */
    discrete(long N = 0) : L_(N), _mesh_hash(N) { EXPECTS(N >= 0); }

    /// Equal-to comparison operator compares the size \f$ N \f$ of the meshes.
    bool operator==(discrete const &) const = default;

    /// Not-equal-to comparison operator compares the size \f$ N \f$ of the meshes.
    bool operator!=(discrete const &) const = default;

    /**
     * @brief %Mesh point of a triqs::mesh::discrete mesh.
     * @details It stores the index \f$ n \f$, the data index \f$ d \f$ and the hash value of the parent mesh.
     */
    struct mesh_point_t {
      /// Parent mesh type.
      using mesh_t = discrete;

      private:
      long _index         = 0;
      long _data_index    = 0;
      uint64_t _mesh_hash = 0;

      public:
      /// Default constructor leaves the mesh point uninitialized.
      mesh_point_t() = default;

      /**
       * @brief Construct a mesh point with a given index \f$ n \f$, data index \f$ d \f$ and hash value of the parent 
       * mesh.
       *
       * @param n Index \f$ n \f$ of the mesh point.
       * @param d Data index \f$ d \f$ of the mesh point.
       * @param mhash Hash value of the parent mesh.
       */
      mesh_point_t(long n, long d, uint64_t mhash) : _index(n), _data_index(d), _mesh_hash(mhash) {}

      /// Get the index \f$ n \f$ of the mesh point.
      [[nodiscard]] long index() const { return _index; }

      /// Get the data index \f$ d \f$ of the mesh point.
      [[nodiscard]] long data_index() const { return _data_index; }

      /// Get the hash value of the parent mesh.
      [[nodiscard]] uint64_t mesh_hash() const noexcept { return _mesh_hash; }
    };

    /**
     * @brief Check if an index \f$ n \f$ is valid.
     *
     * @param n Index \f$ n \f$ to check.
     * @return True if \f$ 0 \leq n < N \f$, false otherwise.
     */
    [[nodiscard]] bool is_index_valid(index_t n) const noexcept { return 0 <= n and n < L_; }

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
     * @brief Map a data index \f$ d \in \{0, 1, \ldots, N-1\} \f$ to the corresponding index \f$ n(d) \f$.
     *
     * @param d Data index \f$ d \f$ to map.
     * @return Index \f$ n(d) = d \f$.
     */
    [[nodiscard]] index_t to_index(long d) const noexcept {
      EXPECTS(is_index_valid(d));
      return d;
    }

    /**
     * @brief Subscript operator to access a mesh point by its data index \f$ d \in \{0, 1, \ldots, N-1\} \f$.
     *
     * @param d Data index \f$ d \f$ of the mesh point.
     * @return mesh_point_t with the index \f$ n(d) = d \f$, data index \f$ d \f$ and hash value of the current mesh.
     */
    [[nodiscard]] mesh_point_t operator[](long d) const { return {to_index(d), d, _mesh_hash}; }

    /**
     * @brief Function call operator to access a mesh point by its index \f$ n \in \{0, 1, \ldots, N-1\} \f$.
     *
     * @param n Index \f$ n \f$ of the mesh point.
     * @return mesh_point_t with the index \f$ n \f$, data index \f$ d(n) = n \f$ and hash value of the current mesh.
     */
    [[nodiscard]] mesh_point_t operator()(index_t n) const { return {n, to_data_index(n), _mesh_hash}; }

    /// Get the hash value of the mesh.
    [[nodiscard]] uint64_t mesh_hash() const { return _mesh_hash; }

    /// Get the size \f$ N \f$ of the mesh, i.e. the number of mesh points.
    [[nodiscard]] long size() const { return L_; }

    /// Get an iterator to the beginning of the mesh.
    [[nodiscard]] auto begin() const { return mesh_iterator<discrete>{.mesh_ptr = this, .data_index = 0}; }

    /// Get a const iterator to the beginning of the mesh.
    [[nodiscard]] auto cbegin() const { return begin(); }

    /// Get an iterator to the end of the mesh.
    [[nodiscard]] auto end() const { return mesh_iterator<discrete>{.mesh_ptr = this, .data_index = size()}; }

    /// Get a const iterator to the end of the mesh.
    [[nodiscard]] auto cend() const { return end(); }

    /**
     * @brief Write a triqs::mesh::discrete mesh to a `std::ostream`.
     *
     * @param sout `std::ostream` object.
     * @param m %Mesh to be written.
     * @return Reference to `std::ostream` object.
     */
    friend std::ostream &operator<<(std::ostream &sout, discrete const &m) { return sout << "Discrete mesh of size " << m.size(); }

    /**
     * @brief Serialize the mesh to a generic archive.
     * @param ar Archive to serialize to.
     */
    void serialize(auto &ar) const { ar & L_ & _mesh_hash; }

    /**
     * @brief Deserialize the mesh from a generic archive.
     * @param ar Archive to deserialize from.
     */
    void deserialize(auto &ar) { ar & L_ & _mesh_hash; }

    /// Get the HDF5 format tag.
    [[nodiscard]] static std::string hdf5_format() { return "MeshIndex"; }

    /**
     * @brief Write a triqs::mesh::discrete mesh to HDF5.
     *
     * @param g `h5::group` to be written to.
     * @param name Name of the subgroup.
     * @param m %Mesh object to be written.
     */
    friend void h5_write(h5::group g, std::string const &name, discrete const &m) {
      h5::group gr = g.create_group(name);
      h5::write_hdf5_format(gr, m); // NOLINT (downcasting to base class)
      h5::write(gr, "size", m.size());
    }

    /**
     * @brief Read a triqs::mesh::discrete mesh from HDF5.
     *
     * @param g `h5::group` to be read from.
     * @param name Name of the subgroup.
     * @param m %Mesh object to be read into.
     */
    friend void h5_read(h5::group g, std::string const &name, discrete &m) {
      h5::group gr = g.open_group(name);
      h5::assert_hdf5_format(gr, m, true);
      long L = h5::read<long>(gr, "size");
      m      = discrete(L);
    }
  };

  // Check mesh concepts.
  static_assert(Mesh<discrete>);
  static_assert(!MeshWithValues<discrete>);

} // namespace triqs::mesh
