// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2023 Simons Foundation
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
// Authors: Henri Menke, Olivier Parcollet, Nils Wentzell

#include <triqs/det_manip/det_manip.hpp>
#include <triqs/det_manip/utils.hpp>
#include <triqs/test_tools/arrays.hpp>

#include <vector>

struct builder {
  double operator()(int i, int j) const { return (i == j ? 1 + j : 0); }
};

TEST(TRIQSDetManip, DetManipConstructWithMatrixBuilderAndCapacity) {
  using namespace triqs::det_manip;

  // create an empty det_manip object
  det_manip dm{builder{}, 10};
  EXPECT_EQ(dm.size(), 0);
  EXPECT_EQ(dm.determinant(), 1);
  EXPECT_EQ(dm.capacity(), 10);

  // try to reserve memory for 5 elements --> should do nothing
  dm.reserve(5);
  EXPECT_EQ(dm.capacity(), 10);

  // reserve memory for 50 elements
  dm.reserve(50);
  EXPECT_EQ(dm.capacity(), 50);

  // clear the data
  dm.clear();
  EXPECT_EQ(dm.size(), 0);
  EXPECT_EQ(dm.determinant(), 1);
  EXPECT_EQ(dm.capacity(), 50);

  // check the data
  EXPECT_NO_THROW(dm.regenerate_and_check());
}

TEST(TRIQSDetManip, DetManipConstructWithMatrixBuilderAndRanges) {
  using namespace triqs::det_manip;
  std::vector<int> x_args{0, 1, 2, 3, 4};
  auto y_args  = x_args;
  auto exp_mat = nda::matrix<double>(5, 5);
  nda::for_each(exp_mat.shape(), [&](auto i, auto j) { exp_mat(i, j) = builder{}(i, j); });
  auto exp_inv_mat = nda::inverse(exp_mat);
  auto exp_det     = nda::determinant(exp_mat);
  auto check       = [&](auto const &dm, auto exp_size, auto exp_cap) {
    EXPECT_EQ(dm.size(), exp_size);
    EXPECT_EQ(dm.capacity(), exp_cap);
    EXPECT_DOUBLE_EQ(exp_det, dm.determinant());
    EXPECT_ARRAY_NEAR(exp_mat, dm.matrix());
    EXPECT_ARRAY_NEAR(exp_inv_mat, dm.inverse_matrix());
    EXPECT_ARRAY_NEAR(exp_inv_mat, dm.inverse_matrix_internal_order());
    for (int i = 0; i < exp_size; ++i) {
      EXPECT_EQ(dm.get_x(i), x_args[i]);
      EXPECT_EQ(dm.get_y(i), y_args[i]);
      EXPECT_EQ(dm.get_x_internal_order()[i], x_args[i]);
      EXPECT_EQ(dm.get_y_internal_order()[i], y_args[i]);
    }
  };

  // create a det_manip object with a matrix builder and argument ranges
  det_manip dm{builder{}, x_args, y_args};
  check(dm, 5, 10);

  // try to reserve memory for 5 elements --> should do nothing
  dm.reserve(5);
  check(dm, 5, 10);

  // reserve memory for 20 elements
  dm.reserve(20);
  check(dm, 5, 20);

  // check the data
  EXPECT_NO_THROW(dm.regenerate_and_check());

  // print a warning
  dm.set_precision_warning(-1);
  EXPECT_NO_THROW(dm.regenerate_and_check());

  // clear the data
  dm.clear();
  EXPECT_EQ(dm.size(), 0);
  EXPECT_EQ(dm.determinant(), 1);
  EXPECT_EQ(dm.capacity(), 20);
  EXPECT_NO_THROW(dm.regenerate_and_check());
}

TEST(TRIQSDetManip, DetManipHDF5) {
  using namespace triqs::det_manip;
  std::vector<int> x_args{0, 1, 2, 3, 4};
  auto y_args = x_args;

  // create a det_manip object with a matrix builder and argument ranges
  det_manip dm{builder{}, x_args, y_args};

  // write to HDF5
  h5::file file("det_manip.h5", 'w');
  h5::write(file, "det", dm);

  // read from HDF5
  det_manip dm_r{builder{}, 1};
  h5::read(file, "det", dm_r);

  // check the read object
  EXPECT_EQ(dm_r.size(), dm.size());
  EXPECT_EQ(dm_r.capacity(), dm.capacity());
  EXPECT_EQ(dm_r.determinant(), dm.determinant());
  EXPECT_ARRAY_EQ(dm_r.matrix(), dm.matrix());
  EXPECT_ARRAY_EQ(dm_r.inverse_matrix(), dm.inverse_matrix());
  EXPECT_ARRAY_EQ(dm_r.inverse_matrix_internal_order(), dm.inverse_matrix_internal_order());
  EXPECT_EQ(dm_r.get_x_internal_order(), dm.get_x_internal_order());
  EXPECT_EQ(dm_r.get_y_internal_order(), dm.get_y_internal_order());
  EXPECT_NO_THROW(dm_r.regenerate_and_check());
}

MAKE_MAIN
