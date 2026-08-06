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

// Test the swap_row and swap_col operations of det_manip.
template <typename T, bool do_row> void test_swap() {
  auto builder = builder3<T>{};

  // loop over matrix sizes
  for (int n = 1; n < 10; ++n) {
    // generate base det_manip object
    std::vector<int> x(n), y(n);
    std::iota(x.begin(), x.end(), 0);
    y       = x;
    auto dm = triqs::det_manip::det_manip{builder, x, y};
    auto M  = dm.matrix();

    // loop over all row/column pairs
    for (int i = 0; i < n; ++i) {
      for (int j = 0; j < n; ++j) {
        // swap rows/columns
        if constexpr (do_row) {
          deep_swap(M(i, nda::range::all), M(j, nda::range::all));
          dm.swap_row(i, j);
        } else {
          deep_swap(M(nda::range::all, i), M(nda::range::all, j));
          dm.swap_col(i, j);
        }

        // check results
        EXPECT_ARRAY_NEAR(dm.matrix(), M, 1.e-10);
        EXPECT_COMPLEX_NEAR(dm.determinant(), nda::linalg::det(M), 1.e-10);
        EXPECT_NO_THROW(dm.regenerate_and_check());
      }
    }
  }
}

TEST(TRIQSDetManip, SwapRowDouble) { test_swap<double, true>(); }

TEST(TRIQSDetManip, SwapRowComplex) { test_swap<std::complex<double>, true>(); }

TEST(TRIQSDetManip, SwapColumnDouble) { test_swap<double, false>(); }

TEST(TRIQSDetManip, SwapColumnComplex) { test_swap<std::complex<double>, false>(); }

MAKE_MAIN;
