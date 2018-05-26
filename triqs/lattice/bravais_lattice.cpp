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
#include "./bravais_lattice.hpp"
#include <triqs/arrays/blas_lapack/dot.hpp>
#include <triqs/arrays/linalg/det_and_inverse.hpp>
#include <triqs/arrays/linalg/cross_product.hpp>
namespace triqs {
namespace lattice {

 const double almost_zero = 1e-10;

 bravais_lattice::bravais_lattice(matrix<double> const& units__, std::vector<r_t> atom_orb_pos_,
                                  std::vector<std::string> atom_orb_name_)
    : units_(3, 3), atom_orb_pos(atom_orb_pos_), atom_orb_name(atom_orb_name_) {
  dim_ = first_dim(units__);
  if ((dim_ < 1) || (dim_ > 3)) TRIQS_RUNTIME_ERROR << " units matrix must be square matrix of size 1, 2 or 3";
  using arrays::range;
  auto r = range(0, dim_);
  units_() = 0;
  units_(r, r) = units__(r, r);
  // First complete the basis. Add some tests for safety
  arrays::vector<double> ux(3), uy(3), uz(3);
  double delta;
  auto _ = range{};
  switch (dim_) {
   case 1:
    ux = units_(0, _);
    uz() = 0;
    uz(1) = 1;
    uz = uz - dot(uz, ux) * ux;
    // no luck, ux was parallel to z, another one must work
    if (sqrt(dot(uz, uz)) < almost_zero) {
     uz() = 0;
     uz(2) = 1; // 0,0,1;
     uz = uz - dot(uz, ux) * ux;
    }
    uz /= sqrt(dot(uz, uz));
    uy = cross_product(uz, ux);
    uy = uy / sqrt(dot(uy, uy)); // uy cannot be 0
    units_(1, _) = uz;
    units_(2, _) = uy;
    break;
   case 2:
    uy() = 0;
    uy(2) = 1;
    uy = cross_product(units_(0, _), units_(1, _));
    delta = sqrt(dot(uy, uy));
    using std::abs;
    if (abs(delta) < almost_zero) TRIQS_RUNTIME_ERROR << "Bravais Lattice : the 2 vectors of unit are not independent : " << units__;
    units_(2, _) = uy / delta;
    break;
   case 3:
    using std::abs;
    ux = units_(0, _);
    uy = units_(1, _);
    uz = units_(2, _);
    delta = dot(cross_product(ux,uy),uz);
    if(abs(delta) < almost_zero) TRIQS_RUNTIME_ERROR << "Bravais Lattice : 2 of the 3 vectors of unit are not independent : " << units__; 
    break;
  }
 }
 //------------------------------------------------------------------------------------

 /// Write into HDF5
 void h5_write(h5::group fg, std::string subgroup_name, bravais_lattice const& bl) {
  h5::group gr = fg.create_group(subgroup_name);
  auto _ = range(0, bl.dim());
  h5_write(gr, "units", bl.units_(_,_));
 }

 /// Read from HDF5
 void h5_read(h5::group fg, std::string subgroup_name, bravais_lattice& bl) {
  h5::group gr = fg.open_group(subgroup_name);
  matrix<double> u;
  h5_read(gr, "units", u);
  bl = bravais_lattice{u}; // NOT COMPLETE
 }

 //------------------------------------------------------------------------------------
 //------------------------------------------------------------------------------------

}
} // namespaces

