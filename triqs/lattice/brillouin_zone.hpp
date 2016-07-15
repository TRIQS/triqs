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
#include "./bravais_lattice.hpp"

namespace triqs {
namespace lattice {

 class brillouin_zone {

  public:
  using point_t = k_t; // domain concept

  brillouin_zone() { // default construction, 3d cubic lattice
   K_reciprocal = arrays::make_unit_matrix<double>(3);
   K_reciprocal_inv = K_reciprocal;
  }

  /// Construct from a bravais_lattice
  brillouin_zone(bravais_lattice const& bl_);

  /// Access to the underlying bravais lattice
  bravais_lattice lattice() const { return lattice_; }
  
  ///return reciprocal matrix: lines are cartesian coordinates of each reciprocal unit vectors
  arrays::matrix_view<double> units() const { return K_reciprocal; }
  ///return reciprocal matrix: lines are cartesian coordinates of each reciprocal unit vectors
  arrays::matrix_view<double> reciprocal_matrix() const { return K_reciprocal; }

  arrays::matrix_view<double> reciprocal_matrix_inv() const { return K_reciprocal_inv; }

  /// Transform from lattice to real coordinates
  template <typename K> k_t lattice_to_real_coordinates(K const& k) const { return _transfo_impl(k, K_reciprocal); }

  /// Transform from real to lattice coordinates
  template <typename K> k_t real_to_lattice_coordinates(K const& k) const { return _transfo_impl(k, K_reciprocal_inv); }

  /// Write into HDF5
  friend void h5_write(h5::group fg, std::string subgroup_name, brillouin_zone const& bz);

  /// Read from HDF5
  friend void h5_read(h5::group fg, std::string subgroup_name, brillouin_zone& bz);

  //  BOOST Serialization
  friend class boost::serialization::access;
  template <class Archive> void serialize(Archive& ar, const unsigned int version) {
   ar& lattice_;
  }

  private:
  bravais_lattice lattice_;
  arrays::matrix<double> K_reciprocal, K_reciprocal_inv;

  ///FIXME
  template <typename K> k_t _transfo_impl(K const& k, arrays::matrix<double> const& K_base) const {
   if (first_dim(k) != lattice().dim()) TRIQS_RUNTIME_ERROR << "latt_to_real_k : dimension of k must be " << lattice().dim();
   k_t res(3);
   res() = 0;
   int dim = lattice().dim();
   for (int i = 0; i < dim; i++) res += k(i) * K_base(i, range{});
   return res;
  }
 };
}
}
