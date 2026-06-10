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

// Phase-2 stress test for det_manip_qr: drive long sequences of *incremental* rank-1 insert/remove
// (the moves that use the Givens update/downdate) and cross-check the determinant and inverse against
// independent from-scratch LAPACK values (nda::linalg::det / nda::linalg::inv). Runs with the periodic
// re-factorization safety net effectively disabled, so the pure-incremental factorization is what is
// being validated. Also exercises removing down to size 0 and re-growing.

#include <triqs/det_manip/det_manip_qr.hpp>
#include <triqs/mc_tools/random_generator.hpp>
#include <nda/linalg/det.hpp>
#include <nda/linalg/inv.hpp>
#include <iostream>

template <class T1, class T2> void assert_close(T1 const &a, T2 const &b, double precision) {
  if (std::abs(a - b) > precision) TRIQS_RUNTIME_ERROR << "assert_close error : " << a << "\n" << b;
}
const double PRECISION = 1.e-8;

// element-wise max-abs-difference check between two square matrices (avoids a test-tool dependency)
template <class MA, class MB> void assert_mat_close(MA const &A, MB const &B, double precision) {
  long n = A.extent(0);
  for (long i = 0; i < n; ++i)
    for (long j = 0; j < n; ++j)
      if (std::abs(A(i, j) - B(i, j)) > precision) TRIQS_RUNTIME_ERROR << "assert_mat_close error at (" << i << "," << j << ")";
}

struct fun_real {
  using result_type   = double;
  using argument_type = double;
  double operator()(double x, double y) const {
    const double pi = acos(-1), beta = 10.0, epsi = 0.1;
    double tau = x - y;
    bool s     = (tau > 0);
    tau        = (s ? tau : beta + tau);
    double r   = epsi + tau / beta * (1 - 2 * epsi);
    return -2 * (pi / beta) / std::sin(pi * r);
  }
};

struct fun_cplx {
  using result_type   = std::complex<double>;
  using argument_type = double;
  std::complex<double> operator()(double x, double y) const {
    return fun_real{}(x, y) * std::exp(std::complex<double>(0.0, 0.3 * (x - y)));
  }
};

// Drive `nsteps` random rank-1 insert/remove moves and check det + inverse against from-scratch nda.
template <typename F> void stress(const char *tag) {
  std::cerr << "--- stress " << tag << " ---" << std::endl;
  F f;
  triqs::det_manip::det_manip_qr<F> D(f, 100);
  D.set_n_operations_before_refactor(1ul << 62); // effectively never refactorize -> pure incremental

  triqs::mc_tools::random_generator RNG("mt19937", 8865);
  for (size_t step = 0; step < 4000; ++step) {
    long s   = D.size();
    auto det = D.determinant();
    typename F::result_type ratio = 1;

    int move = (s == 0) ? 0 : RNG(2); // 0 = insert, 1 = remove
    if (move == 0) {
      double x = RNG(10.0), y = RNG(10.0);
      ratio    = D.try_insert(RNG(s + 1), RNG(s + 1), x, y);
    } else {
      ratio = D.try_remove(RNG(s), RNG(s));
    }

    if (std::abs(ratio * det) > 1.e-3) {
      D.complete_operation();
      if (D.size() > 0) { // cross-checks against independent from-scratch values
        assert_close(D.determinant(), typename F::result_type(nda::linalg::det(D.matrix())), PRECISION);
        assert_mat_close(nda::matrix<typename F::result_type>(nda::linalg::inv(D.matrix())), D.inverse_matrix(), PRECISION);
        assert_close(det * ratio, D.determinant(), PRECISION);
      }
    } else {
      D.reject_last_try();
    }
  }
}

// Remove all the way down to size 0 and re-grow, checking consistency throughout.
void shrink_to_zero() {
  std::cerr << "--- shrink_to_zero ---" << std::endl;
  fun_real f;
  triqs::det_manip::det_manip_qr<fun_real> D(f, 20);
  D.set_n_operations_before_refactor(1ul << 62);

  for (int x = 0; x < 6; ++x) D.insert_at_end(double(x) + 0.3, double(x) - 0.2);
  assert_close(D.determinant(), double(nda::linalg::det(D.matrix())), PRECISION);

  while (D.size() > 0) {
    D.remove(0, 0);
    if (D.size() > 0) assert_close(D.determinant(), double(nda::linalg::det(D.matrix())), PRECISION);
  }
  if (D.determinant() != 1.0) TRIQS_RUNTIME_ERROR << "empty determinant should be 1, got " << D.determinant();

  // re-grow after reaching size 0
  for (int x = 0; x < 4; ++x) D.insert_at_end(double(x) + 1.1, double(x) + 0.7);
  assert_close(D.determinant(), double(nda::linalg::det(D.matrix())), PRECISION);
  assert_mat_close(nda::matrix<double>(nda::linalg::inv(D.matrix())), D.inverse_matrix(), PRECISION);
}

int main() {
  stress<fun_real>("real");
  stress<fun_cplx>("complex");
  shrink_to_zero();
  std::cerr << "ALL PHASE-2 CHECKS PASSED" << std::endl;
}
