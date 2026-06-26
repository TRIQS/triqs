// Copyright (c) 2014-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2014-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2022 Simons Foundation
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

/**
 * @file
 * @brief GoogleTest helpers for comparing Green's functions, tails and block Green's functions.
 */

#pragma once

#include "./arrays.hpp"
#include "../gfs/gf_tests.hpp"
#include "../gfs.hpp"
#include "../utility/exceptions.hpp"

using namespace triqs::gfs;
using namespace triqs;
using namespace triqs::arrays;
using namespace triqs::mesh;

/**
 * @addtogroup triqs-test-tools
 * @{
 */

/**
 * @brief Check that two Green's functions are element-wise close.
 *
 * @details Wraps triqs::gfs::assert_gfs_are_close and turns a thrown triqs::exception into a GoogleTest 
 * `AssertionFailure` carrying the exception message.
 *
 * @tparam X Type of the first Green's function.
 * @tparam Y Type of the second Green's function.
 * @param x First Green's function.
 * @param y Second Green's function.
 * @param precision Absolute tolerance used for the comparison.
 * @return `AssertionSuccess` if the Green's functions are close, `AssertionFailure` otherwise.
 */
template <typename X, typename Y>::testing::AssertionResult test_gfs_are_close(X const &x, Y const &y, double precision = 1.e-6) {
  try {
    assert_gfs_are_close(x, y, precision);
    return ::testing::AssertionSuccess();
  } catch (triqs::exception const &msg) { return ::testing::AssertionFailure() << msg.what(); }
}

/// Non-fatal GoogleTest assertion that two Green's functions are close (see @ref test_gfs_are_close).
#define EXPECT_GF_NEAR(X, ...) EXPECT_TRUE(test_gfs_are_close(X, __VA_ARGS__))

/**
 * @brief Check that the high-frequency tails of two Green's functions are element-wise close.
 *
 * @details Wraps triqs::gfs::assert_tails_are_close and turns a thrown triqs::exception into a GoogleTest 
 * `AssertionFailure` carrying the exception message.
 *
 * @tparam X Type of the first tail.
 * @tparam Y Type of the second tail.
 * @param x First tail.
 * @param y Second tail.
 * @param precision Absolute tolerance used for the comparison.
 * @return `AssertionSuccess` if the tails are close, `AssertionFailure` otherwise.
 */
template <typename X, typename Y>::testing::AssertionResult test_tails_are_close(X const &x, Y const &y, double precision = 1.e-6) {
  try {
    assert_tails_are_close(x, y, precision);
    return ::testing::AssertionSuccess();
  } catch (triqs::exception const &msg) { return ::testing::AssertionFailure() << msg.what(); }
}

/// Non-fatal GoogleTest assertion that two tails are close (see @ref test_tails_are_close).
#define EXPECT_TAIL_NEAR(X, ...) EXPECT_TRUE(test_tails_are_close(X, __VA_ARGS__))

/**
 * @brief Check that two block Green's functions are element-wise close.
 *
 * @details Wraps triqs::gfs::assert_block_gfs_are_close and turns a thrown triqs::exception into a GoogleTest 
 * `AssertionFailure` carrying the exception message.
 *
 * @tparam X Type of the first block Green's function.
 * @tparam Y Type of the second block Green's function.
 * @param x First block Green's function.
 * @param y Second block Green's function.
 * @param precision Absolute tolerance used for the comparison.
 * @return `AssertionSuccess` if the block Green's functions are close, `AssertionFailure` otherwise.
 */
template <typename X, typename Y>::testing::AssertionResult test_block_gfs_are_close(X const &x, Y const &y, double precision = 1.e-6) {
  try {
    assert_block_gfs_are_close(x, y, precision);
    return ::testing::AssertionSuccess();
  } catch (triqs::exception const &msg) { return ::testing::AssertionFailure() << msg.what(); }
}

/// Non-fatal GoogleTest assertion that two block Green's functions are close (see @ref test_block_gfs_are_close).
#define EXPECT_BLOCK_GF_NEAR(X, ...) EXPECT_TRUE(test_block_gfs_are_close(X, __VA_ARGS__))

/**
 * @brief Check that two block-of-block (Block2) Green's functions are element-wise close.
 *
 * @details Wraps triqs::gfs::assert_block2_gfs_are_close and turns a thrown triqs::exception into a GoogleTest 
 * `AssertionFailure` carrying the exception message.
 *
 * @tparam X Type of the first Block2 Green's function.
 * @tparam Y Type of the second Block2 Green's function.
 * @param x First Block2 Green's function.
 * @param y Second Block2 Green's function.
 * @param precision Absolute tolerance used for the comparison.
 * @return `AssertionSuccess` if the Block2 Green's functions are close, `AssertionFailure` otherwise.
 */
template <typename X, typename Y>::testing::AssertionResult test_block2_gfs_are_close(X const &x, Y const &y, double precision = 1.e-6) {
  try {
    assert_block2_gfs_are_close(x, y, precision);
    return ::testing::AssertionSuccess();
  } catch (triqs::exception const &msg) { return ::testing::AssertionFailure() << msg.what(); }
}

/// Non-fatal GoogleTest assertion that two Block2 Green's functions are close (see @ref test_block2_gfs_are_close).
#define EXPECT_BLOCK2_GF_NEAR(X, ...) EXPECT_TRUE(test_block2_gfs_are_close(X, __VA_ARGS__))

/** @} */
