// Copyright (c) 2016-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2016-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2022 Simons Foundation
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
 * @brief Implementation details for triqs/lattice/brillouin_zone.hpp.
 */

#include "./bravais_lattice.hpp"
#include "./brillouin_zone.hpp"

#include <h5/h5.hpp>
#include <nda/nda.hpp>

#include <numbers>
#include <string>
#include <utility>

namespace triqs::lattice {

  brillouin_zone::brillouin_zone(bravais_lattice bl) : lattice_(std::move(bl)) {
    using std::numbers::pi;
    k_units_     = 2 * pi * nda::linalg::inv(nda::transpose(lattice_.units()));
    k_units_inv_ = 1 / (2 * pi) * nda::transpose(lattice_.units());
  }

  void h5_write(h5::group g, std::string const &name, brillouin_zone const &bz) {
    h5::group gr = g.create_group(name);
    h5::write_hdf5_format(gr, bz); // NOLINT (downcasting to base class)
    h5::write(gr, "bravais_lattice", bz.lattice_);
  }

  void h5_read(h5::group g, std::string const &name, brillouin_zone &bz) {
    h5::group gr = g.open_group(name);
    bravais_lattice bl;
    h5::read(gr, "bravais_lattice", bl);
    bz = brillouin_zone{bl};
  }

} // namespace triqs::lattice
