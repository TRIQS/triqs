// Copyright (c) 2014-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2014-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2015 Igor Krivenko
// Copyright (c) 2018-2020 Simons Foundation
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
// Authors: Michel Ferrero, Igor Krivenko, Olivier Parcollet, Nils Wentzell

#include <triqs/test_tools/arrays.hpp>
#include <triqs/arrays.hpp>
#if not H5_VERSION_GE(1, 8, 9)
int main() {}

#else

#include <h5/serialization.hpp>
using h5::deserialize;
using h5::serialize;
using namespace triqs::arrays;

TEST(H5Serialize, All) {

  auto a = array<double, 1>{1, 2, 3, 4, 5};
  auto s = serialize(a);
  auto b = deserialize<array<double, 1>>(s);
  EXPECT_ARRAY_NEAR(a, b);

  auto s2 = serialize(array<double, 1>(10000));
  auto s3 = serialize(array<double, 1>(20000));
  EXPECT_TRUE(first_dim(s3) < first_dim(s2) * 2); // Test scaling in size with overhead
}
MAKE_MAIN;

#endif
