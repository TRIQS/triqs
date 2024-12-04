// Copyright (c) 2022 Simons Foundation
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
// Authors: Olivier Parcollet

#include "./det_manip_test_utils.hpp"

#include <triqs/det_manip/det_manip.hpp>
#include <triqs/test_tools/arrays.hpp>

#include <nda/nda.hpp>

#include <algorithm>
#include <complex>
#include <random>
#include <vector>

// Test the refill operation of det_manip.
template <typename T> void test_refill() {
  std::mt19937 gen(23432);
  std::uniform_real_distribution<> dis(0.0, 10.0);

  // generate base det_manip object
  std::vector<double> x_args(5), y_args(5);
  std::ranges::generate(x_args, [&] { return dis(gen); });
  std::ranges::generate(y_args, [&] { return dis(gen); });
  auto dm_base        = triqs::det_manip::det_manip{builder1<T>{}, x_args, y_args};
  auto const det_base = dm_base.determinant();

  // loop over matrix sizes
  for (int n = 0; n < 9; ++n) {
    // construct new matrix builder arguments expected det_manip object
    std::vector<double> x(n), y(n);
    std::ranges::generate(x, [&] { return dis(gen); });
    std::ranges::generate(y, [&] { return dis(gen); });
    auto dm_exp        = triqs::det_manip::det_manip{builder1<T>{}, x, y};
    auto const det_exp = dm_exp.determinant();

    // try refill operation
    auto dm = dm_base;
    EXPECT_COMPLEX_NEAR(dm.try_refill(x, y), det_exp / det_base, 1.e-13);

    // complete operation
    dm.complete_operation();

    // check results
    EXPECT_ARRAY_NEAR(dm.matrix(), dm_exp.matrix(), 1.e-13);
    EXPECT_ARRAY_NEAR(dm.inverse_matrix(), dm_exp.inverse_matrix(), 1.e-13);
    EXPECT_COMPLEX_NEAR(dm.determinant(), det_exp, 1.e-13);
    EXPECT_EQ(dm.get_x_internal_order(), dm_exp.get_x_internal_order());
    EXPECT_EQ(dm.get_y_internal_order(), dm_exp.get_y_internal_order());
    EXPECT_NO_THROW(dm.regenerate_and_check());
  }
}

TEST(TRIQSDetManip, Refill) {
  test_refill<double>();
  test_refill<std::complex<double>>();
}

MAKE_MAIN;
