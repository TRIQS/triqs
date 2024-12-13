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

// Test the change row/column operation of det_manip.
template <typename T> void test_change() {
  auto builder = builder3<T>{};

  // loop over matrix sizes
  for (int n = 1; n < 10; ++n) {
    // generate base det_manip object
    std::vector<int> x_base(n), y_base(n);
    std::iota(x_base.begin(), x_base.end(), 0);
    y_base              = x_base;
    auto dm_base        = triqs::det_manip::det_manip{builder, x_base, y_base};
    auto const det_base = dm_base.determinant();

    // loop over all rows and columns
    for (int i = 0; i < n; ++i) {
      for (int j = 0; j < n; ++j) {
        // generate new argument and construct expected det_manip object
        auto x_exp         = x_base;
        x_exp[i]           = 10 + i;
        auto y_exp         = y_base;
        y_exp[j]           = 10 + j;
        auto dm_exp        = dm_base;
        dm_exp             = triqs::det_manip::det_manip{builder, x_exp, y_exp};
        auto const det_exp = dm_exp.determinant();

        // perform operation
        auto dm          = dm_base;
        auto const ratio = dm.try_change_col_row(i, j, x_exp[i], y_exp[j]);
        dm.complete_operation();

        // check results
        using triqs::det_manip::detail::rel_diff;
        EXPECT_LT(rel_diff(ratio, dm_exp.determinant() / det_base), 1.e-8);
        EXPECT_LT(rel_diff(dm.matrix(), dm_exp.matrix()), 1.e-8);
        EXPECT_LT(rel_diff(dm.inverse_matrix(), dm_exp.inverse_matrix()), 1.e-8);
        EXPECT_LT(rel_diff(dm.determinant(), det_exp), 1.e-8);
        EXPECT_EQ(dm.get_x_internal_order(), dm_exp.get_x_internal_order());
        EXPECT_EQ(dm.get_y_internal_order(), dm_exp.get_y_internal_order());
        EXPECT_NO_THROW(dm.regenerate_and_check());
      }
    }
  }
}

TEST(TRIQSDetManip, ChangeRowAndColumnDouble) { test_change<double>(); }

TEST(TRIQSDetManip, ChangeRowAndColumnComplex) { test_change<std::complex<double>>(); }

MAKE_MAIN;
