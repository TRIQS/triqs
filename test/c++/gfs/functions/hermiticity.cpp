// Copyright (c) 2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2018 Centre national de la recherche scientifique (CNRS)
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
// Authors: Nils Wentzell

#include <triqs/test_tools/gfs.hpp>

using namespace triqs::gfs;
using namespace nda;

TEST(hermiticity, symmetrize_and_check) {

  triqs::clef::placeholder<0> iw_;
  triqs::clef::placeholder<1> i_;
  triqs::clef::placeholder<2> j_;
  triqs::clef::placeholder<3> k_;
  triqs::clef::placeholder<4> l_;
  double beta = 1;

  // ============ Test with hermitian Gf
  auto h = matrix<dcomplex>{{{1 + 0i, 1i}, {-1i, 2 + 0i}}};
  auto G = gf<imfreq>{{beta, Fermion}, {2, 2}};
  //G(iw_) << inverse(iw_ - h); // FIXME
  for (auto const &iw : G.mesh()) G[iw] = inverse(iw - h);

  EXPECT_TRUE(is_gf_hermitian(G));

  // Check that Gf remains the same
  EXPECT_GF_NEAR(G, make_hermitian(G), 1e-15);

  // Check that the same for the imtime Gf
  auto G_tau = make_gf_from_fourier(G);
  EXPECT_TRUE(is_gf_hermitian(G_tau));
  EXPECT_GF_NEAR(G_tau, make_hermitian(G_tau), 1e-12);

  // ============  Now Break hermiticity
  h = matrix<dcomplex>{{{1 + 0i, 1i}, {1i, 2 + 0i}}};
  for (auto const &iw : G.mesh()) G[iw] = inverse(iw - h);

  EXPECT_FALSE(is_gf_hermitian(G));

  // Restore hermiticity
  EXPECT_TRUE(is_gf_hermitian(make_hermitian(G)));

  // Check that the same for the imtime Gf
  G_tau = make_gf_from_fourier(G);
  EXPECT_FALSE(is_gf_hermitian(G_tau));
  EXPECT_TRUE(is_gf_hermitian(make_hermitian(G_tau)));

  // ============ TensorValued 4 Case
  auto Chi = gf<imfreq, tensor_valued<4>>{{beta, Fermion}, {2, 2, 2, 2}};
  using namespace std::complex_literals;
  Chi(iw_)(i_, j_, k_, l_) << 1.0 / (iw_ - (1 + 1i) * i_ * j_ - (1 - 1i) * k_ * l_);
  EXPECT_TRUE(is_gf_hermitian(Chi));

  // Check that Gf remains the same
  EXPECT_GF_NEAR(Chi, make_hermitian(Chi), 1e-15);

  // Check that the same for the imtime Gf
  auto Chi_tau = make_gf_from_fourier(Chi);
  EXPECT_TRUE(is_gf_hermitian(Chi_tau));
  EXPECT_GF_NEAR(Chi_tau, make_hermitian(Chi_tau), 1e-12);
}

TEST(real_in_tau, symmetrize_and_check) {

  triqs::clef::placeholder<0> iw_;
  triqs::clef::placeholder<1> i_;
  triqs::clef::placeholder<2> j_;
  double beta = 1;

  // ============ Test with hermitian Gf
  auto h = matrix<dcomplex>{{{1 + 0i, 0.1}, {0.1, 2 + 0i}}};
  auto G = gf<imfreq>{{beta, Fermion}, {2, 2}};
  for (auto const &iw : G.mesh()) G[iw] = inverse(iw - h);

  EXPECT_TRUE(is_gf_real_in_tau(G));

  // Check that Gf remains the same
  EXPECT_GF_NEAR(G, make_real_in_tau(G), 1e-15);

  // ============  Now with cplx-valued Hamiltonian
  h = matrix<dcomplex>{{{1 + 0i, 1i}, {-1i, 2 + 0i}}};
  for (auto const &iw : G.mesh()) G[iw] = inverse(iw - h);

  EXPECT_FALSE(is_gf_real_in_tau(G));

  // Restore hermiticity
  EXPECT_TRUE(is_gf_real_in_tau(make_real_in_tau(G)));
}

MAKE_MAIN;
