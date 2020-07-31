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

using namespace triqs::gfs;

int ntau         = 5;
double beta      = 1.2345;
double precision = 1.e-12;

auto g       = gf<imtime, scalar_valued>{{beta, Fermion, ntau}};
auto chi_ref = gf<imtime, scalar_valued>{{beta, Boson, ntau}};
auto chi     = gf<imtime, scalar_valued>{{beta, Boson, ntau}};

// ----------------------------------------------------

TEST(Gf, LocalChi) {

  for (auto tau : g.mesh()) g[tau] = 1.;

  for (auto tau : g.mesh()) chi[tau] = g(tau) * g(beta - tau);

  for (auto tau : chi_ref.mesh()) chi_ref[tau] = 1.;
  EXPECT_ARRAY_NEAR(chi.data(), chi_ref.data());
}

TEST(Gf, LocalChi2) {
  for (auto tau : g.mesh()) g[tau] = std::exp(-2 * tau);

  for (auto tau : g.mesh()) chi[tau] = g(tau) * g(beta - tau);

  EXPECT_NEAR(real(chi(0)), std::exp(-2 * beta), precision);
  EXPECT_NEAR(real(chi(beta * 0.45)), std::exp(-2 * beta), precision);
  EXPECT_NEAR(real(chi(beta)), std::exp(-2 * beta), precision);
}

TEST(Gf, GtauEvalBoundary) {

  for (auto tau : g.mesh()) g[tau] = std::exp(-2 * tau);

  EXPECT_NEAR(real(g(0)), 1, precision);
  EXPECT_NEAR(real(g(beta)), std::exp(-2 * beta), precision);
}

MAKE_MAIN;
