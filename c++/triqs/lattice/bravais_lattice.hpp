// Copyright (c) 2014-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2014-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2020 Simons Foundation
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

#pragma once
#include <triqs/arrays.hpp>
#include <string>
#include <vector>

namespace triqs {
  namespace lattice {

    using r_t      = nda::vector<double>;
    using k_t      = nda::vector<double>;
    using dcomplex = std::complex<double>;
    using nda::array;
    using nda::matrix;

    // --------------------------------------------------------

    class bravais_lattice {

      public:
      using point_t = std::vector<int>; // domain concept. PUT on STACK

      bravais_lattice(matrix<double> const &units, std::vector<r_t> atom_orb_pos, std::vector<std::string> atom_orb_name);
      bravais_lattice(matrix<double> const &units, std::vector<r_t> atom_orb_pos)
         : bravais_lattice(units, atom_orb_pos, std::vector<std::string>(atom_orb_pos.size(), "")) {}
      bravais_lattice(matrix<double> const &units) : bravais_lattice(units, std::vector<r_t>{{0, 0, 0}}) {}
      bravais_lattice() : bravais_lattice(nda::eye<double>(2)) {}

      // Number of orbitals in the unit cell
      int n_orbitals() const { return atom_orb_name.size(); }

      /// Matrix containing lattice basis vectors as rows
      nda::matrix_const_view<double> units() const { return units_; }

      /// Number of dimensions
      int ndim() const { return ndim_; }

      /// Transform into real coordinates.
      template <typename R> r_t lattice_to_real_coordinates(R const &x) const {
        auto res = r_t::zeros({3});
        for (int i = 0; i < ndim_; i++) res += x(i) * units_(i, range{});
        return res;
      }

      // ------------------- Comparison -------------------

      bool operator==(bravais_lattice const &bl) const { return units_ == bl.units() && ndim_ == bl.ndim() && n_orbitals() == bl.n_orbitals(); }

      bool operator!=(bravais_lattice const &bl) const { return !(operator==(bl)); }

      // -------------------- print -------------------

      friend std::ostream &operator<<(std::ostream &sout, bravais_lattice const &bl) {
        return sout << "Bravais Lattice with dimension " << bl.ndim() << ", units " << bl.units() << ", n_orbitals " << bl.n_orbitals();
      }

      /// Write into HDF5
      friend void h5_write(h5::group fg, std::string subgroup_name, bravais_lattice const &bl);

      /// Read from HDF5
      friend void h5_read(h5::group fg, std::string subgroup_name, bravais_lattice &bl);

      //  BOOST Serialization
      friend class boost::serialization::access;
      template <class Archive> void serialize(Archive &ar, const unsigned int version) {
        ar &ndim_;
        ar &units_;
        ar &atom_orb_pos;
        ar &atom_orb_name;
      }

      private:
      matrix<double> units_;
      std::vector<r_t> atom_orb_pos;          // atom_orb_pos[i] = position of ith atoms/orbitals in the unit cell
      std::vector<std::string> atom_orb_name; // names of these atoms/orbitals.
      int ndim_;
    };
  } // namespace lattice
} // namespace triqs
