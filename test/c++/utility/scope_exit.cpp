// Copyright (c) 2016 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2016 Centre national de la recherche scientifique (CNRS)
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
// Authors: Olivier Parcollet

#include <triqs/test_tools/arrays.hpp>
#include <triqs/utility/scope_guard.hpp>

TEST(Scope, All) {

  int a = 0;

  {
    //auto __unused1 = triqs::utility::exec_at_scope_exit([&a]() { a=2;});
    SCOPE_EXIT { a = 2; };
  }

  EXPECT_EQ(a, 2);
}
MAKE_MAIN;
