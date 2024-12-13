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

#include <complex>
#include <numeric>
#include <vector>

// Test the refill operation of det_manip.
template <typename T> void test_refill() {
  auto builder = builder3<T>{};

  // loop over base matrix sizes
  for (int n = 0; n < 10; ++n) {
    // generate base det_manip object
    std::vector<int> x_base(n), y_base(n);
    std::iota(x_base.begin(), x_base.end(), 0);
    y_base              = x_base;
    auto dm_base        = triqs::det_manip::det_manip{builder, x_base, y_base};
    auto const det_base = dm_base.determinant();

    // loop over new matrix sizes
    for (int m = 0; m < 10; ++m) {
      // generate new arguments and construct expected det_manip object
      std::vector<int> x(m), y(m);
      std::iota(x.begin(), x.end(), 10);
      std::iota(y.begin(), y.end(), 10);
      auto dm_exp        = triqs::det_manip::det_manip{builder, x, y};
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
}

TEST(TRIQSDetManip, RefillDouble) { test_refill<double>(); }

TEST(TRIQSDetManip, RefillComplex) { test_refill<std::complex<double>>(); }

MAKE_MAIN;
