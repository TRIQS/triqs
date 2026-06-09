// Copyright (c) 2017--present, The Simons Foundation
// This file is part of TRIQS and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

// Rank-2 NFFT buffer benchmarks: M pattern (intermediate scattering matrix)
// Benchmark argument is k (perturbation order). Each buffer gets k^2/n_orb^2 pushes.

#include "bench_nfft_common.hpp"

// =============================================================================
// FullGrid: uniform prod<imfreq,imfreq> (type1 FINUFFT)
// =============================================================================

static void BM_Rank2_FullGrid_FINUFFT(benchmark::State &state) {
  auto &md         = get_dlr2d_mesh_data();
  int64_t n_points = n_points_rank2(state.range(0));

  int64_t grid_size = 2 * (md.max_n + 1);
  nda::array<dcomplex, 2> output(grid_size, grid_size);
  output = 0;

  buffer_t<2> buf{output, buf_size, beta, tol};
  auto [taus, vals] = gen_random_data<2>(n_points);

  run_nfft_bench<2>(state, buf, taus, vals, output);
}

// =============================================================================
// DLR2D: sparse DLR2D targets, compressgrid=false
// =============================================================================

static void BM_Rank2_DLR2D_FINUFFT(benchmark::State &state) {
  auto &md         = get_dlr2d_mesh_data();
  int64_t n_points = n_points_rank2(state.range(0));

  nda::array<dcomplex, 1> output(md.n_mesh_points);
  output = 0;

  buffer_t<2> buf{output, md.target_mf_2d, buf_size, tol, type_t::type3};
  auto [taus, vals] = gen_random_data<2>(n_points);

  run_nfft_bench<2>(state, buf, taus, vals, output);
}

static void BM_Rank2_DLR2D_Sincos(benchmark::State &state) {
  auto &md         = get_dlr2d_mesh_data();
  int64_t n_points = n_points_rank2(state.range(0));

  nda::array<dcomplex, 1> output(md.n_mesh_points);
  output = 0;

  buffer_t<2> buf{output, md.target_mf_2d, buf_size, tol, type_t::direct_type1};
  auto [taus, vals] = gen_random_data<2>(n_points);

  run_nfft_bench<2>(state, buf, taus, vals, output);
}

static void BM_Rank2_DLR2D_Chain(benchmark::State &state) {
  auto &md         = get_dlr2d_mesh_data();
  int64_t n_points = n_points_rank2(state.range(0));

  nda::array<dcomplex, 1> output(md.n_mesh_points);
  output = 0;

  buffer_t<2> buf{output, md.target_mf_2d, buf_size, tol, type_t::direct_chain};
  auto [taus, vals] = gen_random_data<2>(n_points);

  run_nfft_bench<2>(state, buf, taus, vals, output);
}

static void BM_Rank2_DLR2D_NAF(benchmark::State &state) {
  auto &md         = get_dlr2d_mesh_data();
  int64_t n_points = n_points_rank2(state.range(0));

  nda::array<dcomplex, 1> output(md.n_mesh_points);
  output = 0;

  buffer_t<2> buf{output, md.target_mf_2d, buf_size, tol, type_t::direct_type3};
  auto [taus, vals] = gen_random_data<2>(n_points);

  run_nfft_bench<2>(state, buf, taus, vals, output);
}

static void BM_Rank2_DLR2D_Type1Gather(benchmark::State &state) {
  auto &md         = get_dlr2d_mesh_data();
  int64_t n_points = n_points_rank2(state.range(0));

  nda::array<dcomplex, 1> output(md.n_mesh_points);
  output = 0;

  buffer_t<2> buf{output, md.target_mf_2d, buf_size, tol, type_t::type1_gather};
  auto [taus, vals] = gen_random_data<2>(n_points);

  run_nfft_bench<2>(state, buf, taus, vals, output);
}

static void BM_Rank2_DLR2D_Auto(benchmark::State &state) {
  auto &md         = get_dlr2d_mesh_data();
  int64_t n_points = n_points_rank2(state.range(0));

  nda::array<dcomplex, 1> output(md.n_mesh_points);
  output = 0;

  buffer_t<2> buf{output, md.target_mf_2d, buf_size, tol, type_t::automatic};
  auto [taus, vals] = gen_random_data<2>(n_points);

  run_nfft_bench<2>(state, buf, taus, vals, output);
}

// =============================================================================
// DLR2D_CG: sparse DLR2D targets, compressgrid=true
// =============================================================================

static void BM_Rank2_DLR2D_CG_FINUFFT(benchmark::State &state) {
  auto &md         = get_dlr2d_mesh_data_cg();
  int64_t n_points = n_points_rank2(state.range(0));

  nda::array<dcomplex, 1> output(md.n_mesh_points);
  output = 0;

  buffer_t<2> buf{output, md.target_mf_2d, buf_size, tol, type_t::type3};
  auto [taus, vals] = gen_random_data<2>(n_points);

  run_nfft_bench<2>(state, buf, taus, vals, output);
}

static void BM_Rank2_DLR2D_CG_Type1Gather(benchmark::State &state) {
  auto &md         = get_dlr2d_mesh_data_cg();
  int64_t n_points = n_points_rank2(state.range(0));

  nda::array<dcomplex, 1> output(md.n_mesh_points);
  output = 0;

  buffer_t<2> buf{output, md.target_mf_2d, buf_size, tol, type_t::type1_gather};
  auto [taus, vals] = gen_random_data<2>(n_points);

  run_nfft_bench<2>(state, buf, taus, vals, output);
}

static void BM_Rank2_DLR2D_CG_NAF(benchmark::State &state) {
  auto &md         = get_dlr2d_mesh_data_cg();
  int64_t n_points = n_points_rank2(state.range(0));

  nda::array<dcomplex, 1> output(md.n_mesh_points);
  output = 0;

  buffer_t<2> buf{output, md.target_mf_2d, buf_size, tol, type_t::direct_type3};
  auto [taus, vals] = gen_random_data<2>(n_points);

  run_nfft_bench<2>(state, buf, taus, vals, output);
}

static void BM_Rank2_DLR2D_CG_Auto(benchmark::State &state) {
  auto &md         = get_dlr2d_mesh_data_cg();
  int64_t n_points = n_points_rank2(state.range(0));

  nda::array<dcomplex, 1> output(md.n_mesh_points);
  output = 0;

  buffer_t<2> buf{output, md.target_mf_2d, buf_size, tol, type_t::automatic};
  auto [taus, vals] = gen_random_data<2>(n_points);

  run_nfft_bench<2>(state, buf, taus, vals, output);
}

// clang-format off
BENCHMARK(BM_Rank2_FullGrid_FINUFFT) K_ARGS;

BENCHMARK(BM_Rank2_DLR2D_FINUFFT) K_ARGS;
BENCHMARK(BM_Rank2_DLR2D_Type1Gather) K_ARGS;
BENCHMARK(BM_Rank2_DLR2D_Sincos) K_ARGS;
BENCHMARK(BM_Rank2_DLR2D_Chain) K_ARGS;
BENCHMARK(BM_Rank2_DLR2D_NAF) K_ARGS;
BENCHMARK(BM_Rank2_DLR2D_Auto) K_ARGS;

BENCHMARK(BM_Rank2_DLR2D_CG_FINUFFT) K_ARGS;
BENCHMARK(BM_Rank2_DLR2D_CG_Type1Gather) K_ARGS;
BENCHMARK(BM_Rank2_DLR2D_CG_NAF) K_ARGS;
BENCHMARK(BM_Rank2_DLR2D_CG_Auto) K_ARGS;
// clang-format on
