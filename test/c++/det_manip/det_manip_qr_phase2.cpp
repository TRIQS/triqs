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

// Phase-2 stress test for det_manip_qr: drive long sequences of *incremental* rank-1/2/3 insert/remove
// (the moves that use the Givens update/downdate) plus swap/roll, and cross-check the determinant and
// inverse against independent from-scratch LAPACK values (nda::linalg::det / nda::linalg::inv). Runs
// with the periodic re-factorization safety net effectively disabled, so the pure-incremental
// factorization is what is being validated; verify_qr is asserted at every step.

#include <triqs/det_manip/det_manip_qr.hpp>
#include <triqs/mc_tools/random_generator.hpp>
#include <nda/linalg/det.hpp>
#include <nda/linalg/inv.hpp>
#include <algorithm>
#include <iostream>
#include <vector>

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

// Drive random rank-1/2/3 insert/remove moves and check det + inverse against from-scratch nda.
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
    bool tried                    = true;

    // 0 = insert, 1 = remove (rank 1); 2 = insert2, 3 = remove2 (rank 2); 4 = insert_k, 5 = remove_k (rank 3)
    int move = (s == 0) ? 0 : RNG(6);
    if (move == 0) {
      ratio = D.try_insert(RNG(s + 1), RNG(s + 1), RNG(10.0), RNG(10.0));
    } else if (move == 1) {
      ratio = D.try_remove(RNG(s), RNG(s));
    } else if (move == 2) {
      long i0 = RNG(s + 1), i1 = RNG(s + 2), j0 = RNG(s + 1), j1 = RNG(s + 2);
      if (i0 != i1 && j0 != j1)
        ratio = D.try_insert2(i0, i1, j0, j1, RNG(10.0), RNG(10.0), RNG(10.0), RNG(10.0));
      else
        tried = false;
    } else if (move == 3) {
      if (s >= 2) {
        long i0 = RNG(s), i1 = RNG(s), j0 = RNG(s), j1 = RNG(s);
        if (i0 != i1 && j0 != j1)
          ratio = D.try_remove2(i0, i1, j0, j1);
        else
          tried = false;
      } else
        tried = false;
    } else if (move == 4) {
      // rank-3 insert_k at 3 distinct positions valid at insertion time
      std::vector<long> iv{RNG(s + 1), s + 1, s + 2}, jv{RNG(s + 1), s + 1, s + 2};
      std::vector<double> xv{RNG(10.0), RNG(10.0), RNG(10.0)}, yv{RNG(10.0), RNG(10.0), RNG(10.0)};
      ratio = D.try_insert_k(iv, jv, xv, yv);
    } else {
      // rank-3 remove_k at 3 distinct rows / columns
      if (s >= 3) {
        std::vector<long> iv{RNG(s), RNG(s), RNG(s)}, jv{RNG(s), RNG(s), RNG(s)};
        std::sort(iv.begin(), iv.end());
        std::sort(jv.begin(), jv.end());
        if (iv[0] != iv[1] && iv[1] != iv[2] && jv[0] != jv[1] && jv[1] != jv[2])
          ratio = D.try_remove_k(iv, jv);
        else
          tried = false;
      } else
        tried = false;
    }

    if (tried && std::abs(ratio * det) > 1.e-3) {
      D.complete_operation();
      if (D.size() > 0) {
        if (!D.verify_qr(1.e-9)) TRIQS_RUNTIME_ERROR << "verify_qr failed at size " << D.size();
        assert_close(D.determinant(), typename F::result_type(nda::linalg::det(D.matrix())), PRECISION);
        assert_mat_close(nda::matrix<typename F::result_type>(nda::linalg::inv(D.matrix())), D.inverse_matrix(), PRECISION);
        assert_close(det * ratio, D.determinant(), PRECISION);
      }
    } else {
      D.reject_last_try();
    }
  }
}

// Exercise swap_row / swap_col / roll_matrix and verify they keep the factorization consistent.
void misc_ops() {
  std::cerr << "--- misc_ops (swap/roll) ---" << std::endl;
  fun_real f;
  triqs::det_manip::det_manip_qr<fun_real> D(f, 30);
  D.set_n_operations_before_refactor(1ul << 62);
  triqs::mc_tools::random_generator RNG("mt19937", 4242);

  for (int x = 0; x < 7; ++x) D.insert_at_end(RNG(10.0), RNG(10.0));

  auto check = [&] {
    long s = D.size();
    if (s == 0) return;
    if (!D.verify_qr(1.e-9)) TRIQS_RUNTIME_ERROR << "verify_qr failed (misc_ops)";
    assert_close(D.determinant(), double(nda::linalg::det(D.matrix())), PRECISION);
    assert_mat_close(nda::matrix<double>(nda::linalg::inv(D.matrix())), D.inverse_matrix(), PRECISION);
  };
  check();

  for (int rep = 0; rep < 200; ++rep) {
    long s   = D.size();
    int kind = RNG(4);
    if (kind == 0)
      D.swap_row(RNG(s), RNG(s));
    else if (kind == 1)
      D.swap_col(RNG(s), RNG(s));
    else if (kind == 2)
      D.roll_matrix(RNG(2) == 0 ? D.Up : D.Down);
    else
      D.roll_matrix(RNG(2) == 0 ? D.Left : D.Right);
    check();
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

  for (int x = 0; x < 4; ++x) D.insert_at_end(double(x) + 1.1, double(x) + 0.7);
  assert_close(D.determinant(), double(nda::linalg::det(D.matrix())), PRECISION);
  assert_mat_close(nda::matrix<double>(nda::linalg::inv(D.matrix())), D.inverse_matrix(), PRECISION);
}

int main() {
  stress<fun_real>("real");
  stress<fun_cplx>("complex");
  misc_ops();
  shrink_to_zero();
  std::cerr << "ALL PHASE-2 CHECKS PASSED" << std::endl;
}
