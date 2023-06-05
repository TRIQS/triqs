// Copyright (c) 2021 Simons Foundation
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

#include <triqs/test_tools/gfs.hpp>
#include <triqs/hilbert_space/fundamental_operator_set.hpp>

using triqs::hilbert_space::gf_struct_t;
using gf_struct_old_t = std::vector<std::pair<std::string, std::vector<std::variant<int, std::string>>>>;

TEST(H5, GfStruct_Bkwd) {
  auto gf_struct_old = gf_struct_old_t{{"up", {0, 1}}, {"dn", {0, 1}}};
  auto gf_struct_new = gf_struct_t{{"up", 2}, {"dn", 2}};

  {
    auto f = h5::file{"gf_struct_bkwd.h5", 'w'};
    h5::write(f, "gf_struct_old", gf_struct_old);
    h5::write(f, "gf_struct_new", gf_struct_old);
  }

  auto gf_struct_read_old = gf_struct_t{};
  auto gf_struct_read_new = gf_struct_t{};
  {
    auto f = h5::file{"gf_struct_bkwd.h5", 'r'};
    h5_read_gf_struct(f, "gf_struct_old", gf_struct_read_old);
    h5_read_gf_struct(f, "gf_struct_new", gf_struct_read_new);
  }

  EXPECT_EQ(gf_struct_read_old, gf_struct_new);
  EXPECT_EQ(gf_struct_read_new, gf_struct_new);
}

MAKE_MAIN;
