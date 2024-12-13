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

#include <algorithm>
#include <complex>
#include <random>
#include <vector>

// Test the change row/column operation of det_manip.
template <typename T, bool do_row> void test_change() {
  std::mt19937 gen(23432);
  std::uniform_real_distribution<> dis(0.0, 10.0);

  // loop over matrix sizes
  for (int n = 1; n < 9; ++n) {
    // generate base det_manip object
    std::vector<double> x_args(n), y_args(n);
    std::ranges::generate(x_args, [&] { return dis(gen); });
    std::ranges::generate(y_args, [&] { return dis(gen); });
    auto dm_base        = triqs::det_manip::det_manip{builder1<T>{}, x_args, y_args};
    auto const det_base = dm_base.determinant();

    // loop over all rows/columns
    for (int i = 0; i < n; ++i) {
      // generate new argument, construct expected det_manip object and perform change operation
      auto dm_exp = dm_base;
      auto dm     = dm_base;
      T ratio;
      if constexpr (do_row) {
        auto x_new = x_args;
        x_new[i]   = dis(gen);
        dm_exp     = triqs::det_manip::det_manip{builder1<T>{}, x_new, y_args};
        ratio      = dm.try_change_row(i, x_new[i]);
      } else {
        auto y_new = y_args;
        y_new[i]   = dis(gen);
        dm_exp     = triqs::det_manip::det_manip{builder1<T>{}, x_args, y_new};
        ratio      = dm.try_change_col(i, y_new[i]);
      }
      auto const det_exp = dm_exp.determinant();
      dm.complete_operation();

      // check results
      using triqs::det_manip::detail::rel_diff;
      EXPECT_LT(rel_diff(ratio, dm_exp.determinant() / det_base), 1.e-6);
      EXPECT_LT(rel_diff(dm.matrix(), dm_exp.matrix()), 1.e-6);
      EXPECT_LT(rel_diff(dm.inverse_matrix(), dm_exp.inverse_matrix()), 1.e-6);
      EXPECT_LT(rel_diff(dm.determinant(), det_exp), 1.e-6);
      EXPECT_EQ(dm.get_x_internal_order(), dm_exp.get_x_internal_order());
      EXPECT_EQ(dm.get_y_internal_order(), dm_exp.get_y_internal_order());
      EXPECT_NO_THROW(dm.regenerate_and_check());
    }
  }
}

TEST(TRIQSDetManip, ChangeColumnDouble) { test_change<double, false>(); }

TEST(TRIQSDetManip, ChangeColumnComplex) { test_change<std::complex<double>, false>(); }

TEST(TRIQSDetManip, ChangeRowDouble) { test_change<double, true>(); }

TEST(TRIQSDetManip, ChangeRowComplex) { test_change<std::complex<double>, true>(); }

MAKE_MAIN;
