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

#pragma once
#include "./arrays.hpp"
#include <triqs/operators/many_body_operator.hpp>

// Test that two operators are term-wise equal up to a precision
template <typename X, typename Y>::testing::AssertionResult test_operators_are_close(X const &x, Y const &y, double precision = 1e-6) {
  try {
    assert_operators_are_close(x, y, precision);
    return ::testing::AssertionSuccess();
  } catch (triqs::exception const &msg) { return ::testing::AssertionFailure() << msg.what(); }
}

#define EXPECT_OPERATOR_NEAR(X, ...) EXPECT_TRUE(test_operators_are_close(X, __VA_ARGS__))
