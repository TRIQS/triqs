// Copyright (c) 2022-2023 Simons Foundation
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

#include <triqs/test_tools/arrays.hpp>
#include <triqs/utility/legendre.hpp>

#include <boost/math/special_functions/legendre.hpp>
#include <nda/nda.hpp>

#include <vector>

TEST(TRIQSUtility, LegendreGenerator) {
  auto const x_values = std::vector<double>{-1.0, -0.5, -0.23, 0.0, 0.23, 0.5, 1.0};
  auto const l_max    = 20;

  // compare with stdlib implementation
  for (auto x : x_values) {
    auto gen = triqs::utility::legendre_generator{x};
    for (auto l : nda::range(l_max)) EXPECT_DOUBLE_EQ(gen.next(), boost::math::legendre_p(l, x));
  }
}

MAKE_MAIN;
