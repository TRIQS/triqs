// Copyright (c) 2020-2023 Simons Foundation
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

using scalar_t = std::complex<double>;

// ===== Fill Array Tensor

static void ArrayFill(benchmark::State &state) {
  std::size_t n_iw = state.range(0);
  nda::array<scalar_t, 2> arr{4 * n_iw, 4 * n_iw};
  nda::array<scalar_t, 3> tensor{2 * n_iw + 1, 2 * n_iw, 2 * n_iw};
  for (auto _ : state) {
    for (int q : range(-n_iw, n_iw + 1)) // Bosonic
      for (int m : range(-n_iw, n_iw))   // Fermionic
        for (int n : range(-n_iw, n_iw)) // Fermionic
          tensor(q + n_iw, m + n_iw, n + n_iw) += arr(m + 2 * n_iw, q + m + 2 * n_iw) * arr(q + n + 2 * n_iw, n + 2 * n_iw);
  }
  state.SetBytesProcessed(int64_t(state.iterations()) * 8 * n_iw * n_iw * n_iw * sizeof(scalar_t));
}
BENCHMARK(ArrayFill)->RangeMultiplier(2)->Range(1 << 6, 1 << 8); //->Complexity();

// ===== Fill Green function Tensor Loop

static void GfFillLoop(benchmark::State &state) {
  long n_iw = state.range(0);

  auto iw_mesh_large = mesh::imfreq{1.0, Fermion, 2 * n_iw};
  auto M             = gf<mesh::prod<imfreq, imfreq>, scalar_valued>{{iw_mesh_large, iw_mesh_large}};

  auto iw_mesh = mesh::imfreq{1.0, Fermion, n_iw};
  auto iW_mesh = mesh::imfreq{1.0, Boson, n_iw};
  auto M4      = gf<prod<imfreq, imfreq, imfreq>, scalar_valued>{{iW_mesh, iw_mesh, iw_mesh}};

  for (auto _ : state)
    for (auto iW : iW_mesh)
      for (auto iw1 : iw_mesh)
        for (auto iw2 : iw_mesh) M4[iW, iw1, iw2] += M[iw1, iW + iw1] * M[iW + iw2, iw2];

  state.SetBytesProcessed(int64_t(state.iterations()) * 8 * n_iw * n_iw * n_iw * sizeof(scalar_t));
}
BENCHMARK(GfFillLoop)->RangeMultiplier(2)->Range(1 << 6, 1 << 8); //->Complexity();

// ===== Fill Green function Tensor Clefs

static void GfFillClef(benchmark::State &state) {
  long n_iw = state.range(0);

  nda::clef::placeholder<0> iW;
  nda::clef::placeholder<1> iw1;
  nda::clef::placeholder<2> iw2;

  auto iw_mesh_large = mesh::imfreq{1.0, Fermion, 2 * n_iw};
  auto M             = gf<prod<imfreq, imfreq>, scalar_valued>{{iw_mesh_large, iw_mesh_large}};

  auto iw_mesh = mesh::imfreq{1.0, Fermion, n_iw};
  auto iW_mesh = mesh::imfreq{1.0, Boson, n_iw};
  auto M4      = gf<prod<imfreq, imfreq, imfreq>, scalar_valued>{{iW_mesh, iw_mesh, iw_mesh}};

  for (auto _ : state) M4[iW, iw1, iw2] << M4[iW, iw1, iw2] + M[iw1, iW + iw1] * M[iW + iw2, iw2];

  state.SetBytesProcessed(int64_t(state.iterations()) * 8 * n_iw * n_iw * n_iw * sizeof(scalar_t));
}
BENCHMARK(GfFillClef)->RangeMultiplier(2)->Range(1 << 6, 1 << 8); //->Complexity();

BENCHMARK_MAIN();
