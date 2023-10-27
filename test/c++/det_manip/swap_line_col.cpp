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

#include <triqs/test_tools/arrays.hpp>
#include <triqs/det_manip/det_manip.hpp>
#include <random>
#include <iostream>
#include <nda/linalg/det_and_inverse.hpp>
#include "./old_test_tool.hpp"
#include <triqs/test_tools/arrays.hpp>

struct fun {

  using result_type   = double;
  using argument_type = double;

  double operator()(double x, double y) const {
    const double pi   = acos(-1);
    const double beta = 10.0;
    const double epsi = 0.1;
    double tau        = x - y;
    bool s            = (tau > 0);
    tau               = (s ? tau : beta + tau);
    double r          = epsi + tau / beta * (1 - 2 * epsi);
    return -2 * (pi / beta) / std::sin(pi * r);
  }
};

using d_t              = triqs::det_manip::det_manip<fun>;
// const double precision = 1.e-6;
const int seed         = 23432;

TEST(DetManip, SwapRow) {

  for (int N = 1; N < 9; N++) {
    std::cerr << "================================\n N = " << N << std::endl;

    for (int i0 = 0; i0 < N; i0++)
      for (int j0 = 0; j0 < N; j0++) {

        std::cerr << "------------------------------\n i0 = " << i0 << "j0 = " << j0 << std::endl;

        std::mt19937 gen(seed);
        std::uniform_real_distribution<> dis(0.0, 10.0);

        std::vector<double> X(N), Y(N);
        for (int n = 0; n < N; ++n) {
          X[n] = dis(gen);
          Y[n] = dis(gen);
        }

        auto d = d_t{fun{}, X, Y};

        // result
        std::swap(X[i0], X[j0]);
        auto d2 = d_t{fun{}, X, Y};

        // ops
        d.swap_row(i0, j0);

        EXPECT_ARRAY_NEAR(d.matrix(), d2.matrix(), 1.e-13);
        EXPECT_NEAR(d.determinant(), d2.determinant(), 1.e-13);

        //nda::assert_all_close(d.matrix(), d2.matrix(), precision, true);
      }
  }
}

TEST(DetManip, SwapCol) {

  for (int N = 1; N < 9; N++) {
    std::cerr << "================================\n N = " << N << std::endl;

    for (int i0 = 0; i0 < N; i0++)
      for (int j0 = 0; j0 < N; j0++) {

        std::cerr << "------------------------------\n i0 = " << i0 << "j0 = " << j0 << std::endl;

        std::mt19937 gen(seed);
        std::uniform_real_distribution<> dis(0.0, 10.0);

        std::vector<double> X(N), Y(N);
        for (int n = 0; n < N; ++n) {
          X[n] = dis(gen);
          Y[n] = dis(gen);
        }

        auto d = d_t{fun{}, X, Y};

        // result
        std::swap(Y[i0], Y[j0]);
        auto d2 = d_t{fun{}, X, Y};

        // ops
        d.swap_col(i0, j0);

        EXPECT_ARRAY_NEAR(d.matrix(), d2.matrix(), 1.e-13);
        EXPECT_NEAR(d.determinant(), d2.determinant(), 1.e-13);

        //nda::assert_all_close(d.matrix(), d2.matrix(), precision, true);
      }
  }
}

MAKE_MAIN;
