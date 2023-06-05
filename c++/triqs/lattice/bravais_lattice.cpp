// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
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

#include "./bravais_lattice.hpp"
#include <nda/lapack.hpp>
#include <nda/linalg/cross_product.hpp>

namespace triqs {
  namespace lattice {

    const double almost_zero = 1e-10;

    bravais_lattice::bravais_lattice(nda::matrix<double> const &units__, std::vector<r_t> atom_orb_pos_, std::vector<std::string> atom_orb_name_)
       : atom_orb_pos(atom_orb_pos_) {

      atom_orb_name = atom_orb_name_.empty() ? std::vector<std::string>(atom_orb_pos.size(), "") : atom_orb_name_;
      EXPECTS(atom_orb_pos.size() == atom_orb_name.size());

      using nda::blas::dot;
      using nda::linalg::cross_product;

      ndim_ = first_dim(units__);
      if ((ndim_ < 1) || (ndim_ > 3)) TRIQS_RUNTIME_ERROR << " units matrix must be square matrix of size 1, 2 or 3";
      //using itertools::range;
      auto r       = range(ndim_);
      units_()     = 0;
      units_(r, r) = units__(r, r);
      // First complete the basis. Add some tests for safety
      nda::vector<double> ux(3), uy(3), uz(3);
      double delta;
      auto _ = range::all;
      switch (ndim_) {
        case 1:
          ux    = units_(0, _);
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
          uy           = cross_product(uz, ux);
          uy           = uy / sqrt(dot(uy, uy)); // uy cannot be 0
          units_(1, _) = uz;
          units_(2, _) = uy;
          break;
        case 2:
          uy()  = 0;
          uy(2) = 1;
          uy    = cross_product(units_(0, _), units_(1, _));
          delta = sqrt(dot(uy, uy));
          using std::abs;
          if (abs(delta) < almost_zero) TRIQS_RUNTIME_ERROR << "Bravais Lattice : the 2 vectors of unit are not independent : " << units__;
          units_(2, _) = uy / delta;
          break;
        case 3:
          using std::abs;
          ux    = units_(0, _);
          uy    = units_(1, _);
          uz    = units_(2, _);
          delta = dot(cross_product(ux, uy), uz);
          if (abs(delta) < almost_zero) TRIQS_RUNTIME_ERROR << "Bravais Lattice : 2 of the 3 vectors of unit are not independent : " << units__;
          break;
      }
      units_inv_ = inverse(units_);
    }
    //------------------------------------------------------------------------------------

    /// Write into HDF5
    void h5_write(h5::group fg, std::string subgroup_name, bravais_lattice const &bl) {
      auto gr = fg.create_group(subgroup_name);
      write_hdf5_format(gr, bl);
      auto _       = range(0, bl.ndim());
      h5::write(gr, "units", bl.units_(_, _));
      h5::write(gr, "atom_orb_pos", bl.atom_orb_pos);
      h5::write(gr, "atom_orb_name", bl.atom_orb_name);
    }

    /// Read from HDF5
    void h5_read(h5::group fg, std::string subgroup_name, bravais_lattice &bl) {
      h5::group gr = fg.open_group(subgroup_name);
      nda::matrix<double> units__;
      h5::read(gr, "units", units__);

      auto atom_orb_pos = std::vector<r_t>{{0, 0, 0}};
      h5::try_read(gr, "atom_orb_pos", atom_orb_pos);

      auto atom_orb_name = std::vector<std::string>(atom_orb_pos.size(), "");
      h5::try_read(gr, "atom_orb_name", atom_orb_name);

      bl = bravais_lattice{units__, atom_orb_pos, atom_orb_name};
    }

    //------------------------------------------------------------------------------------
    //------------------------------------------------------------------------------------

  } // namespace lattice
} // namespace triqs
