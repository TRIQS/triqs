// Copyright (c) 2020 Simons Foundation
// Copyright (c) 2017 Igor Krivenko
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
// Authors: Igor Krivenko, Olivier Parcollet, Nils Wentzell

#include <triqs/test_tools/gfs.hpp>

#include <triqs/atom_diag/atom_diag.hpp>
#include <h5/h5.hpp>

#include "./hamiltonian.hpp"

TEST(atom_diag, HDF5) {
  auto fops       = make_fops();
  auto h_real     = make_hamiltonian<many_body_operator_real>(0.4, 1.0, 0.3, 0.03, 0.2);
  auto h_complex  = make_hamiltonian<many_body_operator_complex>(0.4, 1.0, 0.3, 0.03, 0.2i);
  auto ad_real    = triqs::atom_diag::atom_diag<false>(h_real, fops);
  auto ad_complex = triqs::atom_diag::atom_diag<true>(h_complex, fops);

  rw_h5(ad_real, "ad_real", "ad");
  rw_h5(ad_complex, "ad_complex", "ad");

  // We should be able to read a real atom_diag into complex, but not the other way around
  h5::file f_real("ad_real.h5", 'r');
  EXPECT_NO_THROW(h5_read(f_real, "ad", ad_complex));
  h5::file f_complex("ad_complex.h5", 'r');
  EXPECT_THROW(h5_read(f_complex, "ad", ad_real), triqs::runtime_error);
}

TEST(atom_diag, HDF5_empty) {
  auto ad_real    = triqs::atom_diag::atom_diag<false>{};
  auto ad_complex = triqs::atom_diag::atom_diag<true>{};

  rw_h5(ad_real, "ad_real", "ad");
  rw_h5(ad_complex, "ad_complex", "ad");
}

MAKE_MAIN;
