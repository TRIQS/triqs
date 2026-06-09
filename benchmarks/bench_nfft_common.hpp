// Copyright (c) 2017--present, The Simons Foundation
// This file is part of TRIQS and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#pragma once
#include <benchmark/benchmark.h>
#include <triqs/utility/nfft/buffer.hpp>
#include <triqs/mesh.hpp>
#include <triqs/mesh/matsubara_freq.hpp>
#include <random>
#include <set>

using namespace triqs::utility;
using namespace triqs::utility::nfft;
using namespace triqs::mesh;
using dcomplex = std::complex<double>;

// Benchmark parameters
static constexpr double beta     = 160.0;
static constexpr double dlr_wmax = 5.0;
static constexpr double dlr_eps  = 1e-8;
static constexpr int buf_size = 100000;
static constexpr int n_orb    = 1; // number of orbitals per block; n_points_per_buffer = k^2 / n_orb^2
static constexpr double tol   = 1e-8;

// --- Mesh data structs (initialized once per process) ---

// DLR2D mesh data (compressgrid=false): 2D targets + unique n1 projection
struct DLR2DMeshData {
  int64_t n_mesh_points;
  int64_t max_n;
  int64_t n_un1;
  std::vector<std::array<matsubara_freq, 2>> target_mf_2d;
  std::vector<matsubara_freq> target_mf_n1;

  DLR2DMeshData() {
    dlr2d_imfreq mesh{beta, dlr_wmax, dlr_eps, PH};
    n_mesh_points = mesh.size();
    max_n         = mesh.max_n();

    std::set<long> unique_n1_set;
    for (auto mp : mesh) {
      auto [n1, n2] = mp.index();
      unique_n1_set.insert(n1);
    }
    std::vector<long> unique_n1(unique_n1_set.begin(), unique_n1_set.end());
    n_un1 = static_cast<int64_t>(unique_n1.size());

    target_mf_2d.reserve(n_mesh_points);
    for (long d = 0; d < n_mesh_points; ++d) {
      auto [n1, n2] = mesh.to_index(d);
      target_mf_2d.push_back({matsubara_freq(n2, beta, Fermion), matsubara_freq(n1, beta, Fermion)});
    }

    target_mf_n1.reserve(n_un1);
    for (int64_t k = 0; k < n_un1; ++k) target_mf_n1.push_back(matsubara_freq(unique_n1[k], beta, Fermion));
  }
};

// DLR2D mesh data with compressgrid=true
struct DLR2DMeshDataCG {
  int64_t n_mesh_points;
  std::vector<std::array<matsubara_freq, 2>> target_mf_2d;

  DLR2DMeshDataCG() {
    dlr2d_imfreq mesh{beta, dlr_wmax, dlr_eps, PH, /*compressgrid=*/true};
    n_mesh_points = mesh.size();

    target_mf_2d.reserve(n_mesh_points);
    for (long d = 0; d < n_mesh_points; ++d) {
      auto [n1, n2] = mesh.to_index(d);
      target_mf_2d.push_back({matsubara_freq(n2, beta, Fermion), matsubara_freq(n1, beta, Fermion)});
    }
  }
};

// Pure DLR mesh data (M_iw pattern)
struct DLRMeshData {
  int64_t n_dlr_pts;
  std::vector<matsubara_freq> target_mf;

  DLRMeshData() {
    dlr_imfreq mesh{beta, Fermion, dlr_wmax, dlr_eps};
    n_dlr_pts = mesh.size();

    target_mf.reserve(n_dlr_pts);
    for (auto w : mesh) target_mf.push_back(w);
  }
};

inline DLR2DMeshData &get_dlr2d_mesh_data() {
  static DLR2DMeshData data;
  return data;
}

inline DLR2DMeshDataCG &get_dlr2d_mesh_data_cg() {
  static DLR2DMeshDataCG data;
  return data;
}

inline DLRMeshData &get_dlr_mesh_data() {
  static DLRMeshData data;
  return data;
}

// --- Benchmark helper ---
// Runs the standard NFFT benchmark loop: zero output, push n_points, flush.

template <int Rank>
void run_nfft_bench(benchmark::State &state, buffer_t<Rank> &buf, std::vector<std::array<double, Rank>> const &taus,
                    std::vector<dcomplex> const &vals, auto &output) {
  int64_t n_points = static_cast<int64_t>(taus.size());
  for (auto _ : state) {
    output = 0;
    for (int64_t i = 0; i < n_points; ++i) buf.push_back(taus[i], vals[i]);
    buf.flush();
  }
  state.SetComplexityN(n_points);
}

// Generate random tau/value pairs
template <int Rank> std::pair<std::vector<std::array<double, Rank>>, std::vector<dcomplex>> gen_random_data(int64_t n_points) {
  std::mt19937 rng(42);
  std::uniform_real_distribution<double> tau_dist(0.0, beta);
  std::normal_distribution<double> val_dist(0.0, 1.0);

  std::vector<std::array<double, Rank>> taus(n_points);
  std::vector<dcomplex> vals(n_points);
  for (int64_t i = 0; i < n_points; ++i) {
    for (int r = 0; r < Rank; ++r) taus[i][r] = tau_dist(rng);
    vals[i] = dcomplex(val_dist(rng), val_dist(rng));
  }
  return {std::move(taus), std::move(vals)};
}

// Benchmark argument is k (perturbation order). Rank-2 pushes k^2/n_orb^2 points per buffer.
inline int64_t n_points_rank2(int64_t k) { return std::max<int64_t>(k * k / (n_orb * n_orb), 1); }

// clang-format off
#define K_ARGS ->Arg(16)->Arg(32)->Arg(48)->Arg(64)->Arg(96)->Arg(128)->Unit(benchmark::kMicrosecond)->Complexity()
// clang-format on
