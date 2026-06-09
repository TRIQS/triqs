// Copyright (c) 2017--present, The Simons Foundation
// This file is part of TRIQS and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#pragma once
#include "../common.hpp"
#include <limits>
#include <numeric>
#include <unordered_map>

namespace triqs::utility::nfft {

  // NAF writes e = |2n + 1| as a sparse signed binary sum
  //
  //   e = Σ_k ε_k 2^k,   ε_k in {-1, 0, 1},
  //
  // with no adjacent nonzero digits. Example: 13 = 16 - 4 + 1.
  // Then z^e is rebuilt from the shared powers z^(2^k); ε_k = -1 means use
  // conj(z^(2^k)). Negative Matsubara indices only add a final conjugation.
  template <int Rank> struct kernel_naf_t {

    kernel_naf_t() = default;

    kernel_naf_t(shared_state_t<Rank> const &state, int /*buf_size*/) {
      target_map.resize(state.n_targets * Rank);
      for (int r = 0; r < Rank; ++r) {
        int max_digit_row = 0;
        std::unordered_map<unsigned long, int> exp_to_idx;
        digit_offsets[r].push_back(0);
        for (int64_t d = 0; d < state.n_targets; ++d) {
          unsigned long exp   = odd_exponent_abs(state.target_n(r, d));
          auto [it, inserted] = exp_to_idx.try_emplace(exp, static_cast<int>(exp_to_idx.size()));
          if (inserted) {
            // Store the NAF digits once per unique exponent as (row, sign) pairs.
            auto digits = compute_naf(exp);
            digit_offsets[r].push_back(static_cast<int>(digit_row_flat[r].size() + digits.size()));
            for (int digit : digits) {
              int const row = digit >= 0 ? digit : -(digit + 1);
              max_digit_row = std::max(max_digit_row, row);
              digit_neg_flat[r].push_back(static_cast<uint8_t>(digit < 0));
              digit_row_flat[r].push_back(static_cast<uint16_t>(row));
            }
          }
          bool needs_conj          = (state.target_n(r, d) < 0);
          target_map[d * Rank + r] = 2 * it->second + (needs_conj ? 1 : 0);
        }
        n_unique[r]         = static_cast<int>(exp_to_idx.size());
        num_pow2_levels[r]  = std::max(1, max_digit_row + 1);
        simd_pow2_tbl[r].resize(num_pow2_levels[r]);
        scalar_pow2_tbl[r].resize(num_pow2_levels[r]);
        uq_simd_buf[r].resize(n_unique[r]);
        uq_scalar_buf[r].resize(n_unique[r]);
      }
      sums_buf.resize(state.n_targets);

      if constexpr (Rank == 2) {
        // Group equal rank-0 exponents so one lookup can be reused across the group.
        std::vector<int> order(state.n_targets);
        std::iota(order.begin(), order.end(), 0);
        std::sort(order.begin(), order.end(), [&](int lhs, int rhs) {
          int const lhs_key0 = target_map[2 * lhs];
          int const rhs_key0 = target_map[2 * rhs];
          if (lhs_key0 != rhs_key0) return lhs_key0 < rhs_key0;
          return target_map[2 * lhs + 1] < target_map[2 * rhs + 1];
        });

        rank0_group_key.reserve(order.size());
        rank0_group_offset.reserve(order.size() + 1);
        grouped_target_idx.reserve(order.size());
        grouped_rank1_idx.reserve(order.size());

        int current_key = std::numeric_limits<int>::min();
        for (int d : order) {
          int const key0 = target_map[2 * d];
          if (rank0_group_key.empty() || key0 != current_key) {
            rank0_group_key.push_back(key0);
            rank0_group_offset.push_back(static_cast<int>(grouped_target_idx.size()));
            current_key = key0;
          }
          grouped_target_idx.push_back(d);
          grouped_rank1_idx.push_back(target_map[2 * d + 1]);
        }
        rank0_group_offset.push_back(static_cast<int>(grouped_target_idx.size()));
      }
    }

    template <int TolDigits = 12> void execute(shared_state_t<Rank> &state) {
      double const pi_over_beta      = M_PI / state.beta;
      int64_t const buf_counter_simd = state.buf_counter & -simd_size;
      execute_phase2<TolDigits>(state, pi_over_beta, buf_counter_simd);
    }

    private:
    // Per-rank power-of-two tables z^(2^k), in SIMD and scalar form.
    std::array<int, Rank> num_pow2_levels{};
    std::array<std::vector<cbatch>, Rank> simd_pow2_tbl;
    std::array<std::vector<dcomplex>, Rank> scalar_pow2_tbl;

    // Per-rank unique-exponent metadata for the NAF expansion.
    std::array<int, Rank> n_unique{};
    std::array<std::vector<uint16_t>, Rank> digit_row_flat;
    std::array<std::vector<uint8_t>, Rank> digit_neg_flat;
    std::array<std::vector<int>, Rank> digit_offsets;

    // Target -> unique exponent lookup. For each rank:
    //   info = 2 * unique_id + needs_conj.
    std::vector<int> target_map;

    // Temporary per-flush work buffers.
    mutable std::array<std::vector<xsimd::batch<dcomplex>>, Rank> uq_simd_buf;
    mutable std::array<std::vector<dcomplex>, Rank> uq_scalar_buf;
    mutable std::vector<xsimd::batch<dcomplex>> sums_buf;

    // Rank-2 grouped accumulation order: group equal rank-0 exponents together.
    std::vector<int> rank0_group_key;
    std::vector<int> rank0_group_offset;
    std::vector<int> grouped_target_idx;
    std::vector<int> grouped_rank1_idx;
    template <int TolDigits> void accumulate_simd_step(shared_state_t<Rank> &state, int j_begin, double pi_over_beta) {
      poet::static_for<Rank>([&](const auto r) {
        auto &tbl = simd_pow2_tbl[r];
        using rbatch = xsimd::batch<double>;
        // Build z^(2^k) by repeated squaring, starting from z^1.
        auto [sin_vec, cos_vec] = triqs::utility::math::sincos<TolDigits>(rbatch::load_unaligned(&state.x_arr(r, j_begin)) * pi_over_beta);
        tbl[0] = cbatch(cos_vec, sin_vec);
        for (int k = 1; k < num_pow2_levels[r]; ++k) tbl[k] = tbl[k - 1] * tbl[k - 1];
      });

      int const *__restrict__ map_ptr      = target_map.data();
      constexpr int map_stride             = Rank;
      int64_t const n_tgt                  = state.n_targets;
      dcomplex const *__restrict__ fxp     = state.fx_arr.data() + j_begin;

      auto compute_unique_simd = [&](int r, int u) -> cbatch {
        uint16_t const *rows = digit_row_flat[r].data() + digit_offsets[r][u];
        uint8_t const *negs  = digit_neg_flat[r].data() + digit_offsets[r][u];
        int n_digits    = digit_offsets[r][u + 1] - digit_offsets[r][u];
        auto const *base = simd_pow2_tbl[r].data();

        cbatch rank_pow = base[rows[0]];
        if (negs[0]) rank_pow = xsimd::conj(rank_pow);

        for (int i = 1; i < n_digits; ++i) {
          cbatch val = base[rows[i]];
          rank_pow *= negs[i] ? xsimd::conj(val) : val;
        }
        return rank_pow;
      };

      auto fill_unique = [&](cbatch *__restrict__ uq, auto r) {
        for (int u = 0; u < n_unique[r]; ++u) {
          // Materialize the requested exponent z^{|2n+1|} from the shared power-of-two table.
          uq[u] = compute_unique_simd(r, u);
        }
      };

      cbatch *__restrict__ uq0 = uq_simd_buf[0].data();
      fill_unique(uq0, std::integral_constant<int, 0>{});

      cbatch fj = cbatch::load_unaligned(fxp);
      auto load_target = [](cbatch const *__restrict__ uq, int info) {
        cbatch pow = uq[info >> 1];
        return (info & 1) ? xsimd::conj(pow) : pow;
      };
      if constexpr (Rank == 1) {
        cbatch *__restrict__ sp = sums_buf.data();
        poet::dynamic_for<ilp_unroll, 1>(int64_t{0}, n_tgt, [&](int64_t d) {
          // Rank 1 is just a gather from the unique-exponent table.
          sp[d] = xsimd::fma(fj, load_target(uq0, map_ptr[d]), sp[d]);
        });
      } else {
        cbatch *__restrict__ uq1 = uq_simd_buf[1].data();
        fill_unique(uq1, std::integral_constant<int, 1>{});
        if constexpr (Rank > 2) {
          for (int r = 2; r < Rank; ++r) fill_unique(uq_simd_buf[r].data(), r);
        }

        if constexpr (Rank == 2) {
          int const *__restrict__ group_key    = rank0_group_key.data();
          int const *__restrict__ group_offset = rank0_group_offset.data();
          int const *__restrict__ rank1_idx    = grouped_rank1_idx.data();
          cbatch *__restrict__ grouped_sum     = sums_buf.data();
          int const n_groups                   = static_cast<int>(rank0_group_key.size());

          for (int g = 0; g < n_groups; ++g) {
            // Group by the rank-0 exponent so one lookup can be reused over many targets.
            cbatch const fj_u0 = fj * load_target(uq0, group_key[g]);
            for (int p = group_offset[g]; p < group_offset[g + 1]; ++p)
              grouped_sum[p] = xsimd::fma(fj_u0, load_target(uq1, rank1_idx[p]), grouped_sum[p]);
          }
        } else {
          cbatch *__restrict__ sp = sums_buf.data();
          for (int64_t d = 0; d < n_tgt; ++d) {
            int const *info = map_ptr + d * map_stride;
            cbatch pow      = load_target(uq0, info[0]) * load_target(uq1, info[1]);
            for (int r = 2; r < Rank; ++r) pow *= load_target(uq_simd_buf[r].data(), info[r]);
            sp[d] = xsimd::fma(fj, pow, sp[d]);
          }
        }
      }
    }

    template <int TolDigits> void accumulate_scalar_tail(shared_state_t<Rank> &state, int j_begin, double pi_over_beta) {
      int const *__restrict__ map_ptr  = target_map.data();
      constexpr int map_stride         = Rank;
      dcomplex *__restrict__ fiw_ptr   = state.fk_vec.data();

      auto compute_unique_scalar = [&](int r, int u) -> dcomplex {
        uint16_t const *rows = digit_row_flat[r].data() + digit_offsets[r][u];
        uint8_t const *negs  = digit_neg_flat[r].data() + digit_offsets[r][u];
        int n_digits         = digit_offsets[r][u + 1] - digit_offsets[r][u];
        auto const &base = scalar_pow2_tbl[r];

        dcomplex rank_pow = base[rows[0]];
        if (negs[0]) rank_pow = std::conj(rank_pow);

        for (int i = 1; i < n_digits; ++i) {
          dcomplex val = base[rows[i]];
          rank_pow *= negs[i] ? std::conj(val) : val;
        }
        return rank_pow;
      };

      for (int j = j_begin; j < state.buf_counter; ++j) {
        poet::static_for<Rank>([&](const auto r) {
          auto &tbl       = scalar_pow2_tbl[r];
          double const theta = pi_over_beta * state.x_arr(r, j);
          // Scalar tail mirrors the SIMD phase table construction above.
          tbl[0]             = cis<TolDigits>(theta);
          for (int k = 1; k < num_pow2_levels[r]; ++k) tbl[k] = tbl[k - 1] * tbl[k - 1];

          for (int u = 0; u < n_unique[r]; ++u) {
            uq_scalar_buf[r][u] = compute_unique_scalar(r, u);
          }
        });
        dcomplex fj = state.fx_arr[j];
        for (int64_t d = 0; d < state.n_targets; ++d) {
          int const *info = map_ptr + d * map_stride;
          dcomplex pow    = uq_scalar_buf[0][info[0] >> 1];
          if (info[0] & 1) pow = std::conj(pow);
          for (int r = 1; r < Rank; ++r) {
            dcomplex val = uq_scalar_buf[r][info[r] >> 1];
            pow *= (info[r] & 1) ? std::conj(val) : val;
          }
          fiw_ptr[d] += fj * pow;
        }
      }
    }

    template <int TolDigits> [[gnu::noinline]] void execute_phase2(shared_state_t<Rank> &state, double pi_over_beta, int64_t buf_counter_simd) {
      // SIMD sources first, then reduce lanes, then finish the scalar tail.
      std::fill(sums_buf.begin(), sums_buf.end(), cbatch(dcomplex{0, 0}));
      for (int j = 0; j < buf_counter_simd; j += simd_size) accumulate_simd_step<TolDigits>(state, j, pi_over_beta);

      dcomplex *fiw_ptr = state.fk_vec.data();
      if constexpr (Rank == 2) {
        if (!grouped_target_idx.empty()) {
          // SIMD sums are stored in grouped order; scatter them back to the original targets.
          for (std::size_t p = 0; p < grouped_target_idx.size(); ++p) fiw_ptr[grouped_target_idx[p]] += xsimd::reduce_add(sums_buf[p]);
        } else {
          for (int64_t d = 0; d < state.n_targets; ++d) fiw_ptr[d] += xsimd::reduce_add(sums_buf[d]);
        }
      } else {
        for (int64_t d = 0; d < state.n_targets; ++d) fiw_ptr[d] += xsimd::reduce_add(sums_buf[d]);
      }
      accumulate_scalar_tail<TolDigits>(state, static_cast<int>(buf_counter_simd), pi_over_beta);
    }
  };

} // namespace triqs::utility::nfft
