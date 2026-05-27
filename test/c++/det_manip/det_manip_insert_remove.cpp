// Copyright (c) 2016-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2016-2018 Centre national de la recherche scientifique (CNRS)
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
// Authors: Olivier Parcollet, Nils Wentzell

#include "./det_manip_test_utils.hpp"

#include <triqs/det_manip/det_manip.hpp>
#include <triqs/test_tools/arrays.hpp>

#include <nda/nda.hpp>

#include <vector>

// Construct a det_manip object.
auto init_dm() {
  std::vector<int> initial_x{-2, 2}, initial_y{-5, 3};
  return triqs::det_manip::det_manip{builder4{}, initial_x, initial_y};
}

// Remove the second row and column.
auto remove_second_row_col(auto dm) {
  dm.try_remove(1, 1);
  dm.complete_operation();
  return dm;
}

// Insert a row and column at position 1.
auto insert_second_row_col(auto dm) {
  dm.try_insert(1, 1, 6, 4);
  dm.complete_operation();
  return dm;
}

// Remove the first row and column.
auto remove_first_row_col(auto dm) {
  dm.try_remove(0, 0);
  dm.complete_operation();
  return dm;
}

TEST(TRIQSDetManip, InsertAndRemoveSpecificRowsAndColumns) {
  std::vector<int> x_args{-2, 2}, y_args{-5, 3};

  // det_manip objects
  auto dm1 = triqs::det_manip::det_manip{builder4{}, x_args, y_args};
  auto dm2 = remove_second_row_col(dm1);
  auto dm3 = insert_second_row_col(dm2);
  auto dm4 = remove_first_row_col(dm2);

  // expected matrices
  nda::matrix<double> F1 = {{1, 1}, {-3, 5}};
  nda::matrix<double> F2 = {{1}};
  nda::matrix<double> F3 = {{1, 2}, {1, 10}};
  nda::matrix<double> F4 = {};

  // check results
  EXPECT_ARRAY_NEAR(dm1.matrix(), F1);
  EXPECT_EQ(dm1.determinant(), 8);

  EXPECT_ARRAY_NEAR(dm2.matrix(), F2);
  EXPECT_EQ(dm2.determinant(), 1);

  EXPECT_ARRAY_NEAR(dm3.matrix(), F3);
  EXPECT_EQ(dm3.determinant(), 8);

  EXPECT_ARRAY_NEAR(dm4.matrix(), F4);
  EXPECT_EQ(dm4.determinant(), 1);
}

MAKE_MAIN;
