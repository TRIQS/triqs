// Copyright (c) 2017--present, The Simons Foundation
// This file is part of TRIQS and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#pragma once
#include <nda/nda.hpp>
#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <memory>
#include <vector>
#include <triqs/mesh/matsubara_freq.hpp>

#include "finufft.h"
#include "sincos.hpp"
#include <xsimd/xsimd.hpp>
#include <poet/poet.hpp>

namespace triqs::utility::nfft {

  using nda::array_view;
  using dcomplex = std::complex<double>;

  // cis(theta) = e^{i theta}. High-accuracy callers use a fused libm sincos via
  // the compiler builtin where available, falling back to separate std::sin/std::cos
  // on compilers that lack __builtin_sincos; lower tolerances use the polynomial
  // approximation from `sincos.hpp`.
  template <int TolDigits = 12> inline dcomplex cis(double theta) {
    if constexpr (TolDigits >= 12) {
      double s, c;
#if defined(__has_builtin) && __has_builtin(__builtin_sincos)
      __builtin_sincos(theta, &s, &c);
#else
      s = std::sin(theta);
      c = std::cos(theta);
#endif
      return {c, s};
    } else {
      auto [s, c] = triqs::utility::math::sincos<TolDigits>(theta);
      return {c, s};
    }
  }

  inline void check_finufft(int err) {
    if (err > 0) NDA_RUNTIME_ERROR << "Error in FINUFFT: " << err << "\n";
  }

  using finufft_plan_ptr = std::unique_ptr<finufft_plan_s, decltype([](finufft_plan p) {
                                             if (p) finufft_destroy(p);
                                           })>;

  enum class type_t { automatic, type1, type1_gather, type3, direct_type1, direct_type3, direct_chain };

  using target_mf_t = mesh::matsubara_freq;

  // ---- Shared state between buffer_t and kernel structs ----

  template <int Rank> struct shared_state_t {
    // Buffer geometry and current fill level.
    int buf_size    = 0;
    double beta     = 0;
    int buf_counter = 0;

    // Buffered source points (τ_j, f_j).
    nda::array<double, 2> x_arr;  // (Rank, buf_size)
    nda::vector<dcomplex> fx_arr; // (buf_size)

    // Output / target metadata shared by the direct kernels.
    nda::vector<dcomplex> fk_vec; // (n_targets) -- output for direct kernels
    int64_t n_targets = 0;
    nda::array<long, 2> target_n; // (Rank, n_targets) -- Matsubara indices

    void init_direct_common(std::vector<std::array<target_mf_t, Rank>> const &target_mf) {
      beta = target_mf[0][0].beta;
      target_n.resize(Rank, n_targets);
      // Direct kernels only need the fermionic indices n_r, not the full mf objects.
      for (int r = 0; r < Rank; ++r)
        for (int64_t d = 0; d < n_targets; ++d) target_n(r, d) = target_mf[d][r].n;
    }

    // Round up to SIMD boundary to eliminate scalar tail loops
    static constexpr int round_up_simd(int n) {
      constexpr int simd_sz = xsimd::batch<dcomplex>::size;
      return ((n + simd_sz - 1) / simd_sz) * simd_sz;
    }
  };

  // ---- Math utilities ----

  // |2n+1| for fermionic Matsubara index n
  constexpr unsigned long odd_exponent_abs(long n) {
    long odd = 2 * n + 1;
    return static_cast<unsigned long>(odd >= 0 ? odd : -odd);
  }

  using tol_digits_seq_t = std::integer_sequence<int, 6, 8, 10, 12>;

  // Collapse a runtime tolerance to the small set of compile-time kernels we support.
  constexpr int tol_digits_bucket(double tol) {
    if (tol <= 1e-12) return 12;
    if (tol <= 1e-10) return 10;
    if (tol <= 1e-8) return 8;
    return 6;
  }

  // NAF (Non-Adjacent Form) decomposition of n into signed binary digits.
  // Returns encoded digits: k for +1 at bit k, -(k+1) for -1 at bit k.
  inline std::vector<int> compute_naf(unsigned long n) {
    std::vector<int> digits;
    long sn = static_cast<long>(n);
    for (int k = 0; sn > 0; ++k, sn >>= 1) {
      if (sn & 1) {
        int r = 2 - static_cast<int>(sn & 3); // +1 if sn%4==1, -1 if sn%4==3
        digits.push_back(r > 0 ? k : -(k + 1));
        sn -= r;
      }
    }
    return digits;
  }

  // Map raw imaginary-time samples to the FINUFFT type-1 convention:
  //
  //   x_r = 2π(τ_r / β - 1/2),
  //   f_j <- f_j exp(i π Σ_r τ_{rj} / β).
  //
  // The half-grid shift turns odd Matsubara frequencies into an integer-mode FFT.
  template <int TolDigits = 12, int Rank> [[gnu::flatten]] void apply_type1_coord_transform(shared_state_t<Rank> &state, int n) {
    using dbatch = xsimd::batch<double>;
    using cbatch = xsimd::batch<dcomplex>;

    if (n <= 0) return;

    double const inv_beta    = 1.0 / state.beta;
    constexpr int simd_width = static_cast<int>(dbatch::size);
    int const simd_n         = n & -simd_width;

    std::array<double *, Rank> x_ptr{};
    for (int r = 0; r < Rank; ++r) x_ptr[r] = &state.x_arr(r, 0);
    dcomplex *fx_ptr = state.fx_arr.data();

    dbatch const inv_beta_vec(inv_beta);
    dbatch const half_vec(0.5);
    dbatch const two_pi_vec(2.0 * M_PI);
    dbatch const pi_over_beta_vec(M_PI * inv_beta);

    int j = 0;
    for (; j < simd_n; j += simd_width) {
      dbatch tau_sum(0.0);
      for (int r = 0; r < Rank; ++r) {
        dbatch tau = dbatch::load_unaligned(x_ptr[r] + j);
        tau_sum += tau;
        // x_r = 2π(τ_r / β - 1/2) puts the nodes in the NUFFT convention.
        (two_pi_vec * (tau * inv_beta_vec - half_vec)).store_unaligned(x_ptr[r] + j);
      }

      // Multiply by exp(i π Σ_r τ_r / β) so odd Matsubara modes become an integer FFT.
      // Always use the full-precision bucket: this phase enters the data and would
      // otherwise dominate the transform's error at low tolerances.
      auto [sin_theta, cos_theta] = triqs::utility::math::sincos<12>(pi_over_beta_vec * tau_sum);
      cbatch phase(cos_theta, sin_theta);
      (cbatch::load_unaligned(fx_ptr + j) * phase).store_unaligned(fx_ptr + j);
    }

    for (; j < n; ++j) {
      double tau_sum = 0.0;
      for (int r = 0; r < Rank; ++r) {
        double tau = state.x_arr(r, j);
        tau_sum += tau;
        state.x_arr(r, j) = 2 * M_PI * (tau * inv_beta - 0.5);
      }
      // Scalar tail matches the SIMD path above.
      state.fx_arr[j] *= cis<12>(M_PI * tau_sum * inv_beta);
    }
  }

  // ---- SIMD type aliases and helpers ----

  using cbatch                           = xsimd::batch<dcomplex>;
  static constexpr std::size_t simd_size = cbatch::size;

  // Number of target accumulators kept live to expose ILP in the direct kernels.
  static constexpr int ilp_unroll = poet::vector_register_count() <= 16 ? 2 : 4;

  // Accumulate
  //
  //   F_d += Σ_j f_j p_d(j)
  //
  // for `Unroll` targets at once over a SIMD-rounded source block.
  // The inner loop reuses the same SIMD source packet `f_j` across several
  // targets, which is where the extra ILP comes from.
  template <int Unroll, typename SimdPowFunc>
  [[gnu::always_inline]] inline void accumulate_targets_ilp(int64_t n_targets, int n_sources_simd, dcomplex *fx_data, dcomplex *fiw_ptr,
                                                            SimdPowFunc &&compute_simd_pow) {
    static_assert(Unroll > 0 && ((Unroll & (Unroll - 1)) == 0));
    if (n_targets == 0 || n_sources_simd == 0) return;
    constexpr int64_t unroll_mask = ~int64_t{Unroll - 1};
    int64_t const n_targets_main  = n_targets & unroll_mask;
    int64_t d                     = 0;

    for (; d < n_targets_main; d += Unroll) {
      std::array<cbatch, Unroll> acc{};
      for (int j = 0; j < n_sources_simd; j += simd_size) {
        cbatch fj = cbatch::load_unaligned(fx_data + j);
        // One source SIMD packet updates `Unroll` target accumulators.
        poet::static_for<Unroll>([&](const auto i) { acc[i] = xsimd::fma(fj, compute_simd_pow(d + i, j), acc[i]); });
      }

      // Reduce each target accumulator once, after the full source sweep.
      poet::static_for<Unroll>([&](const auto i) { fiw_ptr[d + i] += xsimd::reduce_add(acc[i]); });
    }

    // Tail: same formula, just without target blocking.
    for (; d < n_targets; ++d) {
      cbatch acc{};
      for (int j = 0; j < n_sources_simd; j += simd_size) acc = xsimd::fma(cbatch::load_unaligned(fx_data + j), compute_simd_pow(d, j), acc);
      fiw_ptr[d] += xsimd::reduce_add(acc);
    }
  }

} // namespace triqs::utility::nfft
