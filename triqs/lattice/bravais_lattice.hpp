/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011-2014 by M. Ferrero, O. Parcollet
 *
 * TRIQS is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * TRIQS. If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/
#pragma once
#include <triqs/arrays.hpp>
#include <string>
#include <vector>

namespace triqs {
  namespace lattice {

    using r_t      = arrays::vector<double>;
    using k_t      = arrays::vector<double>;
    using dcomplex = std::complex<double>;
    using arrays::array;
    using arrays::matrix;
    using itertools::range;

    // --------------------------------------------------------

    class bravais_lattice {

      public:
      using point_t = std::vector<int>; // domain concept. PUT on STACK

      bravais_lattice(matrix<double> const &units, std::vector<r_t> atom_orb_pos, std::vector<std::string> atom_orb_name);
      bravais_lattice(matrix<double> const &units, std::vector<r_t> atom_orb_pos)
         : bravais_lattice(units, atom_orb_pos, std::vector<std::string>(atom_orb_pos.size(), "")) {}
      bravais_lattice(matrix<double> const &units) : bravais_lattice(units, std::vector<r_t>{{0, 0, 0}}) {}
      bravais_lattice() : bravais_lattice(arrays::make_unit_matrix<double>(2)) {}

      int n_orbitals() const { return atom_orb_name.size(); }
      arrays::matrix_const_view<double> units() const { return units_; }
      int dim() const { return dim_; }

      /// Transform into real coordinates.
      template <typename R> r_t lattice_to_real_coordinates(R const &x) const {
        r_t res(3);
        res() = 0;
        for (int i = 0; i < dim_; i++) res += x(i) * units_(i, range{});
        return res;
      }

      // ------------------- Comparison -------------------

      bool operator==(bravais_lattice const &bl) const { return units_ == bl.units() && dim_ == bl.dim() && n_orbitals() == bl.n_orbitals(); }

      bool operator!=(bravais_lattice const &bl) const { return !(operator==(bl)); }

      // -------------------- print -------------------

      friend std::ostream &operator<<(std::ostream &sout, bravais_lattice const &bl) {
        return sout << "Bravais Lattice with dimension " << bl.dim_ << ", units " << bl.units_ << ", n_orbitals " << bl.n_orbitals();
      }

      /// Write into HDF5
      friend void h5_write(h5::group fg, std::string subgroup_name, bravais_lattice const &bl);

      /// Read from HDF5
      friend void h5_read(h5::group fg, std::string subgroup_name, bravais_lattice &bl);

      //  BOOST Serialization
      friend class boost::serialization::access;
      template <class Archive> void serialize(Archive &ar, const unsigned int version) {
        ar &TRIQS_MAKE_NVP("units", units_);
        ar &TRIQS_MAKE_NVP("atom_orb_pos", atom_orb_pos);
        ar &TRIQS_MAKE_NVP("atom_orb_name", atom_orb_name);
      }

      private:
      matrix<double> units_;
      std::vector<r_t> atom_orb_pos;          // atom_orb_pos[i] = position of ith atoms/orbitals in the unit cell
      std::vector<std::string> atom_orb_name; // names of these atoms/orbitals.
      int dim_;
    };
  } // namespace lattice
} // namespace triqs
