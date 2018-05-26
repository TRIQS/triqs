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
#include "start.hpp"
#include <triqs/arrays/blas_lapack/dot.hpp>

TEST(Array, Dot) {
  vector<double> a(2), aa(2), c(2);
  a() = 2.0;
  c() = 1;
  vector<int> b(2);
  b() = 3;
  aa  = 2 * a;

  EXPECT_DOUBLE_EQ(dot(a, b), 12);
  EXPECT_DOUBLE_EQ(dot(aa, a), 16);
  EXPECT_DOUBLE_EQ(dot(aa, b), 24);
  EXPECT_DOUBLE_EQ(dot(aa - a, b), 12);
}

TEST(Array, Dot2) {

  /// Added by I. Krivenko, #122
  /// test the complex version, specially with the zdotu workaround on Os X.
  vector<std::complex<double>> v(2);
  v(0) = 0;
  v(1) = {0, 1};

  EXPECT_COMPLEX_NEAR(blas::dot<false>(v, v), -1);
  EXPECT_COMPLEX_NEAR(blas::dot<true>(v, v), 1);
  EXPECT_COMPLEX_NEAR(dot(v, v), -1);
  EXPECT_COMPLEX_NEAR(dotc(v, v), 1);
}
MAKE_MAIN
