// Copyright (c) 2014-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2014-2018 Centre national de la recherche scientifique (CNRS)
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
// Authors: Thomas Ayral, Michel Ferrero, Olivier Parcollet, Nils Wentzell

/**
 * @file
 * @brief Provides a Brillouin zone class.
 */

#pragma once

#include "./bravais_lattice.hpp"

#include <fmt/ranges.h>
#include <h5/h5.hpp>
#include <nda/nda.hpp>

#include <iostream>
#include <ranges>
#include <string>

namespace triqs::lattice {

  /**
   * @addtogroup triqs-lattice
   * @{
   */

  /// Reciprocal space vector type.
  using k_t = nda::vector<double>;

  /**
   * @brief A Brillouin zone class.
   *
   * @details A Brillouin zone is the primitive unit cell in reciprocal space. It is defined by the set of reciprocal
   * lattice basis vectors \f$ \{ \mathbf{b}_1, \dots, \mathbf{b}_d \} \f$, which satisfy
   * \f[
   *   \mathbf{b}_i \cdot \mathbf{a}_j = 2 \pi \delta_{ij} \; ,
   * \f]
   * where \f$ \{ \mathbf{a}_1, \dots, \mathbf{a}_d \} \f$ are the basis vectors of the Bravais lattice in real space.
   *
   * In matrix notation, we can write this as a system of linear equations \f$ A^T B = 2 \pi I \f$, where \f$ A = \big(
   * \mathbf{a}_1 \cdots \mathbf{a}_d \big) \f$ and \f$ B = \big( \mathbf{b}_1 \cdots \mathbf{b}_d \big) \f$ are the
   * matrices containing the basis vectors as their columns and \f$ I \f$ is the identity matrix.
   */
  class brillouin_zone {
    public:
    /// Value type of a Brillouin Zone.
    using value_t = k_t;

    /**
     * @brief Construct a Brillouin zone for a simple cubic lattice with lattice constant \f$ a = 1 \f$.
     * @details The reciprocal basis vectors defining the BZ are given by \f$ B = 2 \pi I \f$, where \f$ I \f$ is the
     * \f$ 3 \times 3 \f$ identity matrix.
     */
    brillouin_zone() : brillouin_zone{bravais_lattice{}} {}

    /**
     * @brief Construct a Brillouin zone for a given Bravais lattice.
     * 
     * @details The reciprocal basis vectors defining the BZ are given by \f$ B = 2 \pi \left( A^T \right)^{-1} \f$,
     * where \f$ A \f$ is the matrix containing the basis vectors of the given Bravais lattice as its columns.
     *
     * @param bl triqs::lattice::bravais_lattice object.
     */
    explicit brillouin_zone(bravais_lattice bl);

    /// Check if a given vector \f$ \mathbf{k} \f$ is part of the domain.
    [[nodiscard]] bool contains(k_t const &) const { return true; }

    /// Get the underlying triqs::lattice::bravais_lattice object.
    [[nodiscard]] auto const &lattice() const { return lattice_; }

    /// Conversion to the underlying triqs::lattice::bravais_lattice object.
    explicit operator bravais_lattice() const { return lattice_; }

    /// Get the number of dimensions of the underlying Bravais lattice.
    [[nodiscard]] auto ndim() const { return lattice_.ndim(); }

    /// Get the matrix \f$ B^T \f$ containing the reciprocal basis vectors as its rows.
    [[nodiscard]] matrix_t const &units() const { return K_reciprocal; }

    /// Get the matrix \f$ B^T \f$ containing the reciprocal basis vectors as its rows.
    [[nodiscard]] matrix_t const &reciprocal_matrix() const { return K_reciprocal; }

    /// Get the inverse matrix \f$ \left( B^T \right)^{-1} \f$.
    [[nodiscard]] matrix_t const &reciprocal_matrix_inv() const { return K_reciprocal_inv; }

    /**
     * @brief Transform a vector \f$ \mathbf{v} \f$ from the reciprocal lattice basis \f$ \{ \mathbf{b}_1, \dots,
     * \mathbf{b}_d \} \f$ to the standard basis \f$ \{ \mathbf{e}_1, \dots, \mathbf{e}_d \} \f$.
     *
     * @details The transformed vector is given by
     * \f[
     *   \tilde{\mathbf{v}} = \sum_{i=1}^{d} \mathbf{b}_i v_i = B \mathbf{v} \; ,
     * \f]
     * where \f$ B \f$ is the matrix with the reciprocal lattice basis vectors as its columns.
     *
     * @param v Vector \f$ \mathbf{v} \f$ to be transformed.
     * @return Vector \f$ \tilde{\mathbf{v}} \f$ in the standard basis.
     */
    template <typename K> [[nodiscard]] k_t lattice_to_real_coordinates(K const &v) const {
      return nda::transpose(K_reciprocal)(nda::range::all, nda::range(ndim())) * nda::basic_array_view{v}(nda::range(ndim()));
    }

    /**
     * @brief Transform a vector \f$ \tilde{\mathbf{v}} \f$ from the standard basis \f$ \{ \mathbf{e}_1, \dots,
     * \mathbf{e}_d \} \f$ to the reciprocal lattice basis \f$ \{ \mathbf{b}_1, \dots, \mathbf{b}_d \} \f$.
     *
     * @details This is the inverse transformation of lattice_to_real_coordinates(). The transformed vector is therefore
     * given by \f$ \mathbf{v} = B^{-1} \tilde{\mathbf{v}} \f$.
     *
     * @param v_tilde Vector \f$ \tilde{\mathbf{v}} \f$ in the standard basis.
     * @return Vector \f$ \mathbf{v} \f$ in the lattice basis.
     */
    template <typename K> [[nodiscard]] k_t real_to_lattice_coordinates(K const &v_tilde) const {
      return nda::transpose(K_reciprocal_inv)(nda::range::all, nda::range(ndim())) * nda::basic_array_view{v_tilde}(nda::range(ndim()));
    }

    /**
     * @brief Equal-to comparison operator.
     * @return True, if the underlying Bravais lattice and the reciprocal basis vectors are equal.
     */
    bool operator==(brillouin_zone const &bz) const { return reciprocal_matrix() == bz.reciprocal_matrix() && lattice() == bz.lattice(); }

    /**
     * @brief Not-equal-to comparison operator.
     * @return True, if the underlying Bravais lattice or the reciprocal basis vectors are not equal.
     */
    bool operator!=(brillouin_zone const &bz) const { return !(operator==(bz)); }

    /**
     * @brief Write a triqs::lattice::brillouin_zone to a `std::ostream`.
     *
     * @param sout `std::ostream` object.
     * @param bz Brillouin zone to be written.
     * @return Reference to `std::ostream` object.
     */
    friend std::ostream &operator<<(std::ostream &sout, brillouin_zone const &bz) {
      auto str = fmt::format("Brillouin zone in {} dimensions:\n", bz.ndim());
      str += fmt::format("  Basis vectors: {}\n",
                         std::views::transform(nda::range(bz.ndim()), [&bz](int i) { return bz.units()(i, nda::range(bz.ndim())); }));
      return sout << str;
    }

    /**
     * @brief Serialize the Brillouin zone to a generic archive.
     * @param ar Archive to serialize to.
     */
    void serialize(auto &ar) const { ar & lattice_ & K_reciprocal & K_reciprocal_inv; }

    /**
     * @brief Deserialize the Brillouin from a generic archive.
     * @param ar Archive to deserialize from.
     */
    void deserialize(auto &ar) { ar & lattice_ & K_reciprocal & K_reciprocal_inv; }

    /// Get the HDF5 format tag.
    [[nodiscard]] static std::string hdf5_format() { return "brillouin_zone"; }

    /**
     * @brief Write a triqs::lattice::brillouin_zone to HDF5.
     *
     * @param g `h5::group` to be written to.
     * @param name Name of the subgroup.
     * @param bz Brillouin zone object to be written.
     */
    friend void h5_write(h5::group g, std::string const &name, brillouin_zone const &bz);

    /**
     * @brief Read a triqs::lattice::brillouin_zone from HDF5.
     *
     * @param g `h5::group` to be read from.
     * @param name Name of the subgroup.
     * @param bz Brillouin zone object to be read into.
     */
    friend void h5_read(h5::group g, std::string const &name, brillouin_zone &bz);

    private:
    bravais_lattice lattice_;
    matrix_t K_reciprocal     = matrix_t::zeros(3, 3);
    matrix_t K_reciprocal_inv = matrix_t::zeros(3, 3);
  };

  /** @} */

} // namespace triqs::lattice
