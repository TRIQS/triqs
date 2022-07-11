// Copyright (c) 2014-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2014-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2019 Simons Foundation
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
// Authors: Michel Ferrero, Olivier Parcollet, Nils Wentzell, tayral

#pragma once
#include "./bravais_lattice.hpp"

namespace triqs {
  namespace lattice {

    using k_t = nda::vector<double>;

    class brillouin_zone {

      public:
      using value_t = k_t; // domain concept

      // Default construction, 3d cubic lattice
      brillouin_zone() {
        K_reciprocal     = nda::eye<double>(3);
        K_reciprocal_inv = K_reciprocal;
      }

      brillouin_zone( brillouin_zone &&) = default;
      brillouin_zone( brillouin_zone const&) = default;
      brillouin_zone& operator =( brillouin_zone const&) = default;
      brillouin_zone& operator =( brillouin_zone &&) = default;

      /**
       * Construct a Brillouin Zone from a Bravais Lattice
       *
       * @param bl The Bravais Lattice
       */
      explicit brillouin_zone(bravais_lattice const &bl);

      /// Check if a k_t is part of the domain
      [[nodiscard]] bool contains(k_t const &) const { return true; }

      /// Access to the underlying bravais lattice
      bravais_lattice const & lattice() const { return lattice_; }

      /// Allow cast to bravais lattice
      explicit operator bravais_lattice() const { return lattice_; }

      /// Number of dimensions
      int ndim() const { return lattice_.ndim(); }

      /// Matrix containing reciprocal basis vectors as rows
      matrix_t const & units() const { return K_reciprocal; }

      /// Matrix containing reciprocal basis vectors as rows
      matrix_t const & reciprocal_matrix() const { return K_reciprocal; }

      /// Inverse of the reciprocal matrix
      matrix_t const & reciprocal_matrix_inv() const { return K_reciprocal_inv; }

      /// Transform from lattice to real coordinates
      template <typename K> k_t lattice_to_real_coordinates(K const &k) const {return transpose(K_reciprocal)(range::all, range(ndim())) * nda::basic_array_view{k}; }

      /// Transform from real to lattice coordinates
      template <typename K> k_t real_to_lattice_coordinates(K const &k) const { return transpose(K_reciprocal_inv)(range::all, range(ndim())) * nda::basic_array_view{k}; }

      // ------------------- Comparison -------------------

      bool operator==(brillouin_zone const &bz) const {
        return reciprocal_matrix() == bz.reciprocal_matrix() && lattice() == bz.lattice();
      }

      bool operator!=(brillouin_zone const &bz) const { return !(operator==(bz)); }

      // -------------------- print -------------------

      friend std::ostream &operator<<(std::ostream &sout, brillouin_zone const &bz) {
        return sout << "Brillouin Zone with " << bz.ndim() << " dimensions and reciprocal matrix " << bz.reciprocal_matrix();
      }

      // -------------------- hdf5 -------------------

      [[nodiscard]] static std::string hdf5_format() { return "brillouin_zone"; }

      /// Write into HDF5
      friend void h5_write(h5::group fg, std::string subgroup_name, brillouin_zone const &bz);

      /// Read from HDF5
      friend void h5_read(h5::group fg, std::string subgroup_name, brillouin_zone &bz);

      // ---------------------------------------

      private:
      bravais_lattice lattice_;
      matrix_t K_reciprocal = matrix_t::zeros(3,3);
      matrix_t K_reciprocal_inv = matrix_t::zeros(3,3);
    };
  } // namespace lattice
} // namespace triqs
