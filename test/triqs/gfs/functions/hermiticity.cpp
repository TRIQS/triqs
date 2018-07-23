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
  double beta = 1;

  // ============ Test with hermitian Gf
  auto h = matrix<dcomplex>{{{1 + 0_j, 1_j}, {-1_j, 2 + 0_j}}};
  auto G = gf<imfreq>{{beta, Fermion}, {2, 2}};
  //G(iw_) << inverse(iw_ - h); // FIXME
  for (auto &iw : G.mesh()) G[iw] = inverse(iw - h);

  EXPECT_TRUE(is_gf_hermitian(G));

  // Check that Gf remains the same
  EXPECT_GF_NEAR(G, make_hermitian(G), 1e-15);

  // ============  Now Break hermiticity
  h = matrix<dcomplex>{{{1 + 0_j, 1_j}, {1_j, 2 + 0_j}}};
  //G(iw_) << inverse(iw_ - h); // FIXME
  for (auto &iw : G.mesh()) G[iw] = inverse(iw - h);

  EXPECT_FALSE(is_gf_hermitian(G));

  // Restore hermiticity
  EXPECT_TRUE(is_gf_hermitian(make_hermitian(G)));
}

MAKE_MAIN;
