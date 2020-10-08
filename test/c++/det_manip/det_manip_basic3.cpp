// Copyright (c) 2016-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2016-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018 Simons Foundation
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
// Authors: Olivier Parcollet, Nils Wentzell

#include <triqs/test_tools/arrays.hpp>
#include <triqs/arrays.hpp>
#include <triqs/det_manip/det_manip.hpp>
#include <triqs/det_manip/det_manip_basic.hpp>

namespace arrays = triqs::arrays;
using namespace triqs::det_manip;

struct func {
  // gives the coefficients of the matrix (function F of the documentation)
  double operator()(int x, int y) const {
    if ((x < 0) and (y < 0))
      return 1;
    else
      return x + y;
  }
};

det_manip_basic<func> init_dm() {
  func f;
  std::vector<int> initial_x{-2, 2}, initial_y{-5, 3};
  det_manip_basic<func> dm(f, initial_x, initial_y);
  std::cerr << "matrix = " << dm.matrix() << std::endl;
  std::cerr << "det    = " << dm.determinant() << std::endl;
  return dm;
}

det_manip_basic<func> remove_second_row_col(det_manip_basic<func> dm) {
  dm.remove(1, 1);
  std::cerr << "matrix = " << dm.matrix() << std::endl;
  std::cerr << "det    = " << dm.determinant() << std::endl;
  return dm;
}

det_manip_basic<func> insert_second_row_col(det_manip_basic<func> dm) {
  dm.insert(1, 1, 6, 4);
  std::cerr << "matrix = " << dm.matrix() << std::endl;
  std::cerr << "det    = " << dm.determinant() << std::endl;
  return dm;
}

det_manip_basic<func> remove_first_row_col(det_manip_basic<func> dm) {
  dm.remove(0, 0);
  std::cerr << "matrix = " << dm.matrix() << std::endl;
  std::cerr << "det    = " << dm.determinant() << std::endl;
  return dm;
}

// ----- TESTS ------------------

TEST(det_manip, det_manip_zero_mat) {

  // using std::abs;

  auto dm1                        = init_dm();
  auto dm2                        = remove_second_row_col(dm1);
  auto dm3                        = insert_second_row_col(dm2);
  auto dm4                        = remove_first_row_col(dm2);
  arrays::matrix<double> true_dm1 = {{1, 1}, {-3, 5}};
  arrays::matrix<double> true_dm2 = {{1}};
  arrays::matrix<double> true_dm3 = {{1, 2}, {1, 10}};
  arrays::matrix<double> true_dm4 = {};

  EXPECT_ARRAY_NEAR(dm1.matrix(), true_dm1);
  EXPECT_EQ(dm1.determinant(), 8);

  EXPECT_ARRAY_NEAR(dm2.matrix(), true_dm2);
  EXPECT_EQ(dm2.determinant(), 1);

  EXPECT_ARRAY_NEAR(dm3.matrix(), true_dm3);
  EXPECT_EQ(dm3.determinant(), 8);

  EXPECT_ARRAY_NEAR(dm4.matrix(), true_dm4);
  EXPECT_EQ(dm4.determinant(), 1);
}

// ------------------------

MAKE_MAIN;
