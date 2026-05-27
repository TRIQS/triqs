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
#include <numeric>
#include <random>
#include <vector>

// Test the insert_k operations of det_manip.
template <typename T> void test_insert(int k) {
  auto builder = builder3<T>{};
  auto gen     = std::mt19937{42};

  // loop over matrix sizes
  for (int n = 0; n < 10; ++n) {
    // generate base det_manip object
    std::vector<int> x_base(n), y_base(n);
    std::iota(x_base.begin(), x_base.end(), 0);
    y_base              = x_base;
    auto dm_base        = triqs::det_manip::det_manip{builder, x_base, y_base};
    auto const det_base = dm_base.determinant();

    // construct matrix builder arguments for the new rows/columns
    std::vector<int> x(k), y(k);
    std::iota(x.begin(), x.end(), 10);
    std::iota(y.begin(), y.end(), 10);

    // vector for insert positions
    std::vector<long> row_idxs(k), col_idxs(k);

    // loop over random insert positions
    for (int i = 0; i < 20; ++i) {
      // generate random insert positions
      std::ranges::sample(std::views::iota(0, n + k), row_idxs.begin(), k, gen);
      std::ranges::sample(std::views::iota(0, n + k), col_idxs.begin(), k, gen);
      std::ranges::sort(row_idxs);
      std::ranges::sort(col_idxs);

      // insert matrix builder arguments
      auto x_exp = x_base;
      auto y_exp = y_base;
      for (int j = 0; j < k; ++j) {
        x_exp.insert(x_exp.begin() + row_idxs[j], x[j]);
        y_exp.insert(y_exp.begin() + col_idxs[j], y[j]);
      }

      // construct expected det_manip object
      auto dm_exp        = triqs::det_manip::det_manip{builder, x_exp, y_exp};
      auto const det_exp = dm_exp.determinant();

      // try insert_k operation
      auto dm          = dm_base;
      auto const ratio = dm.try_insert_k(row_idxs, col_idxs, x, y);

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

TEST(TRIQSDetManip, InsertkDouble) {
  for (int i = 1; i < 5; ++i) test_insert<double>(i);
}

TEST(TRIQSDetManip, InsertkComplex) {
  for (int i = 1; i < 5; ++i) test_insert<std::complex<double>>(i);
}

MAKE_MAIN;
