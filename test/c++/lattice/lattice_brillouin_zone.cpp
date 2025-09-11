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
#include <triqs/lattice/brillouin_zone.hpp>
#include <triqs/test_tools/arrays.hpp>

#include <h5/h5.hpp>
#include <nda/nda.hpp>

#include <array>
#include <iostream>
#include <numbers>

TEST(TRIQSLattice, BrillouinZoneSimpleCubic) {
  using namespace triqs::lattice;
  using std::numbers::pi;
  auto B = nda::eye<double>(3);
  B *= 2 * pi;

  // default constructed BZ should be simple cubic
  auto bz     = brillouin_zone{};
  auto bz_ref = brillouin_zone{bravais_lattice{}};
  EXPECT_EQ(bz.ndim(), 3);
  EXPECT_ARRAY_NEAR(B, bz.units());
  EXPECT_EQ(bz, bz_ref);

  // transformations from lattice to standard basis
  auto r = r_t{2 * pi, 4 * pi, 6 * pi};
  auto n = std::array<long, 3>{1, 2, 3};
  EXPECT_ARRAY_NEAR(r, bz.lattice_to_real_coordinates(n));
  EXPECT_ARRAY_NEAR(nda::basic_array_view{n}, bz.real_to_lattice_coordinates(r));

  // inverse matrix B^{-1}
  auto B_inv = bz.reciprocal_matrix_inv();
  EXPECT_ARRAY_NEAR(B_inv, nda::transpose(bz.lattice().units()) / 2 / pi);
  EXPECT_ARRAY_NEAR(nda::eye<double>(3), bz.reciprocal_matrix_inv() * bz.reciprocal_matrix());

  // print the BZ
  std::cout << bz << std::endl;
}

TEST(TRIQSLattice, BrillouinZoneRectangularLattice) {
  using namespace triqs::lattice;
  using std::numbers::pi;
  constexpr int dim = 2;
  auto A            = matrix_t{{1, 0}, {0, 2}};
  auto bl           = bravais_lattice{A};

  // BZ of rectangular lattice
  auto bz = brillouin_zone{bl};
  EXPECT_EQ(bz.ndim(), dim);
  EXPECT_ARRAY_NEAR(2 * pi * nda::eye<double>(3), nda::transpose(bz.reciprocal_matrix()) * bz.lattice().units());

  // transformations from lattice to standard basis
  auto n_k = k_t{0.5, 0.5, 0};
  auto k   = nda::transpose(bz.reciprocal_matrix()) * n_k;
  EXPECT_ARRAY_NEAR(k, bz.lattice_to_real_coordinates(n_k));
  EXPECT_ARRAY_NEAR(n_k, bz.real_to_lattice_coordinates(k));

  // inverse matrix B^{-1}
  auto B_inv = bz.reciprocal_matrix_inv();
  EXPECT_ARRAY_NEAR(B_inv, nda::transpose(bl.units()) / 2 / pi);
  EXPECT_ARRAY_NEAR(nda::eye<double>(3), bz.reciprocal_matrix_inv() * bz.reciprocal_matrix());

  // print the BZ
  std::cout << bz << std::endl;
}

TEST(TRIQSLattics, BrillouinZoneHDF5) {
  using namespace triqs::lattice;
  auto A     = nda::matrix<double>{{2, 1, 0}, {0, 3, 2}, {0.5, 0, 1}};
  auto orbs  = std::vector<r_t>{{0, 0, 0}, A * r_t{0.5, 0.5, 0.5}};
  auto names = std::vector<std::string>{"atom 1", "atom 2"};
  auto bz    = brillouin_zone{bravais_lattice{A, orbs, names}};

  // write and read to HDF5
  auto bz_r = rw_h5(bz, "triqs_lattice_brillouin_zone", "bz");
  EXPECT_EQ(bz, bz_r);
}

MAKE_MAIN;
