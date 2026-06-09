// Copyright (c) 2017--present, The Simons Foundation
// This file is part of TRIQS and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

// Rank-1 NFFT buffer benchmarks: GM/MG (unique_n1 targets) and M_iw (DLR targets)
// Benchmark argument is k (perturbation order). Each rank-1 buffer gets k pushes.

#include "bench_nfft_common.hpp"

// =============================================================================
// FullGrid: uniform imfreq (type1 FINUFFT)
// =============================================================================

static void BM_Rank1_FullGrid_FINUFFT(benchmark::State &state) {
  auto &md         = get_dlr2d_mesh_data();
  int64_t n_points = state.range(0);

  int64_t grid_size = 2 * (md.max_n + 1);
  nda::array<dcomplex, 1> output(grid_size);
  output = 0;

  buffer_t<1> buf{output, buf_size, beta, tol};
  auto [taus, vals] = gen_random_data<1>(n_points);

  run_nfft_bench<1>(state, buf, taus, vals, output);
}

// =============================================================================
// UniqueN1: unique first-frequency projection from DLR2D mesh
// =============================================================================

static void BM_Rank1_UniqueN1_FINUFFT(benchmark::State &state) {
  auto &md         = get_dlr2d_mesh_data();
  int64_t n_points = state.range(0);

  nda::array<dcomplex, 1> output(md.n_un1);
  output = 0;

  buffer_t<1> buf{output, md.target_mf_n1, buf_size, tol, type_t::type3};
  auto [taus, vals] = gen_random_data<1>(n_points);

  run_nfft_bench<1>(state, buf, taus, vals, output);
}

static void BM_Rank1_UniqueN1_Sincos(benchmark::State &state) {
  auto &md         = get_dlr2d_mesh_data();
  int64_t n_points = state.range(0);

  nda::array<dcomplex, 1> output(md.n_un1);
  output = 0;

  buffer_t<1> buf{output, md.target_mf_n1, buf_size, tol, type_t::direct_type1};
  auto [taus, vals] = gen_random_data<1>(n_points);

  run_nfft_bench<1>(state, buf, taus, vals, output);
}

// Bitwise kernel removed - benchmark disabled
// static void BM_Rank1_UniqueN1_Bitwise(benchmark::State &state) {
//   auto &md         = get_dlr2d_mesh_data();
//   int64_t n_points = state.range(0);
//
//   nda::array<dcomplex, 1> output(md.n_un1);
//   output = 0;
//
//   buffer_t<1> buf{output, md.target_mf_n1, buf_size, tol, type_t::direct_bitwise};
//   auto [taus, vals] = gen_random_data<1>(n_points);
//
//   run_nfft_bench<1>(state, buf, taus, vals, output);
// }

static void BM_Rank1_UniqueN1_NAF(benchmark::State &state) {
  auto &md         = get_dlr2d_mesh_data();
  int64_t n_points = state.range(0);

  nda::array<dcomplex, 1> output(md.n_un1);
  output = 0;

  buffer_t<1> buf{output, md.target_mf_n1, buf_size, tol, type_t::direct_type3};
  auto [taus, vals] = gen_random_data<1>(n_points);

  run_nfft_bench<1>(state, buf, taus, vals, output);
}

static void BM_Rank1_UniqueN1_Chain(benchmark::State &state) {
  auto &md         = get_dlr2d_mesh_data();
  int64_t n_points = state.range(0);

  nda::array<dcomplex, 1> output(md.n_un1);
  output = 0;

  buffer_t<1> buf{output, md.target_mf_n1, buf_size, tol, type_t::direct_chain};
  auto [taus, vals] = gen_random_data<1>(n_points);

  run_nfft_bench<1>(state, buf, taus, vals, output);
}

static void BM_Rank1_UniqueN1_Auto(benchmark::State &state) {
  auto &md         = get_dlr2d_mesh_data();
  int64_t n_points = state.range(0);

  nda::array<dcomplex, 1> output(md.n_un1);
  output = 0;

  buffer_t<1> buf{output, md.target_mf_n1, buf_size, tol, type_t::automatic};
  auto [taus, vals] = gen_random_data<1>(n_points);

  run_nfft_bench<1>(state, buf, taus, vals, output);
}

// =============================================================================
// DLR: pure DLR mesh targets (M_iw pattern)
// =============================================================================

static void BM_Rank1_DLR_FINUFFT(benchmark::State &state) {
  auto &dlr_md     = get_dlr_mesh_data();
  int64_t n_points = state.range(0);

  nda::array<dcomplex, 1> output(dlr_md.n_dlr_pts);
  output = 0;

  buffer_t<1> buf{output, dlr_md.target_mf, buf_size, tol, type_t::type3};
  auto [taus, vals] = gen_random_data<1>(n_points);

  run_nfft_bench<1>(state, buf, taus, vals, output);
}

static void BM_Rank1_DLR_Sincos(benchmark::State &state) {
  auto &dlr_md     = get_dlr_mesh_data();
  int64_t n_points = state.range(0);

  nda::array<dcomplex, 1> output(dlr_md.n_dlr_pts);
  output = 0;

  buffer_t<1> buf{output, dlr_md.target_mf, buf_size, tol, type_t::direct_type1};
  auto [taus, vals] = gen_random_data<1>(n_points);

  run_nfft_bench<1>(state, buf, taus, vals, output);
}

// Bitwise kernel removed - benchmark disabled
// static void BM_Rank1_DLR_Bitwise(benchmark::State &state) {
//   auto &dlr_md     = get_dlr_mesh_data();
//   int64_t n_points = state.range(0);
//
//   nda::array<dcomplex, 1> output(dlr_md.n_dlr_pts);
//   output = 0;
//
//   buffer_t<1> buf{output, dlr_md.target_mf, buf_size, tol, type_t::direct_bitwise};
//   auto [taus, vals] = gen_random_data<1>(n_points);
//
//   run_nfft_bench<1>(state, buf, taus, vals, output);
// }

static void BM_Rank1_DLR_NAF(benchmark::State &state) {
  auto &dlr_md     = get_dlr_mesh_data();
  int64_t n_points = state.range(0);

  nda::array<dcomplex, 1> output(dlr_md.n_dlr_pts);
  output = 0;

  buffer_t<1> buf{output, dlr_md.target_mf, buf_size, tol, type_t::direct_type3};
  auto [taus, vals] = gen_random_data<1>(n_points);

  run_nfft_bench<1>(state, buf, taus, vals, output);
}

static void BM_Rank1_DLR_Chain(benchmark::State &state) {
  auto &dlr_md     = get_dlr_mesh_data();
  int64_t n_points = state.range(0);

  nda::array<dcomplex, 1> output(dlr_md.n_dlr_pts);
  output = 0;

  buffer_t<1> buf{output, dlr_md.target_mf, buf_size, tol, type_t::direct_chain};
  auto [taus, vals] = gen_random_data<1>(n_points);

  run_nfft_bench<1>(state, buf, taus, vals, output);
}

static void BM_Rank1_DLR_Auto(benchmark::State &state) {
  auto &dlr_md     = get_dlr_mesh_data();
  int64_t n_points = state.range(0);

  nda::array<dcomplex, 1> output(dlr_md.n_dlr_pts);
  output = 0;

  buffer_t<1> buf{output, dlr_md.target_mf, buf_size, tol, type_t::automatic};
  auto [taus, vals] = gen_random_data<1>(n_points);

  run_nfft_bench<1>(state, buf, taus, vals, output);
}

// clang-format off
BENCHMARK(BM_Rank1_FullGrid_FINUFFT) K_ARGS;

BENCHMARK(BM_Rank1_UniqueN1_FINUFFT) K_ARGS;
BENCHMARK(BM_Rank1_UniqueN1_Sincos) K_ARGS;
// BENCHMARK(BM_Rank1_UniqueN1_Bitwise) K_ARGS;  // Bitwise kernel removed
BENCHMARK(BM_Rank1_UniqueN1_NAF) K_ARGS;
BENCHMARK(BM_Rank1_UniqueN1_Chain) K_ARGS;
BENCHMARK(BM_Rank1_UniqueN1_Auto) K_ARGS;

BENCHMARK(BM_Rank1_DLR_FINUFFT) K_ARGS;
BENCHMARK(BM_Rank1_DLR_Sincos) K_ARGS;
// BENCHMARK(BM_Rank1_DLR_Bitwise) K_ARGS;  // Bitwise kernel removed
BENCHMARK(BM_Rank1_DLR_NAF) K_ARGS;
BENCHMARK(BM_Rank1_DLR_Chain) K_ARGS;
BENCHMARK(BM_Rank1_DLR_Auto) K_ARGS;
// clang-format on
