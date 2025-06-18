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
// Authors: Thomas Ayral, Philipp Dumitrescu, Michel Ferrero, Olivier Parcollet, Nils Wentzell

/**
 * @file
 * @brief Provides a mesh type for Bravais lattices with Born-von Karman periodic boundary conditions.
 */

#pragma once

#include "./mesh_iterator.hpp"
#include "./utils.hpp"
#include "../lattice/bravais_lattice.hpp"

#include <fmt/ranges.h>
#include <h5/h5.hpp>
#include <nda/nda.hpp>

#include <array>
#include <cstdint>
#include <iostream>
#include <string>

namespace triqs::mesh {

  /**
   * @ingroup triqs-meshes-utils
   * @brief Bring triqs::lattice::bravais_lattice into the triqs::mesh namespace.
   */
  using lattice::bravais_lattice;

  /**
   * @addtogroup triqs-meshes-lattice
   * @{
   */

  /**
   * @brief Cyclic lattice mesh type for Bravais lattices with Born-von Karman periodic boundary conditions.
   *
   * @details A cyclic lattice mesh satisfies the triqs::mesh::MeshWithValues concept and is defined by an underlying
   * triqs::lattice::bravais_lattice and the number of unit cells in each of the three dimensions, \f$ N_1 \f$, \f$ N_2
   * \f$ and \f$ N_3 \f$, which define the supercell and the periodic boundary conditions (PBC) \f$ 
   * f(\mathbf{R}^{\mathbf{n}}) = f(\mathbf{R}^{\mathbf{n}} + \mathbf{T}^{\mathbf{m}}) \f$. Here, \f$ 
   * \mathbf{T}^{\mathbf{m}} = \sum_{i=1}^3 \mathbf{a}_i N_i m_i = \tilde{\mathbf{A}} \mathbf{m} \f$ is a supercell 
   * translation vector, \f$ \mathbf{a}_i \f$ is a Bravais lattice basis vector in direction \f$ i \f$ and \f$ 
   * \mathbf{m} = (m_1, m_2, m_3) \f$ with \f$ m_i \in \mathbb{Z} \f$. 
   *
   * It has the following properties:
   *
   * - Each mesh point is identified by
   *   - an unique index in the supercell \f$ \mathbf{n} = (n_1, n_2, n_3) \f$, where \f$ 0 \leq n_i < N_i \f$, and
   *   - an infinite set of indices due to the periodic boundary conditions, i.e. \f$ \{ \tilde{\mathbf{n}} = \mathbf{n} 
   *   + \mathbf{N} \mathbf{m} : \mathbf{N} \mathbf{m} = (N_1 m_1, N_2 m_2, N_3 m_3) \in \mathbb{Z}^3 \} \f$.
   * - The size of the mesh is \f$ N = N_1 \, N_2 \, N_3 \f$, i.e. the total number of unit cells in the supercell.
   * - An index \f$ \mathbf{n} \f$ is mapped to the corresponding data index \f$ d \f$ by the function \f$ d(\mathbf{n})
   * = n_3 + N_3 (n_2 + N_2 n_1) = n_3 + n_2 N_3 + n_1 N_2 N_3 \f$. The inverse map is \f$ \mathbf{n}(d) = (\lfloor d /
   * s_1 \rfloor, \lfloor (d \mod s_1) / s_2 \rfloor, (d \mod s_1) \mod s_2 ) \f$, where \f$ s_1 = N_2 N_3 \f$ and \f$
   * s_2 = N_3 \f$.
   * - An index \f$ \mathbf{n} \f$ is mapped to the corresponding value (lattice point) \f$ \mathbf{R}(\mathbf{n}) 
   * \equiv \mathbf{R}^{\mathbf{n}} = \sum_{i=1}^3 \mathbf{a}_i n_i = \mathbf{A} \mathbf{n} \f$, where \f$ \mathbf{a}_i 
   * \f$ is the Bravais lattice basis vectors in direction \f$ i \f$. The inverse map is then \f$ \mathbf{n}(
   * \mathbf{R}^{\mathbf{n}}) = \mathbf{A}^{-1} \mathbf{R}^{\mathbf{n}} = \mathbf{n} \f$.
   * 
   * @ref triqs-gfs containers that are based on a cyclic lattice mesh store the function values at the discrete lattice
   * points \f$ \mathbf{R}^{\mathbf{n}} \f$, i.e. \f$ f_{\mathbf{n}} = f(\mathbf{R}^{\mathbf{n}}) \f$. Because of the 
   * PBC, the container only has to store values for indices with \f$ 0 \leq n_i < N_i \f$. To evaluate the function at 
   * an arbitrary lattice point, it is first mapped to the supercell using the PBC and then the corresponding function 
   * value is returned (see triqs::mesh::cyclat::evaluate for details).
   *
   * @code
   * #include <fmt/ranges.h>
   * #include <triqs/mesh.hpp>
   * 
   * int main() {
   *   // initialize a mesh on a cubic lattice with a = 0.5 and a 2x2x3 supercell
   *   triqs::mesh::cyclat m{triqs::mesh::bravais_lattice{nda::eye<double>(3) * 0.5}, {2, 2, 3}};
   * 
   *   // loop over all mesh points and print their index, data index and value (lattice vector)
   *   for (int i = 0; auto mp : m) {
   *     fmt::println("mesh point #{}: index = {}, data index = {}, value = {}", i++, mp.index(), mp.data_index(), nda::vector<double>{mp.value()});
   *   }
   * }
   * @endcode
   *
   * Output:
   *
   * ```
   * mesh point #0: index = [0, 0, 0], data index = 0, value = [0, 0, 0]
   * mesh point #1: index = [0, 0, 1], data index = 1, value = [0, 0, 0.5]
   * mesh point #2: index = [0, 0, 2], data index = 2, value = [0, 0, 1]
   * mesh point #3: index = [0, 1, 0], data index = 3, value = [0, 0.5, 0]
   * mesh point #4: index = [0, 1, 1], data index = 4, value = [0, 0.5, 0.5]
   * mesh point #5: index = [0, 1, 2], data index = 5, value = [0, 0.5, 1]
   * mesh point #6: index = [1, 0, 0], data index = 6, value = [0.5, 0, 0]
   * mesh point #7: index = [1, 0, 1], data index = 7, value = [0.5, 0, 0.5]
   * mesh point #8: index = [1, 0, 2], data index = 8, value = [0.5, 0, 1]
   * mesh point #9: index = [1, 1, 0], data index = 9, value = [0.5, 0.5, 0]
   * mesh point #10: index = [1, 1, 1], data index = 10, value = [0.5, 0.5, 0.5]
   * mesh point #11: index = [1, 1, 2], data index = 11, value = [0.5, 0.5, 1]
   * ```
   */
  class cyclat {
    public:
    /// Value type.
    using value_t = bravais_lattice::point_t;

    /// Index type.
    using index_t = std::array<long, 3>;

    /// Data index type.
    using data_index_t = long;

    /**
     * @brief %Mesh point of a triqs::mesh::cyclat mesh.
     * 
     * @details It inherits from triqs::lattice::bravais_lattice::point_t and in addition to the index \f$ \mathbf{n} 
     * \f$, the underlying Bravais lattice and the value \f$ \mathbf{R}^{\mathbf{n}} \f$, it also stores the data index 
     * \f$ d \f$ and the hash value of the parent mesh.
     */
    class mesh_point_t : public value_t {
      public:
      /// Parent mesh type.
      using mesh_t = cyclat;

      /// Default constructor leaves the mesh point uninitialized.
      mesh_point_t() = default;

      /**
       * @brief Construct a mesh point with a given index \f$ \mathbf{n} \f$, data index \f$ d \f$, hash value of the 
       * parent mesh and Bravais lattice to which the mesh point belongs.
       *
       * @param n Index \f$\mathbf{n} \f$ of the mesh point.
       * @param d Data index \f$ d \f$ of the mesh point.
       * @param mhash Hash value of the parent mesh.
       * @param bl_ptr Pointer to a triqs::lattice::bravais_lattice object.
       */
      mesh_point_t(std::array<long, 3> const &n, long d, uint64_t mhash, bravais_lattice const *bl_ptr)
         : value_t(n, bl_ptr), data_index_(d), mesh_hash_(mhash) {}

      /// Get the data index \f$ d \f$ of the mesh point.
      [[nodiscard]] long data_index() const { return data_index_; }

      /// Get the lattice point \f$ \mathbf{R}^{\mathbf{n}} \f$ of the mesh point.
      [[nodiscard]] value_t const &value() const { return *this; }

      /// Get the hash value of the parent mesh.
      [[nodiscard]] uint64_t mesh_hash() const noexcept { return mesh_hash_; }

      /**
       * @brief Write triqs::mesh::cyclat::mesh_point_t to a `std::ostream`.
       *
       * @param sout `std::ostream` object.
       * @param mp %Mesh point to be written.
       * @return Reference to `std::ostream` object.
       */
      friend std::ostream &operator<<(std::ostream &sout, mesh_point_t const &mp) { return sout << mp.value(); }

      private:
      long data_index_    = 0;
      uint64_t mesh_hash_ = 0;
    };

    public:
    /**
     * @brief Construct a cyclic lattice mesh on a Bravais lattice with the given supercell dimensions.
     *
     * @param bl triqs::lattice::bravais_lattice object representing the underlying Bravais lattice.
     * @param dims Number of unit cells in the supercell along each of the three dimensions, i.e. \f$ (N_1, N_2, N_3) 
     * \f$.
     */
    cyclat(bravais_lattice const &bl, std::array<long, 3> const &dims)
       : bl_(bl),
         dims_(dims),
         size_(nda::stdutil::product(dims)),
         s2_(dims_[2]),
         s1_(dims_[1] * dims_[2]),
         units_(bl.units()),
         units_inv_(nda::linalg::inv(units_)),
         mesh_hash_(hash(nda::sum(bl.units()), dims[0], dims[1], dims[2])) {}

    /**
     * @brief Construct a cyclic lattice mesh on a Bravais lattice with the given periodization matrix.
     * 
     * @note At the moment, only diagonal periodization matrices are supported. They should contain the number of unit 
     * cells along each of the three dimensions on the diagonal.
     *
     * @param bl triqs::lattice::bravais_lattice object representing the underlying Bravais lattice.
     * @param M \f$ 3 \times 3 \f$ periodization matrix.
     */
    cyclat(bravais_lattice const &bl, nda::matrix<long> const &M) : cyclat(bl, std::array{M(0, 0), M(1, 1), M(2, 2)}) {
      EXPECTS((M.shape() == std::array{3l, 3l}));
      EXPECTS(nda::is_matrix_diagonal(M));
    }

    /**
     * @brief Construct a cyclic lattice mesh on a Bravais lattice with a cubic supercell.
     *
     * @param bl triqs::lattice::bravais_lattice object representing the underlying Bravais lattice.
     * @param n Number of unit cells in the supercell along each of the three dimensions.
     */
    cyclat(bravais_lattice const &bl, long n) : cyclat{bl, std::array{n, (bl.ndim() >= 2 ? n : 1l), (bl.ndim() >= 3 ? n : 1)}} {}

    /**
     * @brief Construct a cyclic lattice mesh on a cubic Bravais lattice with \f$ a = 1 \f$ and the given supercell 
     * dimensions.
     *
     * @param n1 Number of unit cells in the supercell along the first dimension.
     * @param n2 Number of unit cells in the supercell along the second dimension.
     * @param n3 Number of unit cells in the supercell along the third dimension.
     */
    cyclat(long n1 = 1, long n2 = 1, long n3 = 1) : cyclat{bravais_lattice{nda::eye<double>(3)}, std::array{n1, n2, n3}} {}

    /// Equal-to comparison operator compares the hash values.
    bool operator==(cyclat const &m) const { return mesh_hash() == m.mesh_hash(); }

    /// Not-equal-to comparison operator compares the hash values.
    bool operator!=(cyclat const &m) const { return !(operator==(m)); }

    /**
     * @brief Check if an index \f$ \mathbf{n} \f$ is valid, i.e. corresponds to a unit cell/lattice point in the
     * supercell.
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
      return n[0] * s1_ + n[1] * s2_ + n[2];
    }

    /**
     * @brief Map a lattice point \f$ \mathbf{R}^{\mathbf{n}} \f$ to its data index \f$ d(\mathbf{R}^{\mathbf{n}}) \f$.
     *
     * @param cmp triqs::mesh::closest_mesh_point_t containing the lattice point \f$ \mathbf{R}^{\mathbf{n}} \f$.
     * @return Data index \f$ d(\mathbf{R}^{\mathbf{n}}) = n_3 + N_3 (n_2 + N_2 n_1) \f$.
     */
    [[nodiscard]] data_index_t to_data_index(closest_mesh_point_t<value_t> const &cmp) const { return to_data_index(to_index(cmp)); }

    /**
     * @brief Map a data index \f$ d \in \{0, 1, \ldots, N-1\} \f$ to the corresponding index \f$ \mathbf{n}(d)
     * \f$.
     *
     * @param d Data index \f$ d \f$ to map.
     * @return Index \f$ \mathbf{n}(d) = (\lfloor d / s_1 \rfloor, \lfloor (d \mod s_1) / s_2 \rfloor, (d \mod s_1) 
     * \mod s_2 ) \f$.
     */
    [[nodiscard]] index_t to_index(data_index_t d) const {
      EXPECTS(0 <= d and d < size());
      long const r0 = d % s1_;
      return {d / s1_, r0 / s2_, r0 % s2_};
    }

    /**
     * @brief Map a lattice point \f$ \mathbf{R}^{\mathbf{n}} \f$ to its index \f$ \mathbf{n} \f$.
     *
     * @param cmp triqs::mesh::closest_mesh_point_t containing the lattice point \f$ \mathbf{R}^{\mathbf{n}} \f$.
     * @return Index \f$ \mathbf{n} \f$.
     */
    [[nodiscard]] index_t to_index(closest_mesh_point_t<value_t> const &cmp) const { return cmp.value.index(); }

    /**
     * @brief Subscript operator to access a mesh point by its data index \f$ d \in \{0, 1, \ldots, N-1\} \f$.
     *
     * @param d Data index \f$ d \f$ of the mesh point.
     * @return mesh_point_t with the index \f$ \mathbf{n}(d) = (\lfloor d / s_1 \rfloor, \lfloor (d \mod s_1) / s_2 
     * \rfloor, (d \mod s_1) \mod s_2 ) \f$, data index \f$ d \f$ and the hash value and underlying Bravais lattice of 
     * the current mesh.
     */
    [[nodiscard]] mesh_point_t operator[](long d) const { return {to_index(d), d, mesh_hash_, &bl_}; }

    /**
     * @brief Subscript operator to access a mesh point by a lattice point \f$ \mathbf{R}^{\mathbf{n}} \f$
     * contained in a triqs::mesh::closest_mesh_point_t.
     *
     * @param cmp triqs::mesh::closest_mesh_point_t containing the lattice point.
     * @return mesh_point_t with the index \f$ \mathbf{n} \f$, data index \f$ d(\mathbf{n}) = d(\mathbf{n}) = n_3 + N_3 
     * (n_2 + N_2 n_1) \f$ and the hash value and underlying Bravais lattice of the current mesh.
     */
    [[nodiscard]] mesh_point_t operator[](closest_mesh_point_t<value_t> const &cmp) const { return (*this)[this->to_data_index(cmp)]; }

    /**
     * @brief Function call operator to access a mesh point by its index \f$ \mathbf{n} \f$.
     *
     * @param n Index \f$ \mathbf{n} \f$ of the mesh point.
     * @return mesh_point_t with the index \f$ \mathbf{n} \f$, data index \f$ d(\mathbf{n}) = d(\mathbf{n}) = n_3 + N_3 
     * (n_2 + N_2 n_1) \f$ and the hash value and underlying Bravais lattice of the current mesh.
     */
    [[nodiscard]] mesh_point_t operator()(index_t const &n) const { return {n, to_data_index(n), mesh_hash_, &bl_}; }

    /**
     * @brief Map an index \f$ \mathbf{n} \f$ to its corresponding lattice point \f$ \mathbf{R}^{\mathbf{n}} \f$.
     *
     * @param n Index \f$ \mathbf{n} \f$ to map.
     * @return triqs::lattice::bravais_lattice::point_t \f$ \mathbf{R}^{\mathbf{n}} \f$.
     */
    [[nodiscard]] value_t to_value(index_t const &n) const {
      EXPECTS(is_index_valid(n));
      return {n, &bl_};
    }

    /// Get the number of unit cells in each of the three dimensions.
    [[nodiscard]] auto const &dims() const { return dims_; }

    /**
     * @brief Get the matrix \f$ \mathbf{A}^T \f$ containing the basis vectors of the Bravais lattice in its rows (see 
     * triqs::lattice::bravais_lattice::units()).
     */
    [[nodiscard]] auto units() const { return nda::matrix_const_view<double>{units_}; }

    /// Get the underlying Bravais lattice.
    [[nodiscard]] auto const &lattice() const noexcept { return bl_; }

    /// Get the hash value of the mesh.
    [[nodiscard]] uint64_t mesh_hash() const { return mesh_hash_; }

    /// Get the size \f$ N \f$ of the mesh, i.e. the number of unit cells in the supercell.
    [[nodiscard]] long size() const { return size_; }

    /**
     * @brief Map an arbitrary index \f$ \tilde{\mathbf{n}} \f$ to the unique index \f$ \mathbf{n} \f$ in the supercell.
     *
     * @param n_tilde Index \f$ \tilde{\mathbf{n}} \f$ to map back to the supercell.
     * @return Corresponding index \f$ \mathbf{n} \f$ in the supercell such that \f$ \tilde{\mathbf{n}} = \mathbf{n} +
     * \mathbf{N} \mathbf{m} \f$.
     */
    [[nodiscard]] index_t index_modulo(index_t const &n_tilde) const {
      return {positive_modulo(n_tilde[0], dims_[0]), positive_modulo(n_tilde[1], dims_[1]), positive_modulo(n_tilde[2], dims_[2])};
    }

    /// Get an iterator to the beginning of the mesh.
    [[nodiscard]] auto begin() const { return mesh_iterator<cyclat>{.mesh_ptr = this, .data_index = 0}; }

    /// Get a const iterator to the beginning of the mesh.
    [[nodiscard]] auto cbegin() const { return begin(); }

    /// Get an iterator to the end of the mesh.
    [[nodiscard]] auto end() const { return mesh_iterator<cyclat>{.mesh_ptr = this, .data_index = size()}; }

    /// Get a const iterator to the end of the mesh.
    [[nodiscard]] auto cend() const { return end(); }

    /**
     * @brief Write a triqs::mesh::cyclat mesh to a `std::ostream`.
     *
     * @param sout `std::ostream` object.
     * @param m %Mesh to be written.
     * @return Reference to `std::ostream` object.
     */
    friend std::ostream &operator<<(std::ostream &sout, cyclat const &m) {
      return sout << "Cyclic lattice mesh with linear dimensions " << m.dims() << "\n -- units = " << m.units() << "\n -- lattice: " << m.lattice();
    }

    /**
     * @brief Serialize the mesh to a generic archive.
     * @param ar Archive to serialize to.
     */
    void serialize(auto &ar) const { ar & bl_ & dims_ & size_ & s2_ & s1_ & units_ & units_inv_ & mesh_hash_; }

    /**
     * @brief Deserialize the mesh from a generic archive.
     * @param ar Archive to deserialize from.
     */
    void deserialize(auto &ar) { ar & bl_ & dims_ & size_ & s2_ & s1_ & units_ & units_inv_ & mesh_hash_; }

    /// Get the HDF5 format tag.
    [[nodiscard]] static std::string hdf5_format() { return "MeshCyclicLattice"; }

    /**
     * @brief Write a triqs::mesh::cyclat mesh to HDF5.
     *
     * @param g `h5::group` to be written to.
     * @param name Name of the subgroup.
     * @param m %Mesh object to be written.
     */
    friend void h5_write(h5::group g, std::string const &name, cyclat const &m) {
      h5::group gr = g.create_group(name);
      h5::write_hdf5_format(gr, m); // NOLINT (downcasting to base class)
      h5::write(gr, "dims", m.dims_);
      h5::write(gr, "bravais_lattice", m.bl_);
    }

    /**
     * @brief Read a triqs::mesh::cyclat mesh from HDF5.
     *
     * @param g `h5::group` to be read from.
     * @param name Name of the subgroup.
     * @param m %Mesh object to be read into.
     */
    friend void h5_read(h5::group g, std::string const &name, cyclat &m) {
      h5::group gr = g.open_group(name);
      h5::assert_hdf5_format(gr, m, true);

      std::array<long, 3> dims{};
      if (gr.has_key("dims")) {
        h5::read(gr, "dims", dims);
      } else {
        // for backward compatibility
        auto M = h5::read<nda::matrix<long>>(gr, "periodization_matrix");
        dims   = {M(0, 0), M(1, 1), M(2, 2)};
      }

      bravais_lattice bl{};
      if (gr.has_key("bravais_lattice")) {
        h5::read(gr, "bravais_lattice", bl);
      } else {
        // for backward compatibility
        h5::read(gr, "bl", bl);
      }

      m = cyclat(bl, dims);
    }

    private:
    bravais_lattice bl_            = {};
    std::array<long, 3> dims_      = {0, 0, 0};
    long size_                     = 0;
    long s2_                       = 1;
    long s1_                       = 1;
    nda::matrix<double> units_     = nda::eye<double>(3);
    nda::matrix<double> units_inv_ = nda::eye<double>(3);
    uint64_t mesh_hash_            = 0;
  };

  /**
   * @brief Evaluate a function \f$ f \f$ defined on a triqs::mesh::cyclat mesh at the given index \f$ 
   * \tilde{\mathbf{n}} \f$.
   * 
   * @details The index is first mapped to the supercell using triqs::mesh::cyclat::index_modulo and then it is used
   * to access the correct function value \f$ f_{\tilde{\mathbf{n}}} = f_{\mathbf{n}} \f$.
   *
   * @param m triqs::mesh::cyclat mesh.
   * @param f Callable object \f$ f \f$ containing the function values \f$ f_{\mathbf{n}} = f(\mathbf{R}^{\mathbf{n}}) 
   * \f$ at the mesh points.
   * @param n_tilde Index \f$ \tilde{\mathbf{n}} \f$ at which to evaluate the function.
   * @return Function value \f$ f_{\mathbf{n}} \f$.
   */
  auto evaluate(cyclat const &m, auto const &f, cyclat::index_t const &n_tilde) { return f(m.index_modulo(n_tilde)); }

  /**
   * @brief Evaluate a function \f$ f \f$ defined on a triqs::mesh::cyclat mesh at the given lattice point \f$
   * \mathbf{R}^{\tilde{\mathbf{n}}} \f$.
   * 
   * @details The index of the lattice point is first mapped to the supercell using triqs::mesh::cyclat::index_modulo 
   * and then it is used to access the correct function value \f$ f_{\tilde{\mathbf{n}}} = f_{\mathbf{n}} \f$.
   *
   * @param m triqs::mesh::cyclat mesh.
   * @param f Callable object \f$ f \f$ containing the function values \f$ f_{\mathbf{n}} = f(\mathbf{R}^{\mathbf{n}}) 
   * \f$ at the mesh points.
   * @param r_n_tilde Lattice point \f$ \mathbf{R}^{\tilde{\mathbf{n}}} \f$ at which to evaluate the function.
   * @return Function value \f$ f_{\mathbf{n}} \f$.
   */
  auto evaluate(cyclat const &m, auto const &f, cyclat::value_t const &r_n_tilde) { return evaluate(m, f, r_n_tilde.index()); }

  /** @} */

  // Check mesh concepts.
  static_assert(Mesh<cyclat>);
  static_assert(MeshWithValues<cyclat>);

} // namespace triqs::mesh
