// Copyright (c) 2015 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2015 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2020 Simons Foundation
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

#include <triqs/utility/real_or_complex.hpp>
#include <triqs/test_tools/arrays.hpp>

using triqs::utility::real_or_complex;

TEST(RealOrComplex, All) {

  auto x = real_or_complex(1);
  auto y = real_or_complex(3);

  {
    auto z = x + y;
    EXPECT_EQ(double(z), 4);
    EXPECT_EQ(z.is_real(), true);
  }

  {
    auto z = x + 1i;
    EXPECT_EQ(z.is_real(), false);
    EXPECT_THROW(double v = double(z), std::exception);
  }

  {
    auto z = 2 + x;
    EXPECT_EQ(double(z), 3);
  }
}
MAKE_MAIN;
