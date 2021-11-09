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

    class brillouin_zone {

      public:
      using point_t = k_t; // domain concept

      brillouin_zone() { // default construction, 3d cubic lattice
        K_reciprocal     = nda::eye<double>(3);
        K_reciprocal_inv = K_reciprocal;
      }

      brillouin_zone( brillouin_zone &&) = default;
      brillouin_zone( brillouin_zone const&) = default;
      brillouin_zone& operator =( brillouin_zone const&) = default;
      brillouin_zone& operator =( brillouin_zone &&) = default;

      /// Construct from a bravais_lattice
      brillouin_zone(bravais_lattice const &bl_);

      /// Access to the underlying bravais lattice
      bravais_lattice const & lattice() const { return lattice_; }

      /// Allow cast to bravais lattice
      operator bravais_lattice() const { return lattice_; }

      /// Matrix containing reciprocal basis vectors as rows
      nda::matrix_const_view<double> units() const { return K_reciprocal; }

      /// Number of dimensions
      int ndim() const { return lattice_.ndim(); }

      /// Matrix containing reciprocal basis vectors as rows
      nda::matrix_const_view<double> reciprocal_matrix() const { return K_reciprocal; }

      /// Inverse of the reciprocal matrix
      nda::matrix_const_view<double> reciprocal_matrix_inv() const { return K_reciprocal_inv; }

      /// Transform from lattice to real coordinates
      template <typename K> k_t lattice_to_real_coordinates(K const &k) const { return _transfo_impl(k, K_reciprocal); }

      /// Transform from real to lattice coordinates
      template <typename K> k_t real_to_lattice_coordinates(K const &k) const { return _transfo_impl(k, K_reciprocal_inv); }

      // ------------------- Comparison -------------------

      bool operator==(brillouin_zone const &bz) const {
        return reciprocal_matrix() == bz.reciprocal_matrix() && lattice() == bz.lattice();
      }

      bool operator!=(brillouin_zone const &bz) const { return !(operator==(bz)); }

      // -------------------- print -------------------

      friend std::ostream &operator<<(std::ostream &sout, brillouin_zone const &bz) {
        return sout << "Brillouin Zone with " << bz.ndim() << " dimensions and reciprocal matrix " << bz.reciprocal_matrix();
      }

      /// Write into HDF5
      friend void h5_write(h5::group fg, std::string subgroup_name, brillouin_zone const &bz);

      /// Read from HDF5
      friend void h5_read(h5::group fg, std::string subgroup_name, brillouin_zone &bz);

      //  BOOST Serialization
      friend class boost::serialization::access;
      template <class Archive> void serialize(Archive &ar, const unsigned int version) {
	ar &lattice_ ;
	ar &K_reciprocal;
       ar &K_reciprocal_inv;
      }

      private:
      bravais_lattice lattice_;
      nda::matrix<double> K_reciprocal, K_reciprocal_inv;

      ///FIXME
      template <typename K> k_t _transfo_impl(K const &k, nda::matrix<double> const &K_base) const {
        if (first_dim(k) != lattice().ndim()) TRIQS_RUNTIME_ERROR << "latt_to_real_k : dimension of k must be " << lattice().ndim();
        auto res = k_t::zeros({3});
        for (int i = 0; i < lattice().ndim(); i++) res += k(i) * K_base(i, range{});
        return res;
      }
    };
  } // namespace lattice
} // namespace triqs
