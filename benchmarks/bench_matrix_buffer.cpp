// Benchmark: matrix_buffer_t push_product vs per-element buffer_t<2> push_back
//
// Compares two approaches for computing a matrix-valued 2D DFT on a DLR2D mesh:
//   1. Per-element: n_orb^2 separate buffer_t<2> instances, k^2 push_back calls + flush
//   2. Product-grid: single matrix_buffer_t, one push_product call
//
// Parametrized over n_orb (1, 4) and compress_grid (true, false) to match
// solver benchmark use-cases (Hubbard_Atom/SIAM: n_orb=1, Plaquette: n_orb=4).

#include "bench_nfft_common.hpp"
#include <triqs/utility/nfft/matrix_buffer.hpp>

// --- Per-element push_back implementation ---
template <int NOrb, bool CG>
static void BM_PerElement_Impl(benchmark::State &state) {
  auto &mesh_data = []() -> auto & {
    if constexpr (CG) return get_dlr2d_mesh_data_cg();
    else return get_dlr2d_mesh_data();
  }();
  long k            = state.range(0);
  auto [taus, vals] = gen_random_data<2>(k * k);

  // Random orbital indices
  std::mt19937 rng(123);
  std::uniform_int_distribution<int> orb_dist(0, NOrb - 1);
  std::vector<int> u_x(k), u_y(k);
  for (auto &u : u_x) u = orb_dist(rng);
  for (auto &u : u_y) u = orb_dist(rng);

  // Create output array and per-element buffers
  using layout_t = nda::contiguous_layout_with_stride_order<nda::encode(std::array{0, 2, 1})>;
  auto output    = nda::array<dcomplex, 3, layout_t>(mesh_data.n_mesh_points, NOrb, NOrb);

  nda::array<buffer_t<2>, 2> bufs(NOrb, NOrb);
  for (int i = 0; i < NOrb; ++i)
    for (int j = 0; j < NOrb; ++j) {
      auto slice = output(nda::range::all, i, j);
      bufs(i, j) = buffer_t<2>{slice, mesh_data.target_mf_2d, buf_size, tol};
    }

  for (auto _ : state) {
    output() = 0;
    long idx = 0;
    for (long j = 0; j < k; ++j)
      for (long i = 0; i < k; ++i) {
        bufs(u_x[j], u_y[i]).push_back(taus[idx], vals[idx]);
        ++idx;
      }
    for (auto &buf : bufs) buf.flush();
  }
  state.SetComplexityN(k);
}

// --- matrix_buffer_t push_product implementation ---
template <int NOrb, bool CG>
static void BM_MatBuf_Impl(benchmark::State &state) {
  auto &mesh_data = []() -> auto & {
    if constexpr (CG) return get_dlr2d_mesh_data_cg();
    else return get_dlr2d_mesh_data();
  }();
  long k = state.range(0);

  // Random tau values and orbital indices
  std::mt19937 rng(42);
  std::uniform_real_distribution<double> tau_dist(0.0, beta);
  std::uniform_int_distribution<int> orb_dist(0, NOrb - 1);
  std::normal_distribution<double> val_dist(0.0, 1.0);

  std::vector<double> x(k), y(k);
  std::vector<int> u_x(k), u_y(k);
  for (long j = 0; j < k; ++j) {
    x[j]   = tau_dist(rng);
    u_x[j] = orb_dist(rng);
  }
  for (long i = 0; i < k; ++i) {
    y[i]   = tau_dist(rng);
    u_y[i] = orb_dist(rng);
  }

  auto V = nda::matrix<dcomplex>(k, k);
  for (long j = 0; j < k; ++j)
    for (long i = 0; i < k; ++i) V(j, i) = dcomplex(val_dist(rng), val_dist(rng));

  // Create output and matrix buffer
  using layout_t = nda::contiguous_layout_with_stride_order<nda::encode(std::array{0, 2, 1})>;
  auto output    = nda::array<dcomplex, 3, layout_t>(mesh_data.n_mesh_points, NOrb, NOrb);

  nfft::matrix_buffer_t<layout_t> mbuf{output, mesh_data.target_mf_2d, NOrb};

  for (auto _ : state) {
    output() = 0;
    mbuf.push_product(x.data(), u_x.data(), k, y.data(), u_y.data(), k, V);
  }
  state.SetComplexityN(k);
}

// --- Wrappers for benchmark registration (avoids macro issues with template commas) ---
// clang-format off

// CG=false (solver default)
static void BM_PerElement_1orb(benchmark::State &s)     { BM_PerElement_Impl<1, false>(s); }
static void BM_PerElement_4orb(benchmark::State &s)     { BM_PerElement_Impl<4, false>(s); }
static void BM_PerElement_16orb(benchmark::State &s)    { BM_PerElement_Impl<16, false>(s); }
static void BM_MatBuf_1orb(benchmark::State &s)         { BM_MatBuf_Impl<1, false>(s); }
static void BM_MatBuf_4orb(benchmark::State &s)         { BM_MatBuf_Impl<4, false>(s); }
static void BM_MatBuf_16orb(benchmark::State &s)        { BM_MatBuf_Impl<16, false>(s); }

// CG=true (compressed grid)
static void BM_PerElement_1orb_CG(benchmark::State &s)  { BM_PerElement_Impl<1, true>(s); }
static void BM_PerElement_4orb_CG(benchmark::State &s)  { BM_PerElement_Impl<4, true>(s); }
static void BM_PerElement_16orb_CG(benchmark::State &s) { BM_PerElement_Impl<16, true>(s); }
static void BM_MatBuf_1orb_CG(benchmark::State &s)      { BM_MatBuf_Impl<1, true>(s); }
static void BM_MatBuf_4orb_CG(benchmark::State &s)      { BM_MatBuf_Impl<4, true>(s); }
static void BM_MatBuf_16orb_CG(benchmark::State &s)     { BM_MatBuf_Impl<16, true>(s); }

// k range matches typical perturbation orders: Hubbard_Atom ~41, Plaquette ~75, SIAM ~75
#define MB_K_ARGS Arg(16)->Arg(32)->Arg(48)->Arg(64)->Arg(96)->Arg(128)

// n_orb=1, CG=false (Hubbard_Atom / SIAM pattern)
BENCHMARK(BM_PerElement_1orb)->MB_K_ARGS->Complexity();
BENCHMARK(BM_MatBuf_1orb)->MB_K_ARGS->Complexity();

// n_orb=4, CG=false (Plaquette pattern)
BENCHMARK(BM_PerElement_4orb)->MB_K_ARGS->Complexity();
BENCHMARK(BM_MatBuf_4orb)->MB_K_ARGS->Complexity();

// n_orb=16, CG=false (4x4 cluster)
BENCHMARK(BM_PerElement_16orb)->MB_K_ARGS->Complexity();
BENCHMARK(BM_MatBuf_16orb)->MB_K_ARGS->Complexity();

// n_orb=1, CG=true
BENCHMARK(BM_PerElement_1orb_CG)->MB_K_ARGS->Complexity();
BENCHMARK(BM_MatBuf_1orb_CG)->MB_K_ARGS->Complexity();

// n_orb=4, CG=true
BENCHMARK(BM_PerElement_4orb_CG)->MB_K_ARGS->Complexity();
BENCHMARK(BM_MatBuf_4orb_CG)->MB_K_ARGS->Complexity();

// n_orb=16, CG=true
BENCHMARK(BM_PerElement_16orb_CG)->MB_K_ARGS->Complexity();
BENCHMARK(BM_MatBuf_16orb_CG)->MB_K_ARGS->Complexity();

// clang-format on

BENCHMARK_MAIN();
