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
