// Copyright (c) 2026 Simons Foundation
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

// Tests for gf_evaluator<mesh::prod<Ms...>> specialization

#include <triqs/test_tools/gfs.hpp>
using namespace triqs::lattice;

// ----- K==N path: all dimensions discrete, direct pointer arithmetic -----

TEST(ProdEvaluator, AllDiscreteScalar) {
  double beta = 10;
  auto g      = gf<prod<imfreq, imfreq>, scalar_valued>{{{beta, Fermion, 5}, {beta, Fermion, 5}}};
  g()         = 0;
  for (auto [i, j] : g.mesh()) g[i, j] = i.data_index() * 10.0 + j.data_index();

  auto [m0, m1] = g.mesh();
  for (auto i : m0)
    for (auto j : m1) EXPECT_CLOSE(g(i, j), double(i.data_index() * 10 + j.data_index()));
}

// ----- K>0 path: leading discrete, trailing continuous -----

TEST(ProdEvaluator, MixedDiscreteContinuous) {
  double beta = 10;
  int n_cheb  = 20;
  auto bl     = bravais_lattice{nda::eye<double>(2)};
  auto m_r    = cyclat{bl, 4};
  auto m_cheb = chebyshev{beta, Fermion, n_cheb};
  auto g      = gf<prod<cyclat, chebyshev>, scalar_valued>{m_r * m_cheb};
  g()         = 1.0;

  // Evaluate: leading arg is mesh_point_t (resolvable), trailing is double (not resolvable)
  double tau = beta / 3.0;
  for (auto r : m_r) {
    auto val = g(r, tau);
    EXPECT_CLOSE(val, 1.0);
  }
}

// ----- eval_to_zero: out-of-range Matsubara frequency returns zero -----

TEST(ProdEvaluator, EvalToZeroScalar) {
  double beta = 1;
  auto g      = gf<prod<imfreq, imfreq>, scalar_valued>{{{beta, Fermion, 5}, {beta, Fermion, 5}}};
  g()         = 42.0;

  auto w_oob = matsubara_freq(1000, beta, Fermion);
  auto w_ok  = matsubara_freq(0, beta, Fermion);

  EXPECT_CLOSE(g(w_oob, w_oob), 0.0);
  EXPECT_CLOSE(g(w_oob, w_ok), 0.0);
  EXPECT_CLOSE(g(w_ok, w_oob), 0.0);
}

TEST(ProdEvaluator, EvalToZeroMatrix) {
  double beta = 1;
  auto g      = gf<prod<imfreq, imfreq>, matrix_valued>{{{beta, Fermion, 5}, {beta, Fermion, 5}}, {2, 2}};
  g()         = 42.0;

  auto w_oob = matsubara_freq(1000, beta, Fermion);
  auto w_ok  = matsubara_freq(0, beta, Fermion);
  auto zero  = nda::matrix<dcomplex>::zeros(2, 2);

  EXPECT_ARRAY_NEAR(g(w_oob, w_ok), zero);
  EXPECT_ARRAY_NEAR(g(w_ok, w_oob), zero);
}

// ----- Non-scalar target: eval_generic path -----

TEST(ProdEvaluator, NonScalarTarget) {
  double beta = 1;
  auto g      = gf<prod<imfreq, imfreq>, matrix_valued>{{{beta, Fermion, 5}, {beta, Fermion, 5}}, {2, 2}};
  g()         = 2.0;

  auto w0       = matsubara_freq(0, beta, Fermion);
  auto res      = g(w0, w0);
  auto expected = nda::matrix<dcomplex>(2, 2);
  expected()    = 2.0;
  EXPECT_ARRAY_NEAR(res, expected);
}

MAKE_MAIN;
