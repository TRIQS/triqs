// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
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

#include "./bravais_lattice.hpp"
#include "../utility/exceptions.hpp"

#include <h5/h5.hpp>
#include <nda/h5.hpp>
#include <nda/nda.hpp>

#include <cmath>
#include <string>
#include <vector>

namespace triqs::lattice {

  bravais_lattice::bravais_lattice(nda::matrix<double> const &A_T, std::vector<r_t> orb_pos, std::vector<std::string> orb_name)
     : atom_orb_pos_(orb_pos) {
    constexpr double almost_zero = 1e-10;
    atom_orb_name_                = orb_name.empty() ? std::vector<std::string>(atom_orb_pos_.size(), "") : orb_name;
    EXPECTS(atom_orb_pos_.size() == atom_orb_name_.size());

    using nda::blas::dot;
    using nda::linalg::cross_product;

    ndim_ = first_dim(A_T);
    if ((ndim_ < 1) || (ndim_ > 3)) TRIQS_RUNTIME_ERROR << " units matrix must be square matrix of size 1, 2 or 3";

    auto r       = range(ndim_);
    units_()     = 0;
    units_(r, r) = A_T(r, r);
    // First complete the basis. Add some tests for safety
    nda::vector<double> ux(3), uy(3), uz(3);
    double delta;
    switch (ndim_) {
      case 1:
        ux    = units_(0, range::all);
        uz()  = 0;
        uz(1) = 1;
        uz    = uz - dot(uz, ux) * ux;
        // no luck, ux was parallel to z, another one must work
        if (sqrt(dot(uz, uz)) < almost_zero) {
          uz()  = 0;
          uz(2) = 1; // 0,0,1;
          uz    = uz - dot(uz, ux) * ux;
        }
        uz /= sqrt(dot(uz, uz));
        uy                    = cross_product(uz, ux);
        uy                    = uy / sqrt(dot(uy, uy)); // uy cannot be 0
        units_(1, range::all) = uz;
        units_(2, range::all) = uy;
        break;
      case 2:
        uy()  = 0;
        uy(2) = 1;
        uy    = cross_product(units_(0, range::all), units_(1, range::all));
        delta = sqrt(dot(uy, uy));
        using std::abs;
        if (abs(delta) < almost_zero) TRIQS_RUNTIME_ERROR << "Bravais Lattice : the 2 vectors of unit are not independent : " << A_T;
        units_(2, range::all) = uy / delta;
        break;
      case 3:
        using std::abs;
        ux    = units_(0, range::all);
        uy    = units_(1, range::all);
        uz    = units_(2, range::all);
        delta = dot(cross_product(ux, uy), uz);
        if (abs(delta) < almost_zero) TRIQS_RUNTIME_ERROR << "Bravais Lattice : 2 of the 3 vectors of unit are not independent : " << A_T;
        break;
    }
    units_inv_ = nda::linalg::inv(units_);
  }

  void h5_write(h5::group g, std::string const &name, bravais_lattice const &bl) {
    auto gr = g.create_group(name);
    h5::write_hdf5_format(gr, bl); // NOLINT (downcasting to base class)
    auto rg = nda::range(bl.ndim());
    h5::write(gr, "units", bl.units_(rg, rg));
    h5::write(gr, "atom_orb_pos", bl.atom_orb_pos_);
    h5::write(gr, "atom_orb_name", bl.atom_orb_name_);
  }

  void h5_read(h5::group g, std::string const &name, bravais_lattice &bl) {
    h5::group gr = g.open_group(name);
    nda::matrix<double> A_T;
    h5::read(gr, "units", A_T);
    auto orb_pos = std::vector<r_t>{{0, 0, 0}};
    h5::try_read(gr, "atom_orb_pos", orb_pos);
    auto orb_name = std::vector<std::string>(orb_pos.size(), "");
    h5::try_read(gr, "atom_orb_name", orb_name);
    bl = bravais_lattice{A_T, orb_pos, orb_name};
  }

} // namespace triqs::lattice
