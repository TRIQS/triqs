// Copyright (c) 2017-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2017-2018 Centre national de la recherche scientifique (CNRS)
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
// Authors: Nils Wentzell

/**
 * @file
 * @brief GoogleTest helper for comparing many-body operators.
 */

#pragma once

#include "./arrays.hpp"
#include "../operators/many_body_operator.hpp"

/**
 * @addtogroup triqs-test-tools
 * @{
 */

/**
 * @brief Check that two many-body operators are term-wise equal up to a given precision.
 *
 * @details Wraps triqs::operators::assert_operators_are_close and turns a thrown triqs::exception into a GoogleTest 
 * `AssertionFailure` carrying the exception message.
 *
 * @tparam X Type of the first operator.
 * @tparam Y Type of the second operator.
 * @param x First operator.
 * @param y Second operator.
 * @param precision Absolute tolerance used for the term-wise comparison.
 * @return `AssertionSuccess` if the operators are close, `AssertionFailure` otherwise.
 */
template <typename X, typename Y>::testing::AssertionResult test_operators_are_close(X const &x, Y const &y, double precision = 1e-6) {
  try {
    triqs::operators::assert_operators_are_close(x, y, precision);
    return ::testing::AssertionSuccess();
  } catch (triqs::exception const &msg) { return ::testing::AssertionFailure() << msg.what(); }
}

/// Non-fatal GoogleTest assertion that two many-body operators are close (see @ref test_operators_are_close).
#define EXPECT_OPERATOR_NEAR(X, ...) EXPECT_TRUE(test_operators_are_close(X, __VA_ARGS__))

/** @} */
