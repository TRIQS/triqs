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

#include <benchmark/benchmark.h>
#include <triqs/det_manip/det_manip.hpp>
#include <triqs/mc_tools/random_generator.hpp>
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

static constexpr long K = 30; // typical DLR mesh size

// Build a det_manip of size N using a seeded RNG for reproducibility
triqs::det_manip::det_manip<fun> make_det(long N) {
  fun f;
  triqs::det_manip::det_manip<fun> D(f, N + 10);
  D.set_singular_threshold(0);
  D.set_n_operations_before_check(N + 100);
  triqs::mc_tools::random_generator RNG("mt19937", 12345);
  for (long n = 0; n < N; ++n) {
    double x = RNG(10.0);
    double y = RNG(10.0);
    D.insert(D.size(), D.size(), x, y);
  }
  D.regenerate(); // refresh inverse for numerical accuracy
  return D;
}

// ============ Rank-1 benchmarks ============

static void BM_insert_ratios_loop(benchmark::State &state) {
  long N = state.range(0);
  auto D = make_det(N);
  std::vector<double> xs(K), ys(K);
  for (long m = 0; m < K; ++m) {
    xs[m] = 0.5 + 0.1 * m;
    ys[m] = 0.3 + 0.1 * m;
  }
  for (auto _ : state) {
    double sum = 0;
    for (long m = 0; m < K; ++m) {
      auto r = D.try_insert(0, 0, xs[m], ys[m]);
      D.reject_last_try();
      sum += r;
    }
    benchmark::DoNotOptimize(sum);
  }
}

static void BM_insert_ratios_batch(benchmark::State &state) {
  long N = state.range(0);
  auto D = make_det(N);
  nda::array<double, 1> xs(K), ys(K);
  for (long m = 0; m < K; ++m) {
    xs(m) = 0.5 + 0.1 * m;
    ys(m) = 0.3 + 0.1 * m;
  }
  for (auto _ : state) {
    auto result = D.insert_ratios(0, 0, xs, ys);
    benchmark::DoNotOptimize(result);
  }
}

BENCHMARK(BM_insert_ratios_loop)->Arg(16)->Arg(64)->Arg(128)->Arg(256)->Arg(512);
BENCHMARK(BM_insert_ratios_batch)->Arg(16)->Arg(64)->Arg(128)->Arg(256)->Arg(512);

// ============ Rank-2 benchmarks ============

static void BM_insert2_ratios_loop(benchmark::State &state) {
  long N = state.range(0);
  auto D = make_det(N);
  std::vector<double> x0s(K), x1s(K), y0s(K), y1s(K);
  for (long m = 0; m < K; ++m) {
    x0s[m] = 0.5 + 0.1 * m;
    x1s[m] = 0.6 + 0.1 * m;
    y0s[m] = 0.3 + 0.1 * m;
    y1s[m] = 0.4 + 0.1 * m;
  }
  for (auto _ : state) {
    double sum = 0;
    for (long m = 0; m < K; ++m) {
      auto r = D.try_insert2(0, 1, 0, 1, x0s[m], x1s[m], y0s[m], y1s[m]);
      D.reject_last_try();
      sum += r;
    }
    benchmark::DoNotOptimize(sum);
  }
}

static void BM_insert2_ratios_batch(benchmark::State &state) {
  long N = state.range(0);
  auto D = make_det(N);
  nda::array<double, 1> x0s(K), x1s(K), y0s(K), y1s(K);
  for (long m = 0; m < K; ++m) {
    x0s(m) = 0.5 + 0.1 * m;
    x1s(m) = 0.6 + 0.1 * m;
    y0s(m) = 0.3 + 0.1 * m;
    y1s(m) = 0.4 + 0.1 * m;
  }
  for (auto _ : state) {
    auto result = D.insert2_ratios(0, 1, 0, 1, x0s, x1s, y0s, y1s);
    benchmark::DoNotOptimize(result);
  }
}

BENCHMARK(BM_insert2_ratios_loop)->Arg(16)->Arg(64)->Arg(128)->Arg(256)->Arg(512);
BENCHMARK(BM_insert2_ratios_batch)->Arg(16)->Arg(64)->Arg(128)->Arg(256)->Arg(512);

static void BM_insert2_ratios_broadcast_batch(benchmark::State &state) {
  long N = state.range(0);
  auto D = make_det(N);
  // x0s(M, K), y0s(M, K) vary; x1s(K), y1s(K) broadcast -- matches insertk broadcast with k=2
  long M = 5;
  nda::array<double, 2> x0s(M, K), y0s(M, K);
  nda::array<double, 1> x1s(K), y1s(K);
  for (long i = 0; i < M; ++i)
    for (long m = 0; m < K; ++m) {
      x0s(i, m) = 0.5 + 0.1 * m + 0.2 * i;
      y0s(i, m) = 0.3 + 0.1 * m + 0.2 * i;
    }
  for (long m = 0; m < K; ++m) {
    x1s(m) = 0.6 + 0.1 * m;
    y1s(m) = 0.4 + 0.1 * m;
  }
  for (auto _ : state) {
    auto result = D.insert2_ratios(0, 1, 0, 1, x0s, x1s, y0s, y1s);
    benchmark::DoNotOptimize(result);
  }
}

BENCHMARK(BM_insert2_ratios_broadcast_batch)->Arg(64)->Arg(128)->Arg(256);

// ============ Matrix (product) benchmarks ============

static void BM_insert_ratios_matrix_loop(benchmark::State &state) {
  long N = state.range(0);
  auto D = make_det(N);
  std::vector<double> xs(K), ys(K);
  for (long m = 0; m < K; ++m) {
    xs[m] = 0.5 + 0.1 * m;
    ys[m] = 0.3 + 0.1 * m;
  }
  for (auto _ : state) {
    double sum = 0;
    for (long a = 0; a < K; ++a)
      for (long b = 0; b < K; ++b) {
        auto r = D.try_insert(0, 0, xs[a], ys[b]);
        D.reject_last_try();
        sum += r;
      }
    benchmark::DoNotOptimize(sum);
  }
}

static void BM_insert_ratios_matrix_batch(benchmark::State &state) {
  long N = state.range(0);
  auto D = make_det(N);
  nda::array<double, 1> xs(K), ys(K);
  for (long m = 0; m < K; ++m) {
    xs(m) = 0.5 + 0.1 * m;
    ys(m) = 0.3 + 0.1 * m;
  }
  for (auto _ : state) {
    auto result = D.insert_ratios_matrix(0, 0, xs, ys);
    benchmark::DoNotOptimize(result);
  }
}

BENCHMARK(BM_insert_ratios_matrix_loop)->Arg(16)->Arg(64)->Arg(128)->Arg(256)->Arg(512);
BENCHMARK(BM_insert_ratios_matrix_batch)->Arg(16)->Arg(64)->Arg(128)->Arg(256)->Arg(512);

// ============ Rank-k benchmarks ============

static void BM_insertk_ratios_batch(benchmark::State &state) {
  long N = state.range(0);
  long k = state.range(1);
  auto D = make_det(N);
  nda::matrix<double> xs(K, k), ys(K, k);
  for (long m = 0; m < K; ++m)
    for (long j = 0; j < k; ++j) {
      xs(m, j) = 0.5 + 0.1 * m + 0.01 * j;
      ys(m, j) = 0.3 + 0.1 * m + 0.01 * j;
    }
  for (auto _ : state) {
    auto result = D.insertk_ratios(xs, ys);
    benchmark::DoNotOptimize(result);
  }
}

BENCHMARK(BM_insertk_ratios_batch)
   ->Args({64, 1})
   ->Args({64, 2})
   ->Args({64, 3})
   ->Args({64, 4})
   ->Args({128, 1})
   ->Args({128, 2})
   ->Args({128, 3})
   ->Args({128, 4})
   ->Args({256, 1})
   ->Args({256, 2})
   ->Args({256, 3})
   ->Args({256, 4});

// ============ Rank-k broadcast benchmarks ============

static constexpr long M_bcast = 5; // broadcast dimension

static void BM_insertk_ratios_broadcast_batch(benchmark::State &state) {
  long N_size = state.range(0);
  long k      = state.range(1);
  auto D      = make_det(N_size);
  nda::array<double, 3> xs(M_bcast, K, k);
  nda::matrix<double> ys(K, k);
  for (long m = 0; m < K; ++m)
    for (long j = 0; j < k; ++j) ys(m, j) = 0.3 + 0.1 * m + 0.01 * j;
  for (long i = 0; i < M_bcast; ++i)
    for (long m = 0; m < K; ++m)
      for (long j = 0; j < k; ++j) xs(i, m, j) = 0.5 + 0.1 * m + 0.01 * j + 0.2 * i;
  for (auto _ : state) {
    auto result = D.insertk_ratios(xs, ys);
    benchmark::DoNotOptimize(result);
  }
}

BENCHMARK(BM_insertk_ratios_broadcast_batch)->Args({64, 2})->Args({64, 3})->Args({128, 2})->Args({128, 3})->Args({256, 2})->Args({256, 3});
