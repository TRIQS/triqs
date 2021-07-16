// Copyright (c) 2016-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2016-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2016 Igor Krivenko
// Copyright (c) 2018 Simons Foundation
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

#include <numeric>

#include <triqs/test_tools/arrays.hpp>
#include <triqs/arrays.hpp>
#include <triqs/det_manip/det_manip.hpp>
#include <triqs/det_manip/det_manip_basic.hpp>
#include <nda/linalg/det_and_inverse.hpp>

namespace arrays = nda;
using _vector    = nda::vector<double>;
using _matrix    = nda::matrix<double>;
using triqs::det_manip::det_manip_basic;

struct func {
  double operator()(int x, int y) const {
    if ((x < 0) and (y < 0))
      return 0;
    else
      return 2 / double(x + y);
  }
};

det_manip_basic<func> init_dm(int size) {
  func f;
  std::vector<int> initial_x(size), initial_y(size);
  std::iota(initial_x.begin(), initial_x.end(), 1);
  std::iota(initial_y.begin(), initial_y.end(), 1);
  det_manip_basic<func> dm(f, initial_x, initial_y);
  std::cerr << "matrix = " << dm.matrix() << std::endl;
  std::cerr << "det    = " << dm.determinant() << std::endl;
  return dm;
}

// ----- TESTS ------------------

TEST(det_manip, det_manip_refill_0) {
  auto dm = init_dm(3);
  EXPECT_CLOSE(1.0 / 5400, dm.determinant());

  EXPECT_NEAR(5400, dm.try_refill(_vector{}, _vector{}), 1e-9);
  dm.complete_operation();

  EXPECT_ARRAY_NEAR(_matrix{}, dm.matrix());
  EXPECT_CLOSE(1, dm.determinant());
  EXPECT_ARRAY_NEAR(_matrix{}, dm.inverse_matrix());
}

TEST(det_manip, det_manip_refill_2) {
  auto dm = init_dm(3);
  EXPECT_CLOSE(1.0 / 5400, dm.determinant());

  EXPECT_NEAR(300.0, dm.try_refill(_vector{1, 2}, _vector{1, 2}), 1e-10);
  dm.complete_operation();

  EXPECT_ARRAY_NEAR(_matrix{{1.0, 2.0 / 3}, {2.0 / 3, 0.5}}, dm.matrix());
  EXPECT_CLOSE(1.0 / 18, dm.determinant());
  EXPECT_ARRAY_NEAR(_matrix{{9, -12}, {-12, 18}}, dm.inverse_matrix());
}

TEST(det_manip, det_manip_refill_4) {
  auto dm = init_dm(3);
  EXPECT_CLOSE(1.0 / 5400, dm.determinant());

  EXPECT_NEAR(1.0 / 4900, dm.try_refill(_vector{1, 2, 3, 4}, _vector{1, 2, 3, 4}), 1e-10);
  dm.complete_operation();

  EXPECT_ARRAY_NEAR(_matrix{{1.0, 2.0 / 3, 1.0 / 2, 2.0 / 5},
                            {2.0 / 3, 1.0 / 2, 2.0 / 5, 1.0 / 3},
                            {1.0 / 2, 2.0 / 5, 1.0 / 3, 2.0 / 7},
                            {2.0 / 5, 1.0 / 3, 2.0 / 7, 1.0 / 4}},
                    dm.matrix());
  EXPECT_CLOSE(1.0 / 26460000, dm.determinant());
  EXPECT_ARRAY_NEAR(_matrix{{100, -600, 1050, -560}, {-600, 4050, -7560, 4200}, {1050, -7560, 14700, -8400}, {-560, 4200, -8400, 4900}},
                    dm.inverse_matrix(), 1e-7);
}

TEST(det_manip, det_manip_refill_empty) {
  auto dm = init_dm(0);
  EXPECT_CLOSE(1.0, dm.determinant());

  EXPECT_NEAR(1.0 / 18, dm.try_refill(_vector{1, 2}, _vector{1, 2}), 1e-10);
  dm.complete_operation();

  EXPECT_ARRAY_NEAR(_matrix{{1.0, 2.0 / 3}, {2.0 / 3, 0.5}}, dm.matrix());
  EXPECT_CLOSE(1.0 / 18, dm.determinant());
  EXPECT_ARRAY_NEAR(_matrix{{9, -12}, {-12, 18}}, dm.inverse_matrix());
}

TEST(det_manip, det_manip_roll_matrix) {
  auto dm = init_dm(2);

  EXPECT_ARRAY_NEAR(_matrix{{1.0, 2.0 / 3},
                            {2.0 / 3, 1.0 / 2}},
                    dm.matrix());

  double det_val = 1.0 / 2.0 - 4.0 / 9.0;
  EXPECT_CLOSE(det_val, dm.determinant());

  dm.roll_matrix(det_manip_basic<func>::Up);

  EXPECT_ARRAY_NEAR(_matrix{{2.0 / 3, 1.0 / 2},
                            {1.0, 2.0 / 3}},
                    dm.matrix());
  EXPECT_CLOSE(-det_val, dm.determinant());

  dm.roll_matrix(det_manip_basic<func>::Right);

  EXPECT_ARRAY_NEAR(_matrix{{1.0 / 2, 2.0 / 3},
                            {2.0 / 3, 1.0}},
                    dm.matrix());

  EXPECT_CLOSE(det_val, dm.determinant());
}

// ------------------------

MAKE_MAIN;
