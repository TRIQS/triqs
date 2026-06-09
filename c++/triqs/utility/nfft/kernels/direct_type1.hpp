// Copyright (c) 2017--present, The Simons Foundation
// This file is part of TRIQS and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#pragma once
#include "../common.hpp"
#include <numeric>

namespace triqs::utility::nfft {

  // Direct type-1 evaluates
  //
  //   F(n) += Σ_j f_j ∏_r z_{rj}^{2 n_r + 1},   z_{rj} = exp(i π τ_{rj} / β).
  template <int Rank> struct kernel_direct_type1_t {

    kernel_direct_type1_t() = default;

    kernel_direct_type1_t(shared_state_t<Rank> const &state, std::vector<std::array<target_mf_t, Rank>> const & /*target_mf*/) {
      constexpr long S = static_cast<long>(cbatch::size);

      // Precompute the target box once so execution only depends on the source points.
      for (int r = 0; r < Rank; ++r) {
        auto row      = state.target_n(r, nda::range::all);
        auto [mn, mx] = std::ranges::minmax_element(row);
        n_min_arr[r]      = *mn;
        n_range_arr[r]    = *mx - *mn + 1;
        n_range_padded[r] = ((n_range_arr[r] + S - 1) / S) * S;
      }

      detect_sequential(state);

      if (is_sequential_) {
        // Dense uniform grids can be accumulated recursively with no gathers.
        seq_strides[Rank - 1] = 1;
        for (int r = Rank - 2; r >= 0; --r) seq_strides[r] = seq_strides[r + 1] * n_range_arr[r + 1];
      } else {
        // Irregular targets fall back to gather tables indexed by the requested n_r values.
        for (int r = 0; r < Rank; ++r) pow_row[r].resize(n_range_padded[r]);

        if constexpr (Rank >= 2) {
          init_factored_groups(state);
        } else {
          target_idx.resize(state.n_targets);
          for (int64_t d = 0; d < state.n_targets; ++d) target_idx[d] = 2 * (state.target_n(0, d) - n_min_arr[0]);
        }
      }
    }

    template <int TolDigits = 12> void execute(shared_state_t<Rank> &state) {
      if (is_sequential_)
        execute_sequential<TolDigits>(state);
      else
        execute_gather<TolDigits>(state);
    }

    private:
    // Target box geometry: n_r runs from n_min_arr[r] to n_min_arr[r] + n_range_arr[r] - 1.
    std::array<long, Rank> n_min_arr{};
    std::array<long, Rank> n_range_arr{};
    std::array<long, Rank> n_range_padded{};

    // Dense-grid path metadata.
    std::array<long, Rank> seq_strides{};
    bool is_sequential_ = false;

    // Gather path tables: one odd-power row per rank, or a flat index for Rank 1.
    std::array<nda::vector<dcomplex>, Rank> pow_row;
    std::vector<long> target_idx;

    // Factored accumulation data (Rank >= 2): group targets by rank-0 index
    struct group_t {
      long pow0_offset;    // index into pow_row[0]
      int64_t start;       // start in padded arrays
      int64_t count;       // real targets in this group
      int64_t padded_count; // count rounded up to SIMD width
    };
    // Rank >= 2 grouped gather layout.
    std::vector<group_t> groups_;
    std::vector<long> factored_idx_;    // padded gather indices for ranks 1..R-1
    std::vector<int64_t> scatter_map_;  // grouped slot -> original target index
    int64_t n_padded_targets_ = 0;
    nda::vector<dcomplex> grouped_fk_;  // grouped output before scattering back

    // Build factored group data: sort targets by rank-0 index, group by unique n0 values.
    // Groups are padded to SIMD width to eliminate scalar tails in the hot loop.
    void init_factored_groups(shared_state_t<Rank> const &state) {
      constexpr int64_t S = static_cast<int64_t>(cbatch::size);

      // Sort target indices by rank-0 Matsubara index
      std::vector<int64_t> perm(state.n_targets);
      std::iota(perm.begin(), perm.end(), 0);
      std::sort(perm.begin(), perm.end(), [&](int64_t a, int64_t b) { return state.target_n(0, a) < state.target_n(0, b); });

      // Pass 1: identify groups and compute total padded size
      groups_.reserve(n_range_arr[0]);
      n_padded_targets_ = 0;
      int64_t g_start = 0;
      while (g_start < state.n_targets) {
        long n0       = state.target_n(0, perm[g_start]);
        int64_t g_end = g_start;
        while (g_end < state.n_targets && state.target_n(0, perm[g_end]) == n0) ++g_end;
        int64_t count        = g_end - g_start;
        int64_t padded_count = (count + S - 1) / S * S;
        groups_.push_back({n0 - n_min_arr[0], n_padded_targets_, count, padded_count});
        n_padded_targets_ += padded_count;
        g_start = g_end;
      }

      // Allocate arrays with padded sizes
      scatter_map_.resize(n_padded_targets_);
      factored_idx_.resize((Rank - 1) * n_padded_targets_);
      grouped_fk_.resize(n_padded_targets_);

      // Pass 2: fill gather indices and scatter map, with padding
      int64_t perm_pos = 0;
      for (auto const &g : groups_) {
        for (int64_t k = 0; k < g.count; ++k) {
          scatter_map_[g.start + k] = perm[perm_pos + k];
          for (int r = 1; r < Rank; ++r)
            factored_idx_[(r - 1) * n_padded_targets_ + g.start + k] = 2 * (state.target_n(r, perm[perm_pos + k]) - n_min_arr[r]);
        }
        // Pad with last real entry's index (result discarded by scatter)
        for (int64_t k = g.count; k < g.padded_count; ++k)
          for (int r = 1; r < Rank; ++r)
            factored_idx_[(r - 1) * n_padded_targets_ + g.start + k] = factored_idx_[(r - 1) * n_padded_targets_ + g.start + g.count - 1];
        perm_pos += g.count;
      }
    }

    // Detect if targets form a dense sequential grid (n_targets == product of n_range_arr)
    void detect_sequential(shared_state_t<Rank> const &state) {
      int64_t product = 1;
      for (int r = 0; r < Rank; ++r) product *= n_range_arr[r];
      if (product != state.n_targets) return;

      // Verify row-major ordering: target d maps to multi-index (k0, k1, ...) where
      // target_n(r, d) = n_min_arr[r] + k_r, with k_r cycling fastest for last rank
      for (int64_t d = 0; d < state.n_targets; ++d) {
        int64_t rem = d;
        for (int r = Rank - 1; r >= 0; --r) {
          long expected_n = n_min_arr[r] + static_cast<long>(rem % n_range_arr[r]);
          if (state.target_n(r, d) != expected_n) return;
          rem /= n_range_arr[r];
        }
      }
      is_sequential_ = true;
    }

    // Compute z_step^n for |z_step|=1 using binary exponentiation + conjugate
    static dcomplex unit_pow(dcomplex z_step, long n) {
      if (n == 0) return {1.0, 0.0};
      bool neg            = (n < 0);
      unsigned long abs_n = static_cast<unsigned long>(neg ? -n : n);
      dcomplex result     = z_step;
      unsigned long p     = abs_n;
      // Find highest bit
      int bits = std::bit_width(p) - 1;
      for (int b = bits - 1; b >= 0; --b) {
        result *= result; // square
        if ((p >> b) & 1) result *= z_step;
      }
      return neg ? std::conj(result) : result;
    }

    // Build SIMD multiplier and stride from a unit-step phasor z_step:
    // mult[k] = z_step^k for k in [0, S), stride = z_step^S.
    static std::pair<cbatch, cbatch> make_simd_multiplier(dcomplex z_step) {
      constexpr std::size_t S = cbatch::size;
      alignas(cbatch::arch_type::alignment()) std::array<dcomplex, S> mult_arr;
      dcomplex z_pow = 1.0;
      for (std::size_t k = 0; k < S; ++k) { mult_arr[k] = z_pow; z_pow *= z_step; }
      return {cbatch::load_aligned(mult_arr.data()), cbatch(z_pow)};
    }

    // Fill destination with geometric sequence exp(i*(2n+1)*pi*tau/beta) for n in [n_min, n_min+n_range).
    // Uses n_range_padded (SIMD-aligned) so no scalar tail is needed.
    template <int TolDigits> void fill_pow_row(dcomplex * __restrict__ dest, int r, double tau, double pi_over_beta) {
      constexpr std::size_t S = cbatch::size;
      long const nr      = n_range_padded[r];
      double const theta1 = pi_over_beta * tau;
      dcomplex const z1 = cis<TolDigits>(theta1);
      dcomplex const z_step = z1 * z1;
      dcomplex const z_base = unit_pow(z_step, n_min_arr[r]) * z1;

      auto [mult, stride] = make_simd_multiplier(z_step);
      cbatch zp_vec(z_base);

      for (long i = 0; i < nr; i += S) {
        (zp_vec * mult).store_unaligned(dest + i);
        zp_vec *= stride;
      }
    }

    // ---- Sequential path: generic recursive template for all ranks ----

    template <int R = 0>
    void seq_accumulate(dcomplex *out, dcomplex coeff,
                        std::array<dcomplex, Rank> const &z_base,
                        std::array<dcomplex, Rank> const &z_step,
                        std::array<std::pair<cbatch, cbatch>, Rank> const &simd_mult) {
      constexpr std::size_t S = cbatch::size;
      if constexpr (R == Rank - 1) {
        // Innermost dimension: SIMD geometric progression
        long const nr      = n_range_arr[R];
        long const nr_simd = nr - (nr % static_cast<long>(S));
        auto const &[mult, stride] = simd_mult[R];
        cbatch zp(coeff * z_base[R]);
        for (long k = 0; k < nr_simd; k += S) {
          xsimd::fma(zp, mult, cbatch::load_unaligned(out + k)).store_unaligned(out + k);
          zp *= stride;
        }
        dcomplex z_scalar = zp.get(0);
        for (long k = nr_simd; k < nr; ++k) { out[k] += z_scalar; z_scalar *= z_step[R]; }
      } else {
        // Outer dimension: scalar loop, recurse into next dimension
        dcomplex z = coeff * z_base[R];
        for (long k = 0; k < n_range_arr[R]; ++k) {
          seq_accumulate<R + 1>(out + k * seq_strides[R], z, z_base, z_step, simd_mult);
          z *= z_step[R];
        }
      }
    }

    template <int TolDigits> void execute_sequential(shared_state_t<Rank> &state) {
      dcomplex * __restrict__ fiw_ptr = state.fk_vec.data();
      double const pi_over_beta = M_PI / state.beta;
      double const step_freq    = 2.0 * pi_over_beta;

      for (int j = 0; j < state.buf_counter; ++j) {
        std::array<dcomplex, Rank> z_base, z_step;
        std::array<std::pair<cbatch, cbatch>, Rank> simd_mult;
        poet::static_for<Rank>([&](auto r) {
          double const tau = state.x_arr(r, j);
          // z_base already includes the smallest odd exponent 2 n_min + 1.
          z_base[r]    = cis<TolDigits>(static_cast<double>(2 * n_min_arr[r] + 1) * pi_over_beta * tau);
          // Multiplying by z_step advances n_r by one, i.e. by two in the odd exponent.
          z_step[r]    = cis<TolDigits>(step_freq * tau);
          simd_mult[r] = make_simd_multiplier(z_step[r]);
        });
        seq_accumulate<0>(fiw_ptr, state.fx_arr[j], z_base, z_step, simd_mult);
      }
    }

    // ---- Gather path: Rank=1 ----

    template <int TolDigits> void execute_gather(shared_state_t<Rank> &state) requires(Rank == 1) {
      using rbatch              = xsimd::batch<double>;
      using ibatch              = xsimd::batch<int64_t>;
      constexpr std::size_t S   = cbatch::size;
      constexpr int B           = 4; // batch size for j-tiling
      double const pi_over_beta = M_PI / state.beta;

      int64_t const nt_simd = state.n_targets - (state.n_targets % static_cast<int64_t>(S));
      dcomplex * __restrict__ fiw_ptr = state.fk_vec.data();
      long const * __restrict__ idx0 = target_idx.data();

      std::array<nda::vector<dcomplex>, B> pow0_buf;
      for (int b = 0; b < B; ++b) pow0_buf[b].resize(n_range_padded[0]);

      int j = 0;
      for (; j + B <= state.buf_counter; j += B) {
        std::array<cbatch, B> fj_vec;
        for (int b = 0; b < B; ++b) {
          // Precompute the whole odd-power row once for each source in the tile.
          fill_pow_row<TolDigits>(pow0_buf[b].data(), 0, state.x_arr(0, j + b), pi_over_beta);
          fj_vec[b] = cbatch(state.fx_arr[j + b]);
        }

        for (int64_t d = 0; d < nt_simd; d += S) {
          auto idx = ibatch::load_unaligned(idx0 + d);
          cbatch acc = cbatch::load_unaligned(fiw_ptr + d);
          for (int b = 0; b < B; ++b) {
            double const * __restrict__ p0 = reinterpret_cast<double const *>(pow0_buf[b].data());
            acc = xsimd::fma(fj_vec[b], cbatch(rbatch::gather(p0, idx), rbatch::gather(p0, idx + 1)), acc);
          }
          acc.store_unaligned(fiw_ptr + d);
        }
        for (int64_t d = nt_simd; d < state.n_targets; ++d)
          for (int b = 0; b < B; ++b) fiw_ptr[d] += state.fx_arr[j + b] * pow0_buf[b].data()[idx0[d] >> 1];
      }

      // Remainder: one source at a time
      for (; j < state.buf_counter; ++j) {
        fill_pow_row<TolDigits>(pow0_buf[0].data(), 0, state.x_arr(0, j), pi_over_beta);
        cbatch const fj(state.fx_arr[j]);
        double const * __restrict__ pow0 = reinterpret_cast<double const *>(pow0_buf[0].data());

        for (int64_t d = 0; d < nt_simd; d += S) {
          auto idx = ibatch::load_unaligned(idx0 + d);
          cbatch pow_val(rbatch::gather(pow0, idx), rbatch::gather(pow0, idx + 1));
          xsimd::fma(fj, pow_val, cbatch::load_unaligned(fiw_ptr + d)).store_unaligned(fiw_ptr + d);
        }
        for (int64_t d = nt_simd; d < state.n_targets; ++d)
          fiw_ptr[d] += state.fx_arr[j] * pow0_buf[0].data()[idx0[d] >> 1];
      }
    }

    // ---- Gather path: Rank>=2, factored accumulation with j-tiling ----

    template <int TolDigits> void execute_gather(shared_state_t<Rank> &state) requires(Rank >= 2) {
      using rbatch              = xsimd::batch<double>;
      using ibatch              = xsimd::batch<int64_t>;
      constexpr std::size_t S   = cbatch::size;
      constexpr int B           = 4; // batch size for j-tiling
      double const pi_over_beta = M_PI / state.beta;

      grouped_fk_ = 0;
      dcomplex * __restrict__ gfk_ptr = grouped_fk_.data();

      // Allocate B pow buffers per rank
      std::array<std::array<nda::vector<dcomplex>, B>, Rank> pow_bufs;
      poet::static_for<Rank>([&](auto r) {
        for (int b = 0; b < B; ++b) pow_bufs[r][b].resize(n_range_padded[r]);
      });

      // Gather rank-1+ power products for one source batch element
      auto gather_pow_prod = [&](int b, int64_t g_start, int64_t k) {
        cbatch pow_prod;
        poet::static_for<1, Rank>([&](auto r) {
          double const * __restrict__ pr = reinterpret_cast<double const *>(pow_bufs[r][b].data());
          long const * __restrict__ idr  = factored_idx_.data() + (r - 1) * n_padded_targets_ + g_start;
          auto idx = ibatch::load_unaligned(idr + k);
          cbatch pv(rbatch::gather(pr, idx), rbatch::gather(pr, idx + 1));
          if constexpr (decltype(r)::value == 1) pow_prod = pv;
          else pow_prod *= pv;
        });
        return pow_prod;
      };

      int j = 0;
      for (; j + B <= state.buf_counter; j += B) {
        for (int b = 0; b < B; ++b)
          poet::static_for<Rank>([&](auto r) { fill_pow_row<TolDigits>(pow_bufs[r][b].data(), r, state.x_arr(r, j + b), pi_over_beta); });

        for (auto const &g : groups_) {
          // Inside one group, rank 0 is fixed and only the remaining ranks are gathered.
          std::array<cbatch, B> combined_vec;
          for (int b = 0; b < B; ++b) combined_vec[b] = cbatch(state.fx_arr[j + b] * pow_bufs[0][b].data()[g.pow0_offset]);

          dcomplex * __restrict__ out    = gfk_ptr + g.start;
          for (int64_t k = 0; k < g.padded_count; k += S) {
            cbatch acc = cbatch::load_unaligned(out + k);
            for (int b = 0; b < B; ++b) acc = xsimd::fma(combined_vec[b], gather_pow_prod(b, g.start, k), acc);
            acc.store_unaligned(out + k);
          }
        }
      }

      // Remainder: one source at a time
      for (; j < state.buf_counter; ++j) {
        poet::static_for<Rank>([&](auto r) { fill_pow_row<TolDigits>(pow_bufs[r][0].data(), r, state.x_arr(r, j), pi_over_beta); });
        dcomplex const fj = state.fx_arr[j];

        for (auto const &g : groups_) {
          // Rank 0 is scalar inside the group; ranks 1..R-1 are gathered SIMD products.
          cbatch const combined_vec(fj * pow_bufs[0][0].data()[g.pow0_offset]);
          dcomplex * __restrict__ out = gfk_ptr + g.start;
          for (int64_t k = 0; k < g.padded_count; k += S)
            xsimd::fma(combined_vec, gather_pow_prod(0, g.start, k), cbatch::load_unaligned(out + k)).store_unaligned(out + k);
        }
      }

      // Scatter only real (non-padded) entries to original target order
      dcomplex * __restrict__ fiw_ptr = state.fk_vec.data();
      for (auto const &g : groups_)
        for (int64_t k = 0; k < g.count; ++k) fiw_ptr[scatter_map_[g.start + k]] += gfk_ptr[g.start + k];
    }
  };

} // namespace triqs::utility::nfft
