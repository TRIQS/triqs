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

  bravais_lattice::bravais_lattice(nda::matrix<double> const &units, std::vector<r_t> orbital_positions, std::vector<std::string> atom_orb_name)
     : atom_orb_pos_(std::move(orbital_positions)),
       atom_orb_name_(atom_orb_name.empty() ? std::vector<std::string>(atom_orb_pos_.size()) : std::move(atom_orb_name)),
       ndim_(static_cast<int>(nda::first_dim(units))) {
    // consistency checks
    EXPECTS(atom_orb_pos_.size() == atom_orb_name_.size());
    if (ndim_ < 1 || ndim_ > 3) TRIQS_RUNTIME_ERROR << "Error in triqs::lattice::bravais_lattice: Basis vector matrix has wrong size: " << units;

    // initialize basis vectors
    auto rg        = nda::range(ndim_);
    units_(rg, rg) = units(rg, rg);

    // complete the basis for 1D and 2D
    if (ndim_ < 3) {
      if (ndim_ == 1) units_(1, 1) = 1;
      units_(2, nda::range::all) = nda::linalg::cross_product(units_(0, nda::range::all), units_(1, nda::range::all));
      units_(2, nda::range::all) /= nda::linalg::norm(units_(2, nda::range::all));
    }

    // linear independence check
    if (std::abs(nda::linalg::det(units_)) < 1e-10)
      TRIQS_RUNTIME_ERROR << "Error in triqs::lattice::bravais_lattice: Basis vectors are not linearly independent" << units_;

    // compute inverse (used for basis transformations)
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
