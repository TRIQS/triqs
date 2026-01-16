// Copyright (c) 2022-2025 Simons Foundation
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

/**
 * @file GfEval.cpp
 * @brief Benchmarks for Green's function evaluation at arbitrary tau points.
 *
 * This benchmark compares the performance of different mesh types for
 * evaluating Green's functions at random imaginary time points:
 *
 * - imtime: Linear interpolation on uniform grid
 * - dlr: Discrete Lehmann Representation basis expansion
 * - chebyshev: Barycentric interpolation on Chebyshev nodes
 * - prod<cyclat, chebyshev>: Product mesh with lattice and Chebyshev dimensions
 *
 * Target types tested:
 * - matrix_valued: 2x2 complex matrix (typical use case)
 * - scalar_valued: complex scalar
 * - scalar_real_valued: real scalar (comparable to diagmc chebfun_t)
 */

#include <benchmark/benchmark.h>
#include <triqs/gfs.hpp>
#include <triqs/mesh.hpp>
#include <triqs/lattice/bravais_lattice.hpp>
#include <nda/nda.hpp>

#include <cmath>
#include <numbers>
#include <random>
#include <vector>

using namespace triqs::gfs;
using namespace triqs::mesh;
using namespace triqs::lattice;

// ============================================================================
// Reference implementation: diagmc-style chebfun_t
//
// This is a self-contained version of the diagmc chebfun_t class for direct
// performance comparison. It uses std::vector storage and the classic
// barycentric interpolation formula without precomputed normalized weights.
// ============================================================================

namespace reference {

  // Scale from [a,b] to [-1,1]
  inline double to_standard(double x, double a, double b) { return 2.0 * (x - 0.5 * (b + a)) / (b - a); }

  class chebfun_t {
    std::vector<double> p; // Chebyshev points on [-1,1]
    std::vector<double> w; // Barycentric weights
    std::vector<double> f; // Function values
    double a_, b_;         // Interval [a, b]

    public:
    chebfun_t(long n, double a, double b) : p(n), w(n), f(n), a_(a), b_(b) {
      const double pi = std::numbers::pi;
      for (long i = 0; i < n; ++i) {
        // Points and weights in ascending order (like diagmc)
        p[n - 1 - i] = std::cos((2.0 * i + 1.0) * pi / (2.0 * n));
        w[n - 1 - i] = ((i % 2 == 0) ? 1.0 : -1.0) * std::sin((2.0 * i + 1.0) * pi / (2.0 * n));
      }
    }

    double &value(long i) { return f[i]; }
    long size() const { return std::ssize(p); }

    // Evaluate using classic barycentric formula (like diagmc)
    double operator()(double x) const {
      x = to_standard(x, a_, b_);
      double num = 0.0, den = 0.0;
      for (size_t i = 0; i < p.size(); ++i) {
        if (x == p[i]) return f[i];
        double q = w[i] / (x - p[i]);
        num += q * f[i];
        den += q;
      }
      return num / den;
    }
  };

} // namespace reference

// ============================================================================
// Configuration
// ============================================================================

namespace {

constexpr double beta     = 100.0; // Inverse temperature
constexpr int N_matrix    = 2;     // Matrix dimension for matrix_valued tests
constexpr int N_eval      = 1000;  // Number of evaluation points per iteration
constexpr double dlr_wmax = 10.0;  // DLR frequency cutoff (gives rank ~50)
constexpr double dlr_eps  = 1e-10; // DLR precision
constexpr int L_lattice   = 8;     // Linear lattice size for cyclat mesh

// Generate random tau points in [0, beta] for evaluation
std::vector<double> generate_tau_points(int n, double b) {
  std::vector<double> tau_pts(n);
  std::mt19937 gen(42); // Fixed seed for reproducibility
  std::uniform_real_distribution<> dis(0.0, b);
  for (int i = 0; i < n; ++i) tau_pts[i] = dis(gen);
  return tau_pts;
}

// Generate random lattice site indices for cyclat evaluation
using r_idx_t = std::array<long, 3>;
std::vector<r_idx_t> generate_r_points(int n, int L) {
  std::vector<r_idx_t> r_pts(n);
  std::mt19937 gen(123); // Fixed seed for reproducibility
  std::uniform_int_distribution<long> dis(0, L - 1);
  for (int i = 0; i < n; ++i) r_pts[i] = {dis(gen), dis(gen), 0};
  return r_pts;
}

const std::vector<double> tau_points = generate_tau_points(N_eval, beta);
const std::vector<r_idx_t> r_points  = generate_r_points(N_eval, L_lattice);

} // namespace

// ============================================================================
// Imtime mesh benchmarks: Linear interpolation on uniform grid
// ============================================================================

static void BM_Imtime_Matrix(benchmark::State &state) {
  long n_tau = state.range(0);
  auto m     = imtime{beta, Fermion, n_tau};
  auto G     = gf<imtime, matrix_valued>{m, {N_matrix, N_matrix}};
  G()        = 0.0;

  for (auto _ : state) {
    for (double tau : tau_points) { benchmark::DoNotOptimize(G(tau)); }
  }
  state.SetItemsProcessed(state.iterations() * N_eval);
}
BENCHMARK(BM_Imtime_Matrix)->RangeMultiplier(2)->Range(1024, 16384)->Unit(benchmark::kMicrosecond);

// ============================================================================
// DLR mesh benchmarks: Discrete Lehmann Representation basis expansion
// ============================================================================

static void BM_DLR_Matrix(benchmark::State &state) {
  double w_max = state.range(0);
  auto m       = dlr{beta, Fermion, w_max, dlr_eps};
  auto G       = gf<dlr, matrix_valued>{m, {N_matrix, N_matrix}};
  G()          = 0.0;

  for (auto _ : state) {
    for (double tau : tau_points) { benchmark::DoNotOptimize(G(tau)); }
  }
  state.SetItemsProcessed(state.iterations() * N_eval);
  state.counters["rank"] = m.size();
}
BENCHMARK(BM_DLR_Matrix)->Arg(1)->Arg(5)->Arg(10)->Arg(20)->Unit(benchmark::kMicrosecond);

// ============================================================================
// Chebyshev mesh benchmarks: Barycentric interpolation on Chebyshev nodes
// ============================================================================

// Matrix-valued (2x2 complex): typical use case for Green's functions
static void BM_Chebyshev_Matrix(benchmark::State &state) {
  long n_cheb = state.range(0);
  auto m      = chebyshev{beta, Fermion, n_cheb};
  auto G      = gf<chebyshev, matrix_valued>{m, {N_matrix, N_matrix}};
  G()         = 0.0;

  for (auto _ : state) {
    for (double tau : tau_points) { benchmark::DoNotOptimize(G(tau)); }
  }
  state.SetItemsProcessed(state.iterations() * N_eval);
}
BENCHMARK(BM_Chebyshev_Matrix)->RangeMultiplier(2)->Range(16, 64)->Unit(benchmark::kMicrosecond);

// Matrix real-valued (2x2 double): tests array code path with real scalars
static void BM_Chebyshev_MatrixReal(benchmark::State &state) {
  long n_cheb = state.range(0);
  auto m      = chebyshev{beta, Fermion, n_cheb};
  auto G      = gf<chebyshev, matrix_real_valued>{m, {N_matrix, N_matrix}};
  G()         = 0.0;

  for (auto _ : state) {
    for (double tau : tau_points) { benchmark::DoNotOptimize(G(tau)); }
  }
  state.SetItemsProcessed(state.iterations() * N_eval);
}
BENCHMARK(BM_Chebyshev_MatrixReal)->RangeMultiplier(2)->Range(16, 64)->Unit(benchmark::kMicrosecond);

// Scalar-valued (complex): tests scalar code path
static void BM_Chebyshev_Scalar(benchmark::State &state) {
  long n_cheb = state.range(0);
  auto m      = chebyshev{beta, Fermion, n_cheb};
  auto G      = gf<chebyshev, scalar_valued>{m};
  G()         = 0.0;

  for (auto _ : state) {
    for (double tau : tau_points) { benchmark::DoNotOptimize(G(tau)); }
  }
  state.SetItemsProcessed(state.iterations() * N_eval);
}
BENCHMARK(BM_Chebyshev_Scalar)->RangeMultiplier(2)->Range(16, 64)->Unit(benchmark::kMicrosecond);

// Scalar real-valued (double): comparable to diagmc chebfun_t implementation
static void BM_Chebyshev_ScalarReal(benchmark::State &state) {
  long n_cheb = state.range(0);
  auto m      = chebyshev{beta, Fermion, n_cheb};
  auto G      = gf<chebyshev, scalar_real_valued>{m};
  G()         = 0.0;

  for (auto _ : state) {
    for (double tau : tau_points) { benchmark::DoNotOptimize(G(tau)); }
  }
  state.SetItemsProcessed(state.iterations() * N_eval);
}
BENCHMARK(BM_Chebyshev_ScalarReal)->RangeMultiplier(2)->Range(16, 64)->Unit(benchmark::kMicrosecond);

// ============================================================================
// Product mesh benchmarks: cyclat x chebyshev
//
// Tests Chebyshev evaluation in a product mesh context, where the inner
// Chebyshev interpolation receives expression templates from the cyclat lookup.
// This exercises the f_returns_expression code path in chebyshev::evaluate().
// ============================================================================

// Product mesh with matrix-valued target
static void BM_ProductMesh_CyclatChebyshev_Matrix(benchmark::State &state) {
  long n_cheb = state.range(0);
  auto bl     = bravais_lattice{nda::eye<double>(2)};
  auto m_r    = cyclat{bl, L_lattice};
  auto m_cheb = chebyshev{beta, Fermion, n_cheb};
  auto G      = gf<prod<cyclat, chebyshev>, matrix_valued>{m_r * m_cheb, {N_matrix, N_matrix}};
  G()         = 0.0;

  for (auto _ : state) {
    for (int i = 0; i < N_eval; ++i) { benchmark::DoNotOptimize(G(r_points[i], tau_points[i])); }
  }
  state.SetItemsProcessed(state.iterations() * N_eval);
  state.counters["cyclat_size"] = m_r.size();
}
BENCHMARK(BM_ProductMesh_CyclatChebyshev_Matrix)->RangeMultiplier(2)->Range(16, 64)->Unit(benchmark::kMicrosecond);

// Product mesh with scalar-valued target (simpler target type)
static void BM_ProductMesh_CyclatChebyshev_Scalar(benchmark::State &state) {
  long n_cheb = state.range(0);
  auto bl     = bravais_lattice{nda::eye<double>(2)};
  auto m_r    = cyclat{bl, L_lattice};
  auto m_cheb = chebyshev{beta, Fermion, n_cheb};
  auto G      = gf<prod<cyclat, chebyshev>, scalar_valued>{m_r * m_cheb};
  G()         = 0.0;

  for (auto _ : state) {
    for (int i = 0; i < N_eval; ++i) { benchmark::DoNotOptimize(G(r_points[i], tau_points[i])); }
  }
  state.SetItemsProcessed(state.iterations() * N_eval);
  state.counters["cyclat_size"] = m_r.size();
}
BENCHMARK(BM_ProductMesh_CyclatChebyshev_Scalar)->RangeMultiplier(2)->Range(16, 64)->Unit(benchmark::kMicrosecond);

// ============================================================================
// Comparison benchmarks: Same accuracy level across mesh types
//
// For fair comparison at comparable representation power:
// - DLR with w_max=10, eps=1e-10 gives rank ~50
// - Chebyshev with N=50 provides similar accuracy for smooth functions
// - Imtime needs ~4096 points for comparable accuracy
// ============================================================================

static void BM_Compare_DLR(benchmark::State &state) {
  auto m = dlr{beta, Fermion, dlr_wmax, dlr_eps};
  auto G = gf<dlr, matrix_valued>{m, {N_matrix, N_matrix}};
  G()    = 0.0;

  for (auto _ : state) {
    for (double tau : tau_points) { benchmark::DoNotOptimize(G(tau)); }
  }
  state.SetItemsProcessed(state.iterations() * N_eval);
  state.SetLabel("rank=" + std::to_string(m.size()));
}
BENCHMARK(BM_Compare_DLR)->Unit(benchmark::kMicrosecond);

static void BM_Compare_Chebyshev(benchmark::State &state) {
  long n_cheb = 50;
  auto m      = chebyshev{beta, Fermion, n_cheb};
  auto G      = gf<chebyshev, matrix_valued>{m, {N_matrix, N_matrix}};
  G()         = 0.0;

  for (auto _ : state) {
    for (double tau : tau_points) { benchmark::DoNotOptimize(G(tau)); }
  }
  state.SetItemsProcessed(state.iterations() * N_eval);
  state.SetLabel("N=" + std::to_string(n_cheb));
}
BENCHMARK(BM_Compare_Chebyshev)->Unit(benchmark::kMicrosecond);

static void BM_Compare_Imtime(benchmark::State &state) {
  long n_tau = 4096;
  auto m     = imtime{beta, Fermion, n_tau};
  auto G     = gf<imtime, matrix_valued>{m, {N_matrix, N_matrix}};
  G()        = 0.0;

  for (auto _ : state) {
    for (double tau : tau_points) { benchmark::DoNotOptimize(G(tau)); }
  }
  state.SetItemsProcessed(state.iterations() * N_eval);
  state.SetLabel("n_tau=" + std::to_string(n_tau));
}
BENCHMARK(BM_Compare_Imtime)->Unit(benchmark::kMicrosecond);

// ============================================================================
// Direct comparison: TRIQS vs diagmc-style reference implementation
//
// Both use scalar real-valued (double) for fair comparison.
// ============================================================================

static void BM_Reference_Chebfun(benchmark::State &state) {
  long n_cheb = state.range(0);

  // Create reference chebfun_t with same parameters as TRIQS
  reference::chebfun_t cheb(n_cheb, 0.0, beta);
  for (long i = 0; i < cheb.size(); ++i) { cheb.value(i) = 0.0; }

  for (auto _ : state) {
    for (double tau : tau_points) { benchmark::DoNotOptimize(cheb(tau)); }
  }
  state.SetItemsProcessed(state.iterations() * N_eval);
}
BENCHMARK(BM_Reference_Chebfun)->RangeMultiplier(2)->Range(16, 64)->Unit(benchmark::kMicrosecond);

// Direct comparison at N=50 (typical use case)
static void BM_DirectCompare_Reference(benchmark::State &state) {
  long n_cheb = 50;
  reference::chebfun_t cheb(n_cheb, 0.0, beta);
  for (long i = 0; i < cheb.size(); ++i) { cheb.value(i) = 0.0; }

  for (auto _ : state) {
    for (double tau : tau_points) { benchmark::DoNotOptimize(cheb(tau)); }
  }
  state.SetItemsProcessed(state.iterations() * N_eval);
  state.SetLabel("diagmc-style");
}
BENCHMARK(BM_DirectCompare_Reference)->Unit(benchmark::kMicrosecond);

static void BM_DirectCompare_TRIQS(benchmark::State &state) {
  long n_cheb = 50;
  auto m      = chebyshev{beta, Fermion, n_cheb};
  auto G      = gf<chebyshev, scalar_real_valued>{m};
  G()         = 0.0;

  for (auto _ : state) {
    for (double tau : tau_points) { benchmark::DoNotOptimize(G(tau)); }
  }
  state.SetItemsProcessed(state.iterations() * N_eval);
  state.SetLabel("TRIQS");
}
BENCHMARK(BM_DirectCompare_TRIQS)->Unit(benchmark::kMicrosecond);

BENCHMARK_MAIN();
