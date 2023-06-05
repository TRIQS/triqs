// Copyright (c) 2022-2023 Simons Foundation
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

#include <benchmark/benchmark.h>
#include <triqs/gfs.hpp>
#include <triqs/mesh.hpp>
#include <nda/nda.hpp>

using namespace triqs::gfs;
using namespace triqs;

// ===== Interpolate Green function

static void GfInterpTau(benchmark::State &state) {
  long n_tau    = state.range(0);
  double beta   = 10.0;
  auto tau_mesh = mesh::imtime{beta, Fermion, n_tau};

  int N  = 3;
  auto G = gf<imtime, matrix_valued>{tau_mesh, {N, N}};
  //G()    = 0.0;

  for (auto _ : state) {
    for (auto tau : tau_mesh) benchmark::DoNotOptimize(G(double(tau)));
  }
}
BENCHMARK(GfInterpTau)->RangeMultiplier(2)->Range(1024, 8192);

// ===== Closest Meshpoint

static void ClosestMeshpointTau(benchmark::State &state) {
  long n_tau    = state.range(0);
  double beta   = 10.0;
  auto tau_mesh = mesh::imtime{beta, Fermion, n_tau};

  int N  = 3;
  auto G = gf<imtime, matrix_valued>{tau_mesh, {N, N}};
  //G()    = 0.0;

  for (auto _ : state) {
    for (auto tau : tau_mesh) benchmark::DoNotOptimize(G[closest_mesh_pt(tau.value())]);
  }
}
BENCHMARK(ClosestMeshpointTau)->RangeMultiplier(2)->Range(1024, 8192);

BENCHMARK_MAIN();
