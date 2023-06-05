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
// Authors: Michel Ferrero, Olivier Parcollet, Nils Wentzell, Thomas Ayral

#include "./brillouin_zone.hpp"
#include <nda/blas.hpp>
#include <nda/linalg/det_and_inverse.hpp>
#include <nda/linalg/cross_product.hpp>
namespace triqs {
  namespace lattice {

    const double almost_zero = 1e-10;

    brillouin_zone::brillouin_zone(bravais_lattice const &bl) : lattice_(bl), K_reciprocal(3, 3) {
      using nda::blas::dot;
      using nda::linalg::cross_product;

      auto Units = lattice().units();
      if (abs(determinant(Units)) < almost_zero) TRIQS_RUNTIME_ERROR << "Brillouin Zone: the 3 unit vectors are not independent: " << Units;
      K_reciprocal       = 2 * M_PI * inverse(transpose(Units));
      K_reciprocal_inv   = inverse(K_reciprocal);
    }

    // -------------- HDF5  --------------------------

    void h5_write(h5::group fg, std::string subgroup_name, brillouin_zone const &bz) {
      h5::group gr = fg.create_group(subgroup_name);
      write_hdf5_format(gr, bz);
      h5_write(gr, "bravais_lattice", bz.lattice_);
    }

    void h5_read(h5::group fg, std::string subgroup_name, brillouin_zone &bz) {
      h5::group gr = fg.open_group(subgroup_name);
      bravais_lattice bl;
      h5_read(gr, "bravais_lattice", bl);
      bz = brillouin_zone{bl};
    }
  } // namespace lattice
} // namespace triqs
