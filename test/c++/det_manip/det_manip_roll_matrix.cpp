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

// Rotate x- or y-vector depending on the roll direction.
template <typename D> void rotate_vec(std::vector<int> &x, std::vector<int> &y, typename D::RollDirection dir) {
  auto const size = static_cast<long>(x.size());
  int tmp         = 0;
  switch (dir) {
    case (D::RollDirection::None): return;
    case (D::RollDirection::Down):
      tmp = x.back();
      for (long i = size - 2; i >= 0; --i) x[i + 1] = x[i];
      x[0] = tmp;
      break;
    case (D::RollDirection::Up):
      tmp = x[0];
      for (auto i = 0; i < size - 1; ++i) x[i] = x[i + 1];
      x.back() = tmp;
      break;
    case (D::RollDirection::Right):
      tmp = y.back();
      for (auto i = size - 2; i >= 0; --i) y[i + 1] = y[i];
      y[0] = tmp;
      break;
    case (D::RollDirection::Left):
      tmp = y[0];
      for (auto i = 0; i < size - 1; ++i) y[i] = y[i + 1];
      y.back() = tmp;
      break;
    default: TRIQS_RUNTIME_ERROR;
  }
}

// Test the roll matrix operation of det_manip.
template <typename T> void test_roll() {
  using dm_type = triqs::det_manip::det_manip<builder3<T>>;
  auto builder  = builder3<T>{};

  // loop over matrix sizes
  for (int n = 2; n < 10; ++n) {
    // generate det_manip object
    std::vector<int> x(n), y(n);
    std::iota(x.begin(), x.end(), 0);
    y       = x;
    auto dm = triqs::det_manip::det_manip{builder, x, y};
    auto M  = dm.matrix();

    // loop over roll directions
    for (auto dir : {dm_type::RollDirection::Down, dm_type::RollDirection::Up, dm_type::RollDirection::Right, dm_type::RollDirection::Left}) {
      // loop over the number of rows/columns
      for (int i = 0; i < n; ++i) {
        // generate expected det_manip object
        rotate_vec<dm_type>(x, y, dir);
        auto dm_exp = dm_type{builder, x, y};

        // roll operation
        dm.roll_matrix(dir);

        // check results
        EXPECT_ARRAY_NEAR(dm.matrix(), dm_exp.matrix(), 1.e-10);
        EXPECT_COMPLEX_NEAR(dm.determinant(), dm_exp.determinant(), 1.e-10);
        EXPECT_NO_THROW(dm.regenerate_and_check());
      }
      EXPECT_ARRAY_NEAR(dm.matrix(), M, 1.e-10);
    }
  }
}

TEST(TRIQSDetManip, RollMatrixDouble) { test_roll<double>(); }

TEST(TRIQSDetManip, RollMatrixComplex) { test_roll<std::complex<double>>(); }

MAKE_MAIN;
