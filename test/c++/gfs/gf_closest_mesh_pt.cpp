// Copyright (c) 2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2018 Centre national de la recherche scientifique (CNRS)
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
// Authors: Olivier Parcollet

#include <triqs/test_tools/gfs.hpp>

TEST(Gf, ClosestMeshPoint) {

  double beta = 1;
  double tmax = 1;
  int N       = 12;

  auto g = gf<retime, scalar_valued>{{-tmax, tmax, N}};

  triqs::clef::placeholder<0> t_;
  g(t_) << 1.0 * t_;

  //
  for (auto const &t : g.mesh()) std::cerr << double(t) << std::endl;

  EXPECT_CLOSE(g[closest_mesh_pt(0.1)], 1.0 / (N - 1));
  EXPECT_CLOSE(g[closest_mesh_pt(-0.1)], -1.0 / (N - 1));

  EXPECT_CLOSE(g[closest_mesh_pt(0.00001)], 1.0 / (N - 1));
  EXPECT_CLOSE(g[closest_mesh_pt(-0.00001)], -1.0 / (N - 1));

  EXPECT_CLOSE(g[closest_mesh_pt(0.2)], 3.0 / (N - 1));
  EXPECT_CLOSE(g[closest_mesh_pt(-0.2)], -3.0 / (N - 1));

  EXPECT_CLOSE(g[closest_mesh_pt(0.3)], 3.0 / (N - 1));
  EXPECT_CLOSE(g[closest_mesh_pt(-0.3)], -3.0 / (N - 1));

  EXPECT_CLOSE(g[closest_mesh_pt(0.4)], 5.0 / (N - 1));
  EXPECT_CLOSE(g[closest_mesh_pt(-0.4)], -5.0 / (N - 1));
}
MAKE_MAIN;
