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
#include <triqs/det_manip/utils.hpp>
#include <triqs/test_tools/arrays.hpp>

#include <nda/nda.hpp>

#include <complex>
#include <numeric>
#include <vector>

// Test the insert operations of det_manip.
template <typename T> void test_insert() {
  auto builder = builder3<T>{};

  // loop over matrix sizes
  for (int n = 0; n < 10; ++n) {
    // generate base det_manip object
    std::vector<int> x_base(n), y_base(n);
    std::iota(x_base.begin(), x_base.end(), 0);
    y_base              = x_base;
    auto dm_base        = triqs::det_manip::det_manip{builder, x_base, y_base};
    auto const det_base = dm_base.determinant();

    // loop over insert positions
    for (int i = 0; i <= n; ++i) {
      for (int j = 0; j <= n; ++j) {
        // construct matrix builder arguments for the new rows/columns
        int x = 10 + i;
        int y = 10 + j;

        // insert matrix builder arguments
        auto x_exp = x_base;
        auto y_exp = y_base;
        x_exp.insert(x_exp.begin() + i, x);
        y_exp.insert(y_exp.begin() + j, y);

        // construct expected det_manip object
        auto dm_exp        = triqs::det_manip::det_manip{builder, x_exp, y_exp};
        auto const det_exp = dm_exp.determinant();

        // try insert operation
        auto dm          = dm_base;
        auto const ratio = dm.try_insert(i, j, x, y);

        // complete operation
        dm.complete_operation();

        // check results
        using triqs::det_manip::detail::rel_diff;
        EXPECT_LT(rel_diff(ratio, dm_exp.determinant() / det_base), 1.e-8);
        EXPECT_LT(rel_diff(dm.matrix(), dm_exp.matrix()), 1.e-8);
        EXPECT_LT(rel_diff(dm.inverse_matrix(), dm_exp.inverse_matrix()), 1.e-8);
        EXPECT_LT(rel_diff(dm.determinant(), det_exp), 1.e-8);
        EXPECT_NO_THROW(dm.regenerate_and_check());
      }
    }
  }
}

TEST(TRIQSDetManip, InsertDouble) { test_insert<double>(); }

TEST(TRIQSDetManip, InsertComplex) { test_insert<std::complex<double>>(); }

MAKE_MAIN;
