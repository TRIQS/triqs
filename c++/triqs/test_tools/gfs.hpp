// Copyright (c) 2014-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2014-2018 Centre national de la recherche scientifique (CNRS)
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
// Authors: Michel Ferrero, Olivier Parcollet, Nils Wentzell

#pragma once
#include "./arrays.hpp"
#include <triqs/gfs.hpp>
#include <triqs/gfs/gf_tests.hpp>

using namespace triqs::gfs;
using namespace triqs;
using namespace triqs::arrays;
using namespace triqs::mesh;

//
template <typename X, typename Y>::testing::AssertionResult test_gfs_are_close(X const &x, Y const &y, double precision = 1.e-6) {

  try {
    assert_gfs_are_close(x, y, precision);
    return ::testing::AssertionSuccess();
  } catch (triqs::exception const &msg) { return ::testing::AssertionFailure() << msg.what(); }
}

#define EXPECT_GF_NEAR(X, ...) EXPECT_TRUE(test_gfs_are_close(X, __VA_ARGS__))

template <typename X, typename Y>::testing::AssertionResult test_tails_are_close(X const &x, Y const &y, double precision = 1.e-6) {

  try {
    assert_tails_are_close(x, y, precision);
    return ::testing::AssertionSuccess();
  } catch (triqs::exception const &msg) { return ::testing::AssertionFailure() << msg.what(); }
}

#define EXPECT_TAIL_NEAR(X, ...) EXPECT_TRUE(test_tails_are_close(X, __VA_ARGS__))
//
template <typename X, typename Y>::testing::AssertionResult test_block_gfs_are_close(X const &x, Y const &y, double precision = 1.e-6) {

  try {
    assert_block_gfs_are_close(x, y, precision);
    return ::testing::AssertionSuccess();
  } catch (triqs::exception const &msg) { return ::testing::AssertionFailure() << msg.what(); }
}

#define EXPECT_BLOCK_GF_NEAR(X, ...) EXPECT_TRUE(test_block_gfs_are_close(X, __VA_ARGS__))

template <typename X, typename Y>::testing::AssertionResult test_block2_gfs_are_close(X const &x, Y const &y, double precision = 1.e-6) {

  try {
    assert_block2_gfs_are_close(x, y, precision);
    return ::testing::AssertionSuccess();
  } catch (triqs::exception const &msg) { return ::testing::AssertionFailure() << msg.what(); }
}

#define EXPECT_BLOCK2_GF_NEAR(X, ...) EXPECT_TRUE(test_block2_gfs_are_close(X, __VA_ARGS__))
