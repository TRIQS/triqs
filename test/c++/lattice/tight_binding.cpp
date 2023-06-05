// Copyright (c) 2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2021 Simons Foundation
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
// Authors: Nils Wentzell

#include <triqs/test_tools/arrays.hpp>
#include <triqs/test_tools/gfs.hpp>

#include <triqs/lattice/tight_binding.hpp>

#include <vector>

using namespace triqs::gfs;
using namespace triqs::lattice;
using namespace triqs::arrays;

TEST(tight_binding, h5_read_write) {
  auto units           = nda::matrix<double>{{1., 0., 0.}, {0., 1., 0.}, {0., 0., 1.}};
  int norb             = 2;
  auto atom_orb_pos    = std::vector(norb, nda::vector<double>{0., 0., 0.});
  auto bl              = bravais_lattice(units, atom_orb_pos);
  auto displ_vec       = std::vector<nda::vector<long>>{{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}};
  double t             = 1.0;
  auto overlap_mat_vec = std::vector(displ_vec.size(), nda::diag(nda::vector<dcomplex>{t, t}));

  auto tb = tight_binding{bl, displ_vec, overlap_mat_vec};

  // write
  {
    auto file = h5::file{"test_tb.h5", 'w'};
    auto grp  = h5::group{file};
    h5_write(grp, "cubic_lattice_nn_hop", tb);
  }

  // read
  {
    auto file  = h5::file{"test_tb.h5", 'r'};
    auto grp   = h5::group{file};
    auto tb_in = h5::h5_read<tight_binding>(grp, "cubic_lattice_nn_hop");
    EXPECT_EQ(tb, tb_in);
  }
}

MAKE_MAIN;
