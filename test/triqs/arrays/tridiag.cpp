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
#include <triqs/test_tools/arrays.hpp>
#include <triqs/arrays/blas_lapack/stev.hpp>
#include <triqs/utility/is_complex.hpp>
#include <triqs/utility/numeric_ops.hpp>

using namespace triqs::arrays;
using namespace triqs;
using dcomplex = std::complex<double>;

template <typename T> void check_eig(matrix<T> M, matrix_view<T> vectors, vector_view<double, 1> values) {
  auto _ = range();
  for (auto i : range(0, first_dim(M))) { EXPECT_ARRAY_NEAR(M * vectors(i, _), values(i) * vectors(i, _), 1.e-13); }
}

template <typename Md, typename Me> void test(Md d, Me e) {
  using value_t = typename Me::value_type;
  using triqs::utility::conj;

  matrix<value_t> A(first_dim(d), first_dim(d));
  assign_foreach(A, [&](size_t i, size_t j) { return j == i ? d(i) : j == i + 1 ? e(i) : j == i - 1 ? conj(e(j)) : .0; });

  int size = first_dim(d);
  for (int init_size : {0, size, 2 * size}) {
    lapack::tridiag_worker<triqs::is_complex<value_t>::value> w(init_size);
    w(d, e);
    check_eig(A, w.vectors(), w.values());
  }
}

TEST(tridiag, real) {
  vector<double> d(5);
  vector<double> e(4);
  assign_foreach(d, [](size_t i) { return 3.2 * i + 2.3; });
  assign_foreach(e, [](size_t i) { return 2.4 * (i + 1.82) + 0.78; });
  e(1) = .0;
  test(d, e);
}

TEST(tridiag, complex) {
  vector<double> d(5);
  vector<dcomplex> e(4);
  assign_foreach(d, [](size_t i) { return 3.2 * i + 2.3; });
  assign_foreach(e, [](size_t i) { return 2.4 * (i + 1.82) + 0.78 * dcomplex(0, 1.0); });
  e(1) = .0;
  test(d, e);
}

MAKE_MAIN;
