// Copyright (c) 2022-2023 Simons Foundation
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

#include <triqs/lattice/bravais_lattice.hpp>
#include <triqs/test_tools/arrays.hpp>

#include <h5/h5.hpp>
#include <nda/nda.hpp>

#include <array>
#include <iostream>
#include <string>
#include <vector>

TEST(TRIQSLattice, BravaisLatticeSimpleCubic) {
  using namespace triqs::lattice;
  auto A     = nda::eye<double>(3);
  auto orbs  = std::vector<r_t>{{0, 0, 0}};
  auto names = std::vector<std::string>{""};

  // default constructed lattice should be simple cubic
  auto bl     = bravais_lattice{};
  auto bl_ref = bravais_lattice{A, orbs, names};
  EXPECT_EQ(bl, bl_ref);

  // lattice points
  auto pt         = r_t{1, 2, 3};
  auto n          = std::array<long, 3>{1, 2, 3};
  auto lattice_pt = bl.to_point(n);
  EXPECT_ARRAY_NEAR(pt, static_cast<r_t>(lattice_pt));
  for (auto i = 0; auto x : pt) EXPECT_EQ(x, lattice_pt[i++]);
  EXPECT_ARRAY_NEAR(pt + pt, static_cast<r_t>(lattice_pt + lattice_pt));
  EXPECT_ARRAY_NEAR(pt - pt, static_cast<r_t>(lattice_pt - lattice_pt));
  EXPECT_ARRAY_NEAR(-pt, static_cast<r_t>(-lattice_pt));
  std::cout << lattice_pt << std::endl;

  // transformations from lattice to standard basis
  EXPECT_ARRAY_NEAR(pt, bl.lattice_to_real_coordinates(pt));
  EXPECT_ARRAY_NEAR(pt, bl.real_to_lattice_coordinates(pt));

  // print the lattice
  std::cout << bl << std::endl;
}

TEST(TRIQSLattice, BravaisLatticeRectangularWithBasis) {
  using namespace triqs::lattice;
  constexpr int dim = 2;
  auto rg           = nda::range(dim);
  auto A            = matrix_t{{1, 0}, {0, 2}};
  auto orbs         = std::vector<r_t>{{0, 0}, {0.5, 0.5}};
  auto names        = std::vector<std::string>{"atom 1", "atom 2"};

  // rectangular lattice with basis
  auto bl = bravais_lattice{A, orbs, names};
  EXPECT_EQ(bl.ndim(), dim);
  EXPECT_ARRAY_NEAR(bl.units()(rg, rg), A);
  EXPECT_EQ(bl.orbital_positions(), orbs);
  EXPECT_EQ(bl.orbital_names(), names);

  // lattice points
  auto n          = std::array<long, 3>{1, 2, 0};
  auto pt         = r_t::zeros(3); // why should it be 3-dimensional?
  pt(rg)          = A * nda::vector_view(n);
  auto lattice_pt = bl.to_point(n);
  EXPECT_ARRAY_NEAR(pt, static_cast<r_t>(lattice_pt));
  for (auto i = 0; auto x : pt) EXPECT_NEAR(x, lattice_pt[i++], 1e-14);
  EXPECT_ARRAY_NEAR(pt + pt, static_cast<r_t>(lattice_pt + lattice_pt));
  EXPECT_ARRAY_NEAR(pt - pt, static_cast<r_t>(lattice_pt - lattice_pt));
  EXPECT_ARRAY_NEAR(-pt, static_cast<r_t>(-lattice_pt));
  std::cout << lattice_pt << std::endl;

  // transformations from lattice to standard basis
  EXPECT_ARRAY_NEAR(pt, bl.lattice_to_real_coordinates(n));
  EXPECT_ARRAY_NEAR(nda::vector_view(n), bl.real_to_lattice_coordinates(pt));

  // print the lattice
  std::cout << bl << std::endl;
}

TEST(TRIQSLattics, BravaisLatticeHDF5) {
  using namespace triqs::lattice;
  auto A     = nda::matrix<double>{{2, 0, 0}, {0, 3, 0}, {0, 0, 1}};
  auto orbs  = std::vector<r_t>{{0, 0, 0}, {0.5, 0.5, 0.5}};
  auto names = std::vector<std::string>{"atom 1", "atom 2"};
  auto bl    = bravais_lattice{A, orbs, names};

  // write and read to HDF5
  auto bl_r = rw_h5(bl, "triqs_lattice_bravais_lattice", "bl");
  EXPECT_EQ(bl, bl_r);
}

MAKE_MAIN;
