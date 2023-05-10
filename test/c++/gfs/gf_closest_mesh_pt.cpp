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
using namespace triqs::lattice;

TEST(ClosestMeshPoint, ReTime) {
  double beta = 1;
  double tmax = 1;
  int N       = 12;

  auto g = gf<retime, scalar_valued>{{-tmax, tmax, N}};

  triqs::clef::placeholder<0> t_;
  g(t_) << 1.0 * t_;

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

TEST(ClosestMeshPoint, BrZone) {
  // use triangular lattice as reference
  int Nk     = 10;
  auto units = nda::matrix<double>{{1.0, 0.0}, {0.5, 0.5 * std::sqrt(3.0)}};
  auto bz    = brillouin_zone{bravais_lattice{units}};
  auto gk    = gf<brzone, scalar_real_valued>{{bz, Nk}};

  // initialize gk with nearest-neighbor tb dispersion
  triqs::clef::placeholder<1> k_;
  gk(k_) << -2.0 * (cos(k_(0)) + cos(0.5 * k_(0) + 0.5 * std::sqrt(3.0) * k_(1)) + cos(0.5 * k_(0) - 0.5 * std::sqrt(3.0) * k_(1)));

  // check if closest_mesh_pt(k) is k
  for (auto k : gk.mesh()) { EXPECT_CLOSE(gk[closest_mesh_pt(nda::vector<double>{k})], gk[k]); }

  // check if closest_mesh_pt(k) is the one with shortest euclidean distance for random k in mesh
  for (int rep = 0; rep < 100; ++rep) {
    nda::vector<double> v = {nda::rand() * (Nk - 1), nda::rand() * (Nk - 1), 0.0};
    nda::vector<double> q = transpose(gk.mesh().units()) * v;
    auto g_expec          = 0.0;
    auto dst              = std::numeric_limits<double>::infinity();

    for (auto k : gk.mesh()) {
      auto k_diff = make_regular(q - nda::vector<double>{k});
      auto dstp   = std::sqrt(nda::blas::dot(k_diff, k_diff)); // TODO: use nda::norm(k_diff, 2)

      if (dstp < dst) {
        dst     = dstp;
        g_expec = gk[k];
      }
    }

    EXPECT_CLOSE(gk[closest_mesh_pt(q)], g_expec);
  }
}
MAKE_MAIN;
