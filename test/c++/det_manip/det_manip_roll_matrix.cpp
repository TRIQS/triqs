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

// Rotate x- or y-vector depending on the roll direction.
template <typename D> void rotate_vec(std::vector<double> &x, std::vector<double> &y, typename D::RollDirection dir) {
  auto const size = static_cast<long>(x.size());
  double tmp      = 0.0;
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
  using dm_type = triqs::det_manip::det_manip<builder1<T>>;
  std::mt19937 gen(23432);
  std::uniform_real_distribution<> dis(0.0, 10.0);

  // loop over matrix sizes
  for (int n = 2; n < 3; ++n) {
    std::vector<double> x_args(n), y_args(n);
    std::ranges::generate(x_args, [&] { return dis(gen); });
    std::ranges::generate(y_args, [&] { return dis(gen); });
    auto dm = triqs::det_manip::det_manip{builder1<T>{}, x_args, y_args};
    auto M  = dm.matrix();

    // loop over roll directions
    for (auto dir : {dm_type::RollDirection::Down, dm_type::RollDirection::Up, dm_type::RollDirection::Right, dm_type::RollDirection::Left}) {
      // loop over the number of rows/columns
      for (int i = 0; i < n; ++i) {
        // generate expected det_manip object
        rotate_vec<dm_type>(x_args, y_args, dir);
        auto dm_exp = dm_type{builder1<T>{}, x_args, y_args};

        // roll operation
        dm.roll_matrix(dir);

        // check results
        EXPECT_ARRAY_NEAR(dm.matrix(), dm_exp.matrix(), 1.e-13);
        EXPECT_COMPLEX_NEAR(dm.determinant(), dm_exp.determinant(), 1.e-13);
        EXPECT_NO_THROW(dm.regenerate_and_check());
      }
      EXPECT_ARRAY_NEAR(dm.matrix(), M, 1.e-13);
    }
  }
}

TEST(TRIQSDetManip, RollMatrixDouble) { test_roll<double>(); }

TEST(TRIQSDetManip, RollMatrixComplex) { test_roll<std::complex<double>>(); }

MAKE_MAIN;
