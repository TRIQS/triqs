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

#include <fmt/ranges.h>
#include <nda/nda.hpp>

#include <algorithm>
#include <complex>
#include <random>
#include <vector>

// Test the remove_k operation of det_manip.
template <typename T> void test_remove_k(int k) {
  std::mt19937 gen(23432);
  std::uniform_real_distribution<> dis(0.0, 10.0);

  // loop over matrix sizes
  for (int n = k; n < 9; ++n) {
    // generate base det_manip object and its matrix builder arguments
    std::vector<double> x_base(n), y_base(n);
    std::ranges::generate(x_base, [&] { return dis(gen); });
    std::ranges::generate(y_base, [&] { return dis(gen); });
    auto dm_base = triqs::det_manip::det_manip{builder1<T>{}, x_base, y_base};

    // perform some random swap operations
    auto int_dist = std::uniform_int_distribution<>{0, n - 1};
    for (int i = 0; i < 50; ++i) {
      dm_base.swap_col(int_dist(gen), int_dist(gen));
      dm_base.swap_row(int_dist(gen), int_dist(gen));
    }
    auto const det_base = dm_base.determinant();

    // vector for remove positions
    std::vector<long> row_idxs(k), col_idxs(k);

    // loop over random remove positions
    for (int i = 0; i < 100; ++i) {
      // generate random remove positions
      std::ranges::sample(std::views::iota(0, n), row_idxs.begin(), k, gen);
      std::ranges::sample(std::views::iota(0, n), col_idxs.begin(), k, gen);
      std::ranges::sort(row_idxs);
      std::ranges::sort(col_idxs);

      // remove matrix builder arguments
      std::vector<double> x_exp = dm_base.get_x();
      std::vector<double> y_exp = dm_base.get_y();
      for (int j = 0; j < k; ++j) {
        x_exp.erase(x_exp.begin() + row_idxs[j] - j);
        y_exp.erase(y_exp.begin() + col_idxs[j] - j);
      }

      // construct expected det_manip object
      auto dm_exp        = triqs::det_manip::det_manip{builder1<T>{}, x_exp, y_exp};
      auto const det_exp = dm_exp.determinant();

      // try remove_k operation
      auto dm          = dm_base;
      auto const ratio = dm.try_remove_k(row_idxs, col_idxs);

      // complete operation
      dm.complete_operation();

      // check results
      using triqs::det_manip::detail::rel_diff;
      EXPECT_LT(rel_diff(ratio, dm_exp.determinant() / det_base), 1.e-6);
      EXPECT_LT(rel_diff(dm.matrix(), dm_exp.matrix()), 1.e-6);
      EXPECT_LT(rel_diff(dm.inverse_matrix(), dm_exp.inverse_matrix()), 1.e-6);
      EXPECT_LT(rel_diff(dm.determinant(), det_exp), 1.e-6);
      EXPECT_NO_THROW(dm.regenerate_and_check());
    }
  }
}

TEST(TRIQSDetManip, RemoveKDouble) {
  for (int i = 1; i < 3; ++i) test_remove_k<double>(i);
}

TEST(TRIQSDetManip, RemoveKComplex) {
  for (int i = 1; i < 3; ++i) test_remove_k<std::complex<double>>(i);
}

MAKE_MAIN;
