// Copyright (c) 2024 Simons Foundation
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

#include <gtest/gtest.h>
#include <triqs/test_tools/gfs.hpp>
#include <triqs/gfs/dlr_fast_eval.hpp>

using namespace triqs::gfs;
using triqs::mesh::dlr_imtime;

constexpr nda::clef::placeholder<0> tau_;

using std::exp;
static auto onefermion(auto tau, double eps, double beta) { return -exp(-eps * tau) / (1 + exp(-beta * eps)); }

TEST(Gf, dlr_fast_eval) {
  double beta = 2.0, w_max = 5.0, eps = 1e-10, omega = 1.337;

  auto G = gf<dlr_imtime, scalar_valued>{{beta, Fermion, w_max, eps}};
  G[tau_] << onefermion(tau_, omega, beta);
  auto Gc = make_gf_dlr(G);

  dlr_fast_eval fast{Gc};
  for (double tau : {0.0, 0.13, 0.5, 1.0, 1.7, beta}) EXPECT_NEAR(std::abs(fast(tau) - Gc(tau)), 0.0, 1e-9);
}

MAKE_MAIN;
