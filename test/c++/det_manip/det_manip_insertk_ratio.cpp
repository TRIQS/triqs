// Copyright (c) 2024-2025 Simons Foundation
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

#include <triqs/det_manip/det_manip.hpp>
#include <triqs/det_manip/det_manip_basic.hpp>
#include <triqs/mc_tools/random_generator.hpp>
#include <nda/linalg/det.hpp>
#include <iostream>
#include <cmath>
#include <vector>

struct fun {
  using result_type   = double;
  using argument_type = double;

  double operator()(double x, double y) const {
    const double pi   = acos(-1.0);
    const double beta = 10.0;
    const double epsi = 0.1;
    double tau        = x - y;
    bool s            = (tau > 0);
    tau               = (s ? tau : beta + tau);
    double r          = epsi + tau / beta * (1 - 2 * epsi);
    return -2 * (pi / beta) / std::sin(pi * r);
  }
};

template <typename T1, typename T2> void assert_close(T1 const &A, T2 const &B, double precision, std::string const &msg = "") {
  double diff  = std::abs(A - B);
  double scale = std::max(std::abs(double(A)), std::abs(double(B)));
  if (diff > precision * std::max(scale, 1.0))
    TRIQS_RUNTIME_ERROR << "assert_close error: " << A << " vs " << B << " diff=" << diff << " reldiff=" << diff / std::max(scale, 1e-30) << " "
                        << msg;
}

template <typename DM> void build_det(DM &D, int target_size, triqs::mc_tools::random_generator &RNG) {
  for (int n = 0; n < target_size; ++n) {
    double x = RNG(10.0);
    double y = RNG(10.0);
    D.insert(D.size(), D.size(), x, y);
  }
}

// Reference for det_manip: use try_insert for k=1, try_insert2 for k=2, try_insert_k for k>=3
double insertk_ratio_via_try(triqs::det_manip::det_manip<fun> &D, std::span<const double> xs, std::span<const double> ys) {
  long k = static_cast<long>(xs.size());
  double ratio;
  if (k == 1) {
    ratio = D.try_insert(0, 0, xs[0], ys[0]);
  } else if (k == 2) {
    ratio = D.try_insert2(0, 1, 0, 1, xs[0], xs[1], ys[0], ys[1]);
  } else {
    std::vector<long> positions(k);
    std::iota(positions.begin(), positions.end(), 0L);
    std::vector<double> xv(xs.begin(), xs.end());
    std::vector<double> yv(ys.begin(), ys.end());
    ratio = D.try_insert_k(positions, positions, xv, yv);
  }
  D.reject_last_try();
  return ratio;
}

// Reference for det_manip_basic: build the (N+k)x(N+k) augmented matrix
double insertk_ratio_reference_basic(triqs::det_manip::det_manip_basic<fun> &D, std::span<const double> xs, std::span<const double> ys) {
  fun f;
  long N  = D.size();
  long k  = static_cast<long>(xs.size());
  long Nk = N + k;

  nda::matrix<double> aug(Nk, Nk);

  // Fill existing block (shifted to rows/cols k..N+k-1)
  for (long r = 0; r < N; ++r)
    for (long c = 0; c < N; ++c) aug(r + k, c + k) = f(D.get_x(r), D.get_y(c));

  // New rows (0..k-1) x existing cols (k..N+k-1)
  for (long l = 0; l < k; ++l)
    for (long c = 0; c < N; ++c) aug(l, c + k) = f(xs[l], D.get_y(c));

  // Existing rows (k..N+k-1) x new cols (0..k-1)
  for (long m = 0; m < k; ++m)
    for (long r = 0; r < N; ++r) aug(r + k, m) = f(D.get_x(r), ys[m]);

  // New rows x new cols (top-left kxk block)
  for (long l = 0; l < k; ++l)
    for (long m = 0; m < k; ++m) aug(l, m) = f(xs[l], ys[m]);

  nda::range R(0, Nk);
  double det_new = nda::linalg::det(aug(R, R));

  if (N == 0) return det_new;
  return det_new / D.determinant();
}

// Helper: build (K, k) matrix of random values
nda::matrix<double> random_matrix(long K, long k, triqs::mc_tools::random_generator &RNG, double range = 10.0) {
  nda::matrix<double> m(K, k);
  for (long i = 0; i < K; ++i)
    for (long j = 0; j < k; ++j) m(i, j) = RNG(range);
  return m;
}

// ============ Batched insertk_ratios tests ============

// Batched result must match try_insert / try_insert2 / try_insert_k for each candidate
void test_insertk_ratios_vs_sequential() {
  std::cerr << "=== test_insertk_ratios_vs_sequential ===" << std::endl;
  fun f;
  triqs::det_manip::det_manip<fun> D(f, 100);
  triqs::mc_tools::random_generator RNG("mt19937", 14001);
  build_det(D, 15, RNG);

  for (int k = 1; k <= 6; ++k) {
    long K  = 20;
    auto xs = random_matrix(K, k, RNG);
    auto ys = random_matrix(K, k, RNG);

    auto batch = D.insertk_ratios(xs, ys);

    for (long m = 0; m < K; ++m) {
      std::vector<double> xm(k), ym(k);
      for (long j = 0; j < k; ++j) {
        xm[j] = xs(m, j);
        ym[j] = ys(m, j);
      }
      auto ref = insertk_ratio_via_try(D, xm, ym);
      assert_close(batch(m), ref, 1.e-6, "vs_sequential k=" + std::to_string(k) + " m=" + std::to_string(m));
    }
  }
  std::cerr << "PASSED" << std::endl;
}

// Batched k=1 matches try_insert, k=2 matches try_insert2, k>=3 matches try_insert_k
void test_insertk_ratios_vs_try() {
  std::cerr << "=== test_insertk_ratios_vs_try ===" << std::endl;
  fun f;
  triqs::det_manip::det_manip<fun> D(f, 100);
  triqs::mc_tools::random_generator RNG("mt19937", 14002);
  build_det(D, 15, RNG);

  for (int k = 1; k <= 5; ++k) {
    long K  = 15;
    auto xs = random_matrix(K, k, RNG);
    auto ys = random_matrix(K, k, RNG);

    auto batch = D.insertk_ratios(xs, ys);

    for (long m = 0; m < K; ++m) {
      std::vector<double> xm(k), ym(k);
      for (long j = 0; j < k; ++j) {
        xm[j] = xs(m, j);
        ym[j] = ys(m, j);
      }
      auto ref = insertk_ratio_via_try(D, xm, ym);
      assert_close(batch(m), ref, 1.e-6, "vs_try k=" + std::to_string(k) + " m=" + std::to_string(m));
    }
  }
  std::cerr << "PASSED" << std::endl;
}

// N=0: empty matrix
void test_insertk_ratios_empty_matrix() {
  std::cerr << "=== test_insertk_ratios_empty_matrix ===" << std::endl;
  fun f;
  triqs::det_manip::det_manip<fun> D(f, 100);
  triqs::mc_tools::random_generator RNG("mt19937", 14003);

  for (int k = 1; k <= 4; ++k) {
    long K  = 10;
    auto xs = random_matrix(K, k, RNG);
    auto ys = random_matrix(K, k, RNG);

    auto batch = D.insertk_ratios(xs, ys);

    for (long m = 0; m < K; ++m) {
      std::vector<double> xm(k), ym(k);
      for (long j = 0; j < k; ++j) {
        xm[j] = xs(m, j);
        ym[j] = ys(m, j);
      }
      auto ref = insertk_ratio_via_try(D, xm, ym);
      assert_close(batch(m), ref, 1.e-14, "empty k=" + std::to_string(k) + " m=" + std::to_string(m));
    }
  }
  std::cerr << "PASSED" << std::endl;
}

// State must not change
void test_insertk_ratios_state_unchanged() {
  std::cerr << "=== test_insertk_ratios_state_unchanged ===" << std::endl;
  fun f;
  triqs::det_manip::det_manip<fun> D(f, 100);
  triqs::mc_tools::random_generator RNG("mt19937", 14004);
  build_det(D, 15, RNG);

  auto det_before  = D.determinant();
  auto inv_before  = D.inverse_matrix();
  auto size_before = D.size();

  for (int k = 1; k <= 4; ++k) {
    auto xs = random_matrix(10, k, RNG);
    auto ys = random_matrix(10, k, RNG);
    D.insertk_ratios(xs, ys);
  }

  if (D.size() != size_before) TRIQS_RUNTIME_ERROR << "Size changed!";
  assert_close(D.determinant(), det_before, 1.e-14, "det changed");
  auto inv_after = D.inverse_matrix();
  for (int i = 0; i < size_before; ++i)
    for (int j = 0; j < size_before; ++j) assert_close(inv_after(i, j), inv_before(i, j), 1.e-14, "inv changed");
  std::cerr << "PASSED" << std::endl;
}

// Various matrix sizes
void test_insertk_ratios_various_sizes() {
  std::cerr << "=== test_insertk_ratios_various_sizes ===" << std::endl;
  fun f;

  for (int N : {1, 2, 5, 10, 30}) {
    triqs::det_manip::det_manip<fun> D(f, 100);
    triqs::mc_tools::random_generator RNG("mt19937", 12005 + N);
    build_det(D, N, RNG);

    for (int k = 1; k <= 4; ++k) {
      long K  = 10;
      auto xs = random_matrix(K, k, RNG);
      auto ys = random_matrix(K, k, RNG);

      auto batch = D.insertk_ratios(xs, ys);
      for (long m = 0; m < K; ++m) {
        std::vector<double> xm(k), ym(k);
        for (long j = 0; j < k; ++j) {
          xm[j] = xs(m, j);
          ym[j] = ys(m, j);
        }
        auto ref = insertk_ratio_via_try(D, xm, ym);
        assert_close(batch(m), ref, 1.e-6, "N=" + std::to_string(N) + " k=" + std::to_string(k) + " m=" + std::to_string(m));
      }
    }
  }
  std::cerr << "PASSED" << std::endl;
}

// Cross-validate det_manip vs det_manip_basic
void test_insertk_ratios_cross_validate() {
  std::cerr << "=== test_insertk_ratios_cross_validate ===" << std::endl;
  fun f;

  for (int N : {3, 5, 8}) {
    triqs::det_manip::det_manip<fun> D(f, 100);
    triqs::det_manip::det_manip_basic<fun> Db(f, 100);
    triqs::mc_tools::random_generator RNG("mt19937", 12006 + N);

    for (int n = 0; n < N; ++n) {
      double x = RNG(10.0);
      double y = RNG(10.0);
      D.insert(D.size(), D.size(), x, y);
      Db.insert(Db.size(), Db.size(), x, y);
    }
    D.regenerate();

    for (int k = 1; k <= 4; ++k) {
      long K  = 10;
      auto xs = random_matrix(K, k, RNG);
      auto ys = random_matrix(K, k, RNG);

      auto batch_opt   = D.insertk_ratios(xs, ys);
      auto batch_basic = Db.insertk_ratios(xs, ys);

      for (long m = 0; m < K; ++m)
        assert_close(batch_opt(m), batch_basic(m), 1.e-2, "cross N=" + std::to_string(N) + " k=" + std::to_string(k) + " m=" + std::to_string(m));
    }
  }
  std::cerr << "PASSED" << std::endl;
}

// K=1: single candidate
void test_insertk_ratios_single_candidate() {
  std::cerr << "=== test_insertk_ratios_single_candidate ===" << std::endl;
  fun f;
  triqs::det_manip::det_manip<fun> D(f, 100);
  triqs::mc_tools::random_generator RNG("mt19937", 14005);
  build_det(D, 10, RNG);

  for (int k = 1; k <= 4; ++k) {
    auto xs    = random_matrix(1, k, RNG);
    auto ys    = random_matrix(1, k, RNG);
    auto batch = D.insertk_ratios(xs, ys);
    std::vector<double> xm(k), ym(k);
    for (long j = 0; j < k; ++j) {
      xm[j] = xs(0, j);
      ym[j] = ys(0, j);
    }
    auto ref = insertk_ratio_via_try(D, xm, ym);
    assert_close(batch(0), ref, 1.e-10, "single k=" + std::to_string(k));
  }
  std::cerr << "PASSED" << std::endl;
}

// K=0: empty batch
void test_insertk_ratios_empty_batch() {
  std::cerr << "=== test_insertk_ratios_empty_batch ===" << std::endl;
  fun f;
  triqs::det_manip::det_manip<fun> D(f, 100);
  triqs::mc_tools::random_generator RNG("mt19937", 14006);
  build_det(D, 10, RNG);

  nda::matrix<double> xs(0, 3), ys(0, 3);
  auto batch = D.insertk_ratios(xs, ys);
  if (batch.size() != 0) TRIQS_RUNTIME_ERROR << "Expected empty result";
  std::cerr << "PASSED" << std::endl;
}

// det_manip_basic: batch matches sequential
void test_basic_insertk_ratios_vs_sequential() {
  std::cerr << "=== test_basic_insertk_ratios_vs_sequential ===" << std::endl;
  fun f;
  triqs::det_manip::det_manip_basic<fun> Db(f, 100);
  triqs::mc_tools::random_generator RNG("mt19937", 14007);
  build_det(Db, 15, RNG);

  for (int k = 1; k <= 4; ++k) {
    long K  = 10;
    auto xs = random_matrix(K, k, RNG);
    auto ys = random_matrix(K, k, RNG);

    auto batch = Db.insertk_ratios(xs, ys);
    for (long m = 0; m < K; ++m) {
      std::vector<double> xm(k), ym(k);
      for (long j = 0; j < k; ++j) {
        xm[j] = xs(m, j);
        ym[j] = ys(m, j);
      }
      auto ref = insertk_ratio_reference_basic(Db, xm, ym);
      assert_close(batch(m), ref, 1.e-14, "basic k=" + std::to_string(k) + " m=" + std::to_string(m));
    }
  }
  std::cerr << "PASSED" << std::endl;
}

// Helper: build (M, K, k) rank-3 array of random values
nda::array<double, 3> random_array3(long M, long K, long k, triqs::mc_tools::random_generator &RNG, double range = 10.0) {
  nda::array<double, 3> a(M, K, k);
  for (long i = 0; i < M; ++i)
    for (long j = 0; j < K; ++j)
      for (long l = 0; l < k; ++l) a(i, j, l) = RNG(range);
  return a;
}

// ============ Broadcast insertk_ratios tests ============

// xs(M, K, k), ys(K, k): broadcast ys. Verify against sequential calls.
void test_insertk_ratios_broadcast_x() {
  std::cerr << "=== test_insertk_ratios_broadcast_x ===" << std::endl;
  fun f;
  triqs::det_manip::det_manip<fun> D(f, 100);
  triqs::mc_tools::random_generator RNG("mt19937", 14008);
  build_det(D, 15, RNG);

  for (int k = 1; k <= 4; ++k) {
    long M = 5, K = 10;
    auto xs3 = random_array3(M, K, k, RNG);
    auto ys2 = random_matrix(K, k, RNG);

    auto batch = D.insertk_ratios(xs3, ys2);
    TRIQS_ASSERT(batch.shape() == (std::array<long, 2>{M, K}));

    for (long i = 0; i < M; ++i)
      for (long m = 0; m < K; ++m) {
        std::vector<double> xm(k), ym(k);
        for (long j = 0; j < k; ++j) {
          xm[j] = xs3(i, m, j);
          ym[j] = ys2(m, j);
        }
        auto ref = insertk_ratio_via_try(D, xm, ym);
        assert_close(batch(i, m), ref, 1.e-6, "broadcast_x k=" + std::to_string(k) + " i=" + std::to_string(i) + " m=" + std::to_string(m));
      }
  }
  std::cerr << "PASSED" << std::endl;
}

// xs(K, k), ys(M, K, k): broadcast xs. Verify against sequential calls.
void test_insertk_ratios_broadcast_y() {
  std::cerr << "=== test_insertk_ratios_broadcast_y ===" << std::endl;
  fun f;
  triqs::det_manip::det_manip<fun> D(f, 100);
  triqs::mc_tools::random_generator RNG("mt19937", 14009);
  build_det(D, 15, RNG);

  for (int k = 1; k <= 4; ++k) {
    long M = 5, K = 10;
    auto xs2 = random_matrix(K, k, RNG);
    auto ys3 = random_array3(M, K, k, RNG);

    auto batch = D.insertk_ratios(xs2, ys3);
    TRIQS_ASSERT(batch.shape() == (std::array<long, 2>{M, K}));

    for (long i = 0; i < M; ++i)
      for (long m = 0; m < K; ++m) {
        std::vector<double> xm(k), ym(k);
        for (long j = 0; j < k; ++j) {
          xm[j] = xs2(m, j);
          ym[j] = ys3(i, m, j);
        }
        auto ref = insertk_ratio_via_try(D, xm, ym);
        assert_close(batch(i, m), ref, 1.e-6, "broadcast_y k=" + std::to_string(k) + " i=" + std::to_string(i) + " m=" + std::to_string(m));
      }
  }
  std::cerr << "PASSED" << std::endl;
}

// Cross-validate broadcast between det_manip and det_manip_basic
void test_insertk_ratios_broadcast_cross_validate() {
  std::cerr << "=== test_insertk_ratios_broadcast_cross_validate ===" << std::endl;
  fun f;

  for (int N_size : {3, 5, 8}) {
    triqs::det_manip::det_manip<fun> D(f, 100);
    triqs::det_manip::det_manip_basic<fun> Db(f, 100);
    triqs::mc_tools::random_generator RNG("mt19937", 13003 + N_size);

    for (int n = 0; n < N_size; ++n) {
      double x = RNG(10.0);
      double y = RNG(10.0);
      D.insert(D.size(), D.size(), x, y);
      Db.insert(Db.size(), Db.size(), x, y);
    }
    D.regenerate();

    for (int k = 1; k <= 3; ++k) {
      long M = 4, K = 6;
      // Test broadcast ys
      auto xs3         = random_array3(M, K, k, RNG);
      auto ys2         = random_matrix(K, k, RNG);
      auto batch_opt   = D.insertk_ratios(xs3, ys2);
      auto batch_basic = Db.insertk_ratios(xs3, ys2);
      for (long i = 0; i < M; ++i)
        for (long m = 0; m < K; ++m)
          assert_close(batch_opt(i, m), batch_basic(i, m), 1.e-2, "cross bcast_x N=" + std::to_string(N_size) + " k=" + std::to_string(k));

      // Test broadcast xs
      auto xs2b         = random_matrix(K, k, RNG);
      auto ys3b         = random_array3(M, K, k, RNG);
      auto batch_opt2   = D.insertk_ratios(xs2b, ys3b);
      auto batch_basic2 = Db.insertk_ratios(xs2b, ys3b);
      for (long i = 0; i < M; ++i)
        for (long m = 0; m < K; ++m)
          assert_close(batch_opt2(i, m), batch_basic2(i, m), 1.e-2, "cross bcast_y N=" + std::to_string(N_size) + " k=" + std::to_string(k));
    }
  }
  std::cerr << "PASSED" << std::endl;
}

// N=0 with broadcasting
void test_insertk_ratios_broadcast_empty_matrix() {
  std::cerr << "=== test_insertk_ratios_broadcast_empty_matrix ===" << std::endl;
  fun f;
  triqs::det_manip::det_manip<fun> D(f, 100);
  triqs::mc_tools::random_generator RNG("mt19937", 14010);

  for (int k = 1; k <= 3; ++k) {
    long M = 3, K = 5;
    auto xs3 = random_array3(M, K, k, RNG);
    auto ys2 = random_matrix(K, k, RNG);

    auto batch = D.insertk_ratios(xs3, ys2);
    for (long i = 0; i < M; ++i)
      for (long m = 0; m < K; ++m) {
        std::vector<double> xm(k), ym(k);
        for (long j = 0; j < k; ++j) {
          xm[j] = xs3(i, m, j);
          ym[j] = ys2(m, j);
        }
        auto ref = insertk_ratio_via_try(D, xm, ym);
        assert_close(batch(i, m), ref, 1.e-14, "bcast_empty k=" + std::to_string(k) + " i=" + std::to_string(i) + " m=" + std::to_string(m));
      }
  }
  std::cerr << "PASSED" << std::endl;
}

// N=0 with broadcasting in the other direction: xs(K, k), ys(M, K, k) (Rx < Ry)
void test_insertk_ratios_broadcast_y_empty_matrix() {
  std::cerr << "=== test_insertk_ratios_broadcast_y_empty_matrix ===" << std::endl;
  fun f;
  triqs::det_manip::det_manip<fun> D(f, 100);
  triqs::mc_tools::random_generator RNG("mt19937", 14011);

  for (int k = 1; k <= 3; ++k) {
    long M   = 3, K = 5;
    auto xs2 = random_matrix(K, k, RNG);
    auto ys3 = random_array3(M, K, k, RNG);

    auto batch = D.insertk_ratios(xs2, ys3);
    for (long i = 0; i < M; ++i)
      for (long m = 0; m < K; ++m) {
        std::vector<double> xm(k), ym(k);
        for (long j = 0; j < k; ++j) {
          xm[j] = xs2(m, j);
          ym[j] = ys3(i, m, j);
        }
        auto ref = insertk_ratio_via_try(D, xm, ym);
        assert_close(batch(i, m), ref, 1.e-14, "bcast_y_empty k=" + std::to_string(k) + " i=" + std::to_string(i) + " m=" + std::to_string(m));
      }
  }
  std::cerr << "PASSED" << std::endl;
}

// State unchanged after broadcast call
void test_insertk_ratios_broadcast_state_unchanged() {
  std::cerr << "=== test_insertk_ratios_broadcast_state_unchanged ===" << std::endl;
  fun f;
  triqs::det_manip::det_manip<fun> D(f, 100);
  triqs::mc_tools::random_generator RNG("mt19937", 14012);
  build_det(D, 15, RNG);

  auto det_before  = D.determinant();
  auto size_before = D.size();

  for (int k = 1; k <= 3; ++k) {
    auto xs3 = random_array3(4, 8, k, RNG);
    auto ys2 = random_matrix(8, k, RNG);
    D.insertk_ratios(xs3, ys2);
    auto xs2 = random_matrix(8, k, RNG);
    auto ys3 = random_array3(4, 8, k, RNG);
    D.insertk_ratios(xs2, ys3);
  }

  if (D.size() != size_before) TRIQS_RUNTIME_ERROR << "Size changed!";
  assert_close(D.determinant(), det_before, 1.e-14, "det changed");
  std::cerr << "PASSED" << std::endl;
}

int main() {
  // Batched insertk_ratios tests
  test_insertk_ratios_vs_sequential();
  test_insertk_ratios_vs_try();
  test_insertk_ratios_empty_matrix();
  test_insertk_ratios_state_unchanged();
  test_insertk_ratios_various_sizes();
  test_insertk_ratios_cross_validate();
  test_insertk_ratios_single_candidate();
  test_insertk_ratios_empty_batch();
  test_basic_insertk_ratios_vs_sequential();

  // Broadcast insertk_ratios tests
  test_insertk_ratios_broadcast_x();
  test_insertk_ratios_broadcast_y();
  test_insertk_ratios_broadcast_cross_validate();
  test_insertk_ratios_broadcast_empty_matrix();
  test_insertk_ratios_broadcast_y_empty_matrix();
  test_insertk_ratios_broadcast_state_unchanged();

  std::cerr << "\nAll tests PASSED." << std::endl;
  return 0;
}
