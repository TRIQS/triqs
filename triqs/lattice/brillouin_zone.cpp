/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011 by M. Ferrero, O. Parcollet
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
#include "./brillouin_zone.hpp"
#include <triqs/arrays/blas_lapack/dot.hpp>
#include <triqs/arrays/linalg/det_and_inverse.hpp>
#include <triqs/arrays/linalg/cross_product.hpp>
namespace triqs {
namespace lattice {

 const double almost_zero = 1e-10;


 brillouin_zone::brillouin_zone(bravais_lattice const& bl_) : lattice_(bl_), K_reciprocal(3, 3) {
  using arrays::range;
  auto _ = range{};
  auto Units = lattice().units();
  double delta = dot(Units(0, _), cross_product(Units(1, _), Units(2, _)));
  if (std::abs(delta) < almost_zero) TRIQS_RUNTIME_ERROR << "Brillouin Zone: the 3 unit vectors are not independent: " << Units;
  K_reciprocal(0, _) = cross_product(Units(1, _), Units(2, _)) / delta;
  K_reciprocal(1, _) = cross_product(Units(2, _), Units(0, _)) / delta;
  K_reciprocal(2, _) = cross_product(Units(0, _), Units(1, _)) / delta;
  K_reciprocal = K_reciprocal * 2 * M_PI;
  K_reciprocal_inv = inverse(K_reciprocal);
 }

 //------------------------------------------------------------------------------------
 /// Write into HDF5
 void h5_write(h5::group fg, std::string subgroup_name, brillouin_zone const& bz) {
  h5::group gr = fg.create_group(subgroup_name);
  h5_write(gr, "bravais_lattice", bz.lattice_);
 }

 /// Read from HDF5
 void h5_read(h5::group fg, std::string subgroup_name, brillouin_zone& bz) {
  h5::group gr = fg.open_group(subgroup_name);
  bravais_lattice bl;
  h5_read(gr, "bravais_lattice", bl);
  bz = brillouin_zone{bl};
 }
}
} // namespaces

