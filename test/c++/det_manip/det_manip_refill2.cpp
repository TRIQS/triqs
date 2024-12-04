// Copyright (c) 2016-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2016-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2021 Simons Foundation
// Copyright (c) 2016 Igor Krivenko
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
// Authors: Igor Krivenko, Olivier Parcollet, Nils Wentzell

#include <triqs/det_manip/det_manip.hpp>
#include <triqs/test_tools/arrays.hpp>

#include <nda/nda.hpp>

#include <numeric>
#include <vector>

// Matrix builder.
struct builder {
  double operator()(int x, int y) const {
    if ((x < 0) and (y < 0))
      return 0;
    else
      return 2 / double(x + y);
  }
};

// Construct a det_manip object with a given size.
auto init_dm(int size) {
  std::vector<int> x_args(size), y_args(size);
  std::iota(x_args.begin(), x_args.end(), 1);
  std::iota(y_args.begin(), y_args.end(), 1);
  return triqs::det_manip::det_manip{builder{}, x_args, y_args};
}

TEST(TRIQSDetManip, RefillSize3WithSize0) {
  // initialize det_manip
  auto dm = init_dm(3);
  EXPECT_CLOSE(1.0 / 5400, dm.determinant());

  // refill operation
  EXPECT_NEAR(5400, dm.try_refill(std::vector<int>{}, std::vector<int>{}), 1e-9);
  dm.complete_operation();

  // check results
  EXPECT_ARRAY_NEAR(nda::matrix<double>{}, dm.matrix());
  EXPECT_CLOSE(1, dm.determinant());
  EXPECT_ARRAY_NEAR(nda::matrix<double>{}, dm.inverse_matrix());
}

TEST(TRIQSDetManip, RefillSize3WithSize2) {
  // initialize det_manip
  auto dm = init_dm(3);
  EXPECT_CLOSE(1.0 / 5400, dm.determinant());

  // refill operation
  EXPECT_NEAR(300.0, dm.try_refill(std::vector<int>{1, 2}, std::vector<int>{1, 2}), 1e-10);
  dm.complete_operation();

  // check results
  auto F     = nda::matrix<double>{{1.0, 2.0 / 3}, {2.0 / 3, 0.5}};
  auto F_inv = nda::matrix<double>{{9, -12}, {-12, 18}};
  EXPECT_ARRAY_NEAR(F, dm.matrix());
  EXPECT_CLOSE(1.0 / 18, dm.determinant());
  EXPECT_ARRAY_NEAR(F_inv, dm.inverse_matrix());
}

TEST(TRIQSDetManip, RefillSize3WithSize4) {
  // initialize det_manip
  auto dm = init_dm(3);
  EXPECT_CLOSE(1.0 / 5400, dm.determinant());

  // refill operation
  EXPECT_NEAR(1.0 / 4900, dm.try_refill(std::vector<int>{1, 2, 3, 4}, std::vector<int>{1, 2, 3, 4}), 1e-10);
  dm.complete_operation();

  // check results
  auto F     = nda::matrix<double>{{1.0, 2.0 / 3, 1.0 / 2, 2.0 / 5},
                                   {2.0 / 3, 1.0 / 2, 2.0 / 5, 1.0 / 3},
                                   {1.0 / 2, 2.0 / 5, 1.0 / 3, 2.0 / 7},
                                   {2.0 / 5, 1.0 / 3, 2.0 / 7, 1.0 / 4}};
  auto F_inv = nda::matrix<double>{{100, -600, 1050, -560}, {-600, 4050, -7560, 4200}, {1050, -7560, 14700, -8400}, {-560, 4200, -8400, 4900}};
  EXPECT_ARRAY_NEAR(F, dm.matrix());
  EXPECT_CLOSE(1.0 / 26460000, dm.determinant());
  EXPECT_ARRAY_NEAR(F_inv, dm.inverse_matrix(), 1e-7);
}

TEST(TRIQSDetManip, RefillSize0WithSize2) {
  // initialize det_manip
  auto dm = init_dm(0);
  EXPECT_CLOSE(1.0, dm.determinant());

  // refill operation
  EXPECT_NEAR(1.0 / 18, dm.try_refill(std::vector<int>{1, 2}, std::vector<int>{1, 2}), 1e-10);
  dm.complete_operation();

  // check results
  auto F     = nda::matrix<double>{{1.0, 2.0 / 3}, {2.0 / 3, 0.5}};
  auto F_inv = nda::matrix<double>{{9, -12}, {-12, 18}};
  EXPECT_ARRAY_NEAR(F, dm.matrix());
  EXPECT_CLOSE(1.0 / 18, dm.determinant());
  EXPECT_ARRAY_NEAR(F_inv, dm.inverse_matrix());
}

MAKE_MAIN;
