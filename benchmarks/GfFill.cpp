#include <benchmark/benchmark.h>
#include <triqs/gfs.hpp>
#include <triqs/mesh.hpp>
#include <nda/nda.hpp>

using namespace triqs::gfs;
using namespace triqs::mesh;

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

  auto iw_mesh_large = gf_mesh<imfreq>{1.0, Fermion, 2 * n_iw};
  auto M             = gf<prod<imfreq, imfreq>, scalar_valued>{{iw_mesh_large, iw_mesh_large}};

  auto iw_mesh = gf_mesh<imfreq>{1.0, Fermion, n_iw};
  auto iW_mesh = gf_mesh<imfreq>{1.0, Boson, n_iw};
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

  triqs::clef::placeholder<0> iW;
  triqs::clef::placeholder<1> iw1;
  triqs::clef::placeholder<2> iw2;

  auto iw_mesh_large = gf_mesh<imfreq>{1.0, Fermion, 2 * n_iw};
  auto M             = gf<prod<imfreq, imfreq>, scalar_valued>{{iw_mesh_large, iw_mesh_large}};

  auto iw_mesh = gf_mesh<imfreq>{1.0, Fermion, n_iw};
  auto iW_mesh = gf_mesh<imfreq>{1.0, Boson, n_iw};
  auto M4      = gf<prod<imfreq, imfreq, imfreq>, scalar_valued>{{iW_mesh, iw_mesh, iw_mesh}};

  //for (auto _ : state) M4[iW, iw1_, iw2_] << M4[iW, iw1_, iw2_] + M[iw1_, iW + iw1_] * M[iW + iw2_, iw2_]; FIXME Does not work!
  for (auto _ : state) M4(iW, iw1, iw2) << M4(iW, iw1, iw2) + M(iw1, iW + iw1) * M(iW + iw2, iw2);

  state.SetBytesProcessed(int64_t(state.iterations()) * 8 * n_iw * n_iw * n_iw * sizeof(scalar_t));
}
BENCHMARK(GfFillClef)->RangeMultiplier(2)->Range(1 << 6, 1 << 8); //->Complexity();

BENCHMARK_MAIN();
