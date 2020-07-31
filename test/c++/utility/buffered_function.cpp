// Copyright (c) 2014-2016 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2014-2016 Centre national de la recherche scientifique (CNRS)
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
// Authors: Michel Ferrero, Olivier Parcollet

#include <triqs/utility/buffered_function.hpp>
#include <triqs/test_tools/arrays.hpp>

TEST(BufferedFunction, All) {
  auto f = [x = 0]() mutable {
    auto res = x * x;
    x++;
    return res;
  };
  auto gen = triqs::utility::buffered_function<double>(f, 5);
  for (int u = 0; u < 22; ++u) EXPECT_EQ(gen(), u * u);
}
MAKE_MAIN;
