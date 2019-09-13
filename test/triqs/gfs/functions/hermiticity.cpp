/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2018 by N. Wentzell
 *
 * TRIQS is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * TRIQS. If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#include <triqs/test_tools/gfs.hpp>

using namespace triqs::gfs;
using namespace triqs::arrays;

TEST(hermiticity, symmetrize_and_check) {

  triqs::clef::placeholder<0> iw_;
  triqs::clef::placeholder<1> i_;
  triqs::clef::placeholder<2> j_;
  triqs::clef::placeholder<3> k_;
  triqs::clef::placeholder<4> l_;
  double beta = 1;

  // ============ Test with hermitian Gf
  auto h = matrix<dcomplex>{{{1 + 0_j, 1_j}, {-1_j, 2 + 0_j}}};
  auto G = gf<imfreq>{{beta, Fermion}, {2, 2}};
  //G(iw_) << inverse(iw_ - h); // FIXME
  for (auto &iw : G.mesh()) G[iw] = inverse(iw - h);

  EXPECT_TRUE(is_gf_hermitian(G));

  // Check that Gf remains the same
  EXPECT_GF_NEAR(G, make_hermitian(G), 1e-15);

  // Check that the same for the imtime Gf
  auto G_tau = make_gf_from_fourier(G);
  EXPECT_TRUE(is_gf_hermitian(G_tau));
  EXPECT_GF_NEAR(G_tau, make_hermitian(G_tau), 1e-14);

  // ============  Now Break hermiticity
  h = matrix<dcomplex>{{{1 + 0_j, 1_j}, {1_j, 2 + 0_j}}};
  for (auto &iw : G.mesh()) G[iw] = inverse(iw - h);

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
  EXPECT_GF_NEAR(Chi_tau, make_hermitian(Chi_tau), 1e-14);
}

MAKE_MAIN;
