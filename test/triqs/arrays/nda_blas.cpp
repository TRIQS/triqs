/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011 by O. Parcollet
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
#include "./nda_test_common.hpp"

#include <triqs/arrays/blas_lapack/gemm.hpp>
#include <triqs/arrays/blas_lapack/gemv.hpp>
#include <triqs/arrays/blas_lapack/ger.hpp>
#include <triqs/arrays/blas_lapack/axpy.hpp>

TEST(NDA, Blas1) {

  placeholder<0> i_;
  placeholder<1> j_;

  vector<dcomplex> V(5), V2(5);
  V[i_] << i_;
  V2() = -1;

  auto Vc = V;
  blas::axpy(2.0, V, V2);

  EXPECT_ARRAY_NEAR(V, Vc); // no change
  EXPECT_ARRAY_NEAR(V2, array<dcomplex, 1>{-1, 1, 3, 5, 7});
}

//----------------------------

TEST(NDA, Blas3R) {

  placeholder<0> i_;
  placeholder<1> j_;
  matrix<double> M1(2, 2), M2(2, 2), M3(2, 2);

  M1(i_, j_) << i_ + j_;
  M2().as_array_view() = 1;
  M3()                 = 0;

  blas::gemm(1.0, M1, M2, 1.0, M3);

  EXPECT_ARRAY_NEAR(M1, matrix<double>{{0, 1}, {1, 2}});
  EXPECT_ARRAY_NEAR(M2, matrix<double>{{1, 1}, {1, 1}});
  EXPECT_ARRAY_NEAR(M3, matrix<double>{{1, 1}, {3, 3}});
}

//----------------------------
TEST(NDA, Blas3R_f) {

  placeholder<0> i_;
  placeholder<1> j_;
  matrix<double> M1(2, 2, FORTRAN_LAYOUT), M2(2, 2, FORTRAN_LAYOUT), M3(2, 2, FORTRAN_LAYOUT);

  M1(i_, j_) << i_ + j_;
  M2().as_array_view() = 1;
  M3()                 = 0;

  blas::gemm(1.0, M1, M2, 1.0, M3);

  EXPECT_ARRAY_NEAR(M1, matrix<double>{{0, 1}, {1, 2}});
  EXPECT_ARRAY_NEAR(M2, matrix<double>{{1, 1}, {1, 1}});
  EXPECT_ARRAY_NEAR(M3, matrix<double>{{1, 1}, {3, 3}});
}

//----------------------------
TEST(NDA, Blas3C) {

  placeholder<0> i_;
  placeholder<1> j_;
  matrix<double> M1(2, 2), M2(2, 2), M3(2, 2);

  M1(i_, j_) << i_ + j_;
  M2().as_array_view() = 1;
  M3()                 = 0;

  blas::gemm(1.0, M1, M2, 1.0, M3);

  EXPECT_ARRAY_NEAR(M1, matrix<double>{{0, 1}, {1, 2}});
  EXPECT_ARRAY_NEAR(M2, matrix<double>{{1, 1}, {1, 1}});
  EXPECT_ARRAY_NEAR(M3, matrix<double>{{1, 1}, {3, 3}});
}

//----------------------------
TEST(NDA, Blas3Cf) {
  placeholder<0> i_;
  placeholder<1> j_;
  matrix<dcomplex> M1(2, 2, FORTRAN_LAYOUT), M2(2, 2, FORTRAN_LAYOUT), M3(2, 2, FORTRAN_LAYOUT);

  M1(i_, j_) << i_ + j_;
  M2().as_array_view() = 1;
  M3()                 = 0;

  blas::gemm(1.0, M1, M2, 1.0, M3);

  EXPECT_ARRAY_NEAR(M1, matrix<double>{{0, 1}, {1, 2}});
  EXPECT_ARRAY_NEAR(M2, matrix<double>{{1, 1}, {1, 1}});
  EXPECT_ARRAY_NEAR(M3, matrix<double>{{1, 1}, {3, 3}});
}

//----------------------------
TEST(NDA, Blas2) {
  placeholder<0> i_;
  matrix<double> M(2, 2, FORTRAN_LAYOUT);
  M() = 0;
  vector<double> V(2);
  V[i_] << i_ + 1;

  blas::ger(1.0, V, V, M);
  EXPECT_ARRAY_NEAR(M, matrix<double>{{1, 2}, {2, 4}});
}

//----------------------------

TEST(NDA, Blas3InvMat) {
  placeholder<0> i_;
  placeholder<1> j_;

  matrix<double> M(2, 2);
  M(i_, j_) << i_ + j_;
  vector<int> ipiv(2);
  lapack::getrf(M, ipiv);
  lapack::getri(M, ipiv);
  EXPECT_ARRAY_NEAR(M, matrix<double>{{-2, 1}, {1, 0}});
}

// ==================DEEP SWAP for vectors============================================

// FIXME Rename as BLAS_SWAP (swap of blas). Only for vector of same size
TEST(NDA, DeepSwap) {
  auto V = vector<double>{3, 3, 3};
  auto W = vector<double>{4, 4, 4};

  deep_swap(V, W);

  // V , W are swapped
  EXPECT_EQ(V, (vector<double>{4, 4, 4}));
  EXPECT_EQ(W, (vector<double>{3, 3, 3}));
}
// ----------------------------------

TEST(NDA, DeepSwapView) {
  auto V = vector<double>{3, 3, 3};
  auto W = vector<double>{4, 4, 4, 4};

  auto VV = V(range(0, 2));
  auto WW = W(range(0, 2));

  deep_swap(VV, WW);

  // VV, WW swapped
  EXPECT_EQ(WW, (vector<double>{3, 3}));
  EXPECT_EQ(VV, (vector<double>{4, 4}));
}

MAKE_MAIN;
