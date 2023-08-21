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

#include <triqs/test_tools/gfs.hpp>

TEST(Gf, BlockPyH5) {

  auto mesh = mesh::imfreq{/*beta*/ 10.0, Fermion, /*n_iw*/ 100};
  auto g_iw = gf{mesh, make_shape()};
  g_iw()    = 0.0;
  auto bgf  = block_gf{std::vector{g_iw, g_iw}};

  auto file    = h5::file("block_pyh5_ref.h5", 'r');
  auto bgf_ref = h5::read<block_gf<imfreq, scalar_valued>>(file, "bgf");

  EXPECT_BLOCK_GF_NEAR(bgf, bgf_ref);
}

MAKE_MAIN;
