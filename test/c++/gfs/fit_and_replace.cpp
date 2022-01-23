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
// Authors: Nils Wentzell

#include <triqs/test_tools/gfs.hpp>

TEST(Gf, RestrictedView) {

  triqs::clef::placeholder<0> iw_;
  double beta = 10;
  size_t N_iw    = 100;

  auto iw_mesh = mesh::imfreq{beta, Fermion, N_iw};
  auto G_iw    = gf<imfreq>{iw_mesh, {2, 2}};
  G_iw(iw_) << 1 / (iw_ - 1);

  int n_max    = 80;
  auto G_iw_rv = restricted_view(G_iw(), n_max);

  auto iw_mesh_small = mesh::imfreq{beta, Fermion, n_max};
  auto G_iw_exact    = gf<imfreq>{iw_mesh_small, {2, 2}};
  G_iw_exact(iw_) << 1 / (iw_ - 1);

  EXPECT_GF_NEAR(G_iw_rv, G_iw_exact);
}

TEST(Gf, FitAndReplace) {

  triqs::clef::placeholder<0> iw_;
  double beta = 10;
  size_t N_iw    = 100;

  auto iw_mesh  = mesh::imfreq{beta, Fermion, N_iw};
  auto G_iw     = gf<imfreq>{iw_mesh, {2, 2}};
  auto Sigma_iw = G_iw;

  Sigma_iw(iw_) << 1 / (iw_ - 2) + 3 / (iw_ + 3);
  G_iw(iw_) << 1 / (iw_ - 1 - Sigma_iw[iw_]);
  auto G_new_iw = G_iw;

  int n_min = 50;
  int n_max = 80;

  // Fill Green function with Garbage in the last part
  for (auto const &iw : iw_mesh)
    if (abs(iw.index()) > n_max) G_new_iw[iw] = iw * iw;

  array<dcomplex, 3> km(2, 2, 2);
  km()        = 0;
  km(1, 0, 0) = 1;
  km(1, 1, 1) = 1;

  auto [tail, err] = fit_tail_on_window(G_new_iw, n_min, n_max, km, 30, 9);

  replace_by_tail(G_new_iw(), tail, n_max);
  EXPECT_GF_NEAR(G_iw, G_new_iw, 1e-9);
}

MAKE_MAIN;
