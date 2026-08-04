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

  // Chain kernel for odd Matsubara exponents e = |2n + 1|.
  // For each rank, keep only the unique exponents and build them from z^1 by
  // repeatedly combining rows that are already available. If e = a + b or
  // e = a - b, then
  //
  //   z^e = z^a z^b,     z^{a-b} = z^a conj(z^b).
  //
  // So the planner searches for a short addition/subtraction chain for the
  // required exponents. Negative Matsubara indices only add a final conjugation.
  template <int Rank> struct kernel_chain_t {

    kernel_chain_t() = default;

    kernel_chain_t(shared_state_t<Rank> const &state) {
      // Build a shared addition/subtraction plan over the unique exponents per
      // rank and reuse those rows across all targets.
      target_map.resize(state.n_targets * Rank);
      std::array<std::vector<int>, Rank> row_indices_by_rank;
      int total_unique_entries = 0;
      int total_uq_entries     = 0;
      for (int r = 0; r < Rank; ++r) {
        std::unordered_map<unsigned long, int> exp_to_unique;
        for (int64_t d = 0; d < state.n_targets; ++d) {
          unsigned long exp   = odd_exponent_abs(state.target_n(r, d));
          auto it             = exp_to_unique.try_emplace(exp, static_cast<int>(exp_to_unique.size())).first;
          bool needs_conj          = (state.target_n(r, d) < 0);
          target_map[d * Rank + r] = 2 * it->second + (needs_conj ? 1 : 0);
        }
        n_unique[r] = static_cast<int>(exp_to_unique.size());
        unique_entry_offsets[r] = total_unique_entries;
        uq_entry_offsets[r]     = total_uq_entries;
        total_unique_entries += n_unique[r];
        total_uq_entries += n_unique[r];

        std::vector<unsigned long> unique_exponents(n_unique[r]);
        for (auto const &[exp, u] : exp_to_unique) unique_exponents[u] = exp;
        // Plan how to synthesize every required odd exponent from a short chain.
        plans_[r] = build_rank_plan(unique_exponents);

        row_indices_by_rank[r].resize(n_unique[r]);
        for (auto const &[exp, u] : exp_to_unique)
          row_indices_by_rank[r][u] = plans_[r].required_row_idx[u];
        simd_row_tbl_[r].resize(plans_[r].row_exp.size());
        if constexpr (Rank == 1) {
          pow_tbl[r].resize(static_cast<long>(plans_[r].row_exp.size()), max_simd_source_block);
          table_bytes_per_source_ += static_cast<int64_t>(plans_[r].row_exp.size()) * static_cast<int64_t>(sizeof(dcomplex));
        }
        scalar_pow_tbl[r].resize(plans_[r].row_exp.size());
      }
      unique_row_idx.resize(total_unique_entries);
      for (int r = 0; r < Rank; ++r)
        std::copy(row_indices_by_rank[r].begin(), row_indices_by_rank[r].end(), unique_row_idx.begin() + unique_entry_offsets[r]);
      if constexpr (Rank != 1) {
        uq_scalar_buf.resize(total_uq_entries);
        sums_buf.resize(state.n_targets);
      }

      if constexpr (Rank == 1) {
        rank1_target_row_offset.resize(state.n_targets);
        rank1_target_needs_conj.resize(state.n_targets);
        for (int64_t d = 0; d < state.n_targets; ++d) {
          int const target_info          = target_map[d];
          int const unique_idx           = target_info >> 1;
          rank1_target_row_offset[d]     = static_cast<int64_t>(row_indices_by_rank[0][unique_idx]) * max_simd_source_block;
          rank1_target_needs_conj[d]     = static_cast<uint8_t>(target_info & 1);
        }
      }

      if constexpr (Rank == 2) {
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
    struct op_t {
      int lhs;
      int rhs;
      bool subtract_rhs;
    };

    struct rank_plan_t {
      // row_exp[0] = 1. Each later row is synthesized from two earlier rows:
      //   row_exp[k+1] = row_exp[lhs] +/- row_exp[rhs].
      std::vector<unsigned long> row_exp;
      std::vector<op_t> ops;
      std::vector<int> required_row_idx;
    };

    static constexpr int64_t plan_score_weight            = 1024;
    static constexpr int max_simd_source_block            = 256;
    static constexpr int64_t simd_block_cache_budget_bytes = 1024 * 1024;
    static constexpr int rank1_target_unroll              = ilp_unroll;

    // Per-rank addition/subtraction-chain plans.
    std::array<rank_plan_t, Rank> plans_;

    // Row tables used while evaluating a flush.
    // Rank 1 keeps a larger block table in `pow_tbl`; higher ranks use row-by-row SIMD tables.
    std::array<nda::array<dcomplex, 2>, Rank> pow_tbl;
    std::array<std::vector<dcomplex>, Rank> scalar_pow_tbl;
    std::array<std::vector<cbatch>, Rank> simd_row_tbl_;

    // Per-rank unique-exponent metadata.
    std::array<int, Rank> n_unique{};
    std::array<int, Rank> unique_entry_offsets{};
    std::array<int, Rank> uq_entry_offsets{};
    std::vector<int> unique_row_idx;

    // Target -> unique exponent lookup, encoded as 2 * unique_id + needs_conj.
    std::vector<int> target_map;

    // Temporary per-flush buffers.
    mutable std::vector<dcomplex> uq_scalar_buf;
    mutable std::vector<xsimd::batch<dcomplex>> sums_buf;

    // Rank-2 grouped accumulation metadata.
    std::vector<int> rank0_group_key;
    std::vector<int> rank0_group_offset;
    std::vector<int> grouped_target_idx;
    std::vector<int> grouped_rank1_idx;

    // Rank-1 target lookup is flattened so each target can jump straight to its synthesized row.
    std::vector<int64_t> rank1_target_row_offset;
    std::vector<uint8_t> rank1_target_needs_conj;

    // Used to choose the size of the synthesized source block for Rank 1.
    int64_t table_bytes_per_source_ = 0;

    // Greedy planner: add reachable required exponents first, otherwise add the helper
    // that unlocks the most missing exponents.
    static rank_plan_t build_rank_plan(std::vector<unsigned long> const &required_exponents) {
      rank_plan_t plan;
      if (required_exponents.empty()) return plan;

      std::vector<unsigned long> required = required_exponents;
      std::sort(required.begin(), required.end());
      required.erase(std::unique(required.begin(), required.end()), required.end());

      unsigned long const max_required = required.back();
      unsigned long const helper_limit = std::max<unsigned long>(max_required, std::bit_ceil(max_required));

      plan.row_exp.push_back(1);
      std::unordered_map<unsigned long, int> exp_to_row{{1UL, 0}};
      plan.required_row_idx.resize(required_exponents.size(), -1);

      auto find_current_step = [&](unsigned long target) -> std::optional<op_t> {
        std::optional<op_t> best;
        unsigned long best_span = std::numeric_limits<unsigned long>::max();
        for (std::size_t i = 0; i < plan.row_exp.size(); ++i) {
          unsigned long const a = plan.row_exp[i];
          if (a < target) {
            if (auto it = exp_to_row.find(target - a); it != exp_to_row.end()) {
              unsigned long span = std::max(a, target - a);
              if (span < best_span) {
                best_span = span;
                best      = op_t{static_cast<int>(i), it->second, false};
              }
            }
          }
          if (auto it = exp_to_row.find(target + a); it != exp_to_row.end()) {
            unsigned long span = target + a;
            if (span < best_span) {
              best_span = span;
              best      = op_t{it->second, static_cast<int>(i), true};
            }
          }
        }
        return best;
      };

      auto reachable_with_candidate = [&](unsigned long target, unsigned long candidate) {
        if (target == candidate) return true;
        auto contains = [&](unsigned long exp) { return exp == candidate || exp_to_row.contains(exp); };

        for (unsigned long a : plan.row_exp) {
          if (a < target && contains(target - a)) return true;
          if (contains(target + a)) return true;
        }

        if (candidate < target && contains(target - candidate)) return true;
        return contains(target + candidate);
      };

      auto add_row = [&](unsigned long exp, op_t op) {
        exp_to_row.emplace(exp, static_cast<int>(plan.row_exp.size()));
        plan.row_exp.push_back(exp);
        plan.ops.push_back(op);
      };

      // Extend the plan to exponent x with a binary (double-and-add) ladder:
      // ensure floor(x/2), add 2*floor(x/2) = half + half, and, for odd x, add
      // x = 2*floor(x/2) + 1 using row 1 (always present). Costs at most
      // 2*log2(x) rows per isolated exponent; used when no candidate row unlocks
      // any missing required exponent, where pure greedy scoring would creep
      // one exponent per step.
      auto ensure_binary = [&](auto &&self, unsigned long x) -> void {
        if (exp_to_row.contains(x)) return;
        unsigned long const half = x / 2;
        self(self, half);
        int const half_row = exp_to_row.at(half);
        if (!exp_to_row.contains(2 * half)) add_row(2 * half, op_t{half_row, half_row, false});
        if (x % 2 == 1 && !exp_to_row.contains(x))
          add_row(x, op_t{exp_to_row.at(2 * half), exp_to_row.at(1), false});
      };

      while (true) {
        bool all_done = true;
        bool progress = false;

        for (std::size_t u = 0; u < required.size(); ++u) {
          unsigned long const exp = required[u];
          if (exp_to_row.contains(exp)) continue;
          all_done = false;
          if (auto op = find_current_step(exp)) {
            // Best case: the next required exponent is already reachable from existing rows.
            add_row(exp, *op);
            progress                 = true;
          }
        }

        if (all_done) break;
        if (progress) continue;

        unsigned long best_candidate = 0;
        std::optional<op_t> best_op;
        int64_t best_score = std::numeric_limits<int64_t>::min();
        std::unordered_map<unsigned long, op_t> candidates;

        for (std::size_t i = 0; i < plan.row_exp.size(); ++i) {
          for (std::size_t j = 0; j <= i; ++j) {
            unsigned long const ai = plan.row_exp[i];
            unsigned long const aj = plan.row_exp[j];

            unsigned long const sum = ai + aj;
            if (sum <= helper_limit && !exp_to_row.contains(sum)) candidates.try_emplace(sum, op_t{static_cast<int>(i), static_cast<int>(j), false});

            if (ai > aj) {
              unsigned long const diff = ai - aj;
              if (diff > 0 && diff <= helper_limit && !exp_to_row.contains(diff))
                candidates.try_emplace(diff, op_t{static_cast<int>(i), static_cast<int>(j), true});
            }
          }
        }

        bool any_unlock = false;
        for (auto const &[candidate, op] : candidates) {
          int64_t score = 0;
          // Prefer helpers that unlock many missing required exponents cheaply.
          if (std::binary_search(required.begin(), required.end(), candidate)) {
            score += 1'000'000;
            any_unlock = true;
          }
          for (unsigned long exp : required)
            if (!exp_to_row.contains(exp) && reachable_with_candidate(exp, candidate)) {
              score += plan_score_weight;
              any_unlock = true;
            }
          score -= static_cast<int64_t>(candidate);

          if (score > best_score) {
            best_score     = score;
            best_candidate = candidate;
            best_op        = op;
          }
        }

        if (!any_unlock) {
          // No candidate unlocks progress (isolated exponents): extend toward the
          // smallest missing required exponent with a guaranteed binary chain.
          auto const it = std::find_if(required.begin(), required.end(), [&](unsigned long exp) { return !exp_to_row.contains(exp); });
          if (it == required.end()) NDA_RUNTIME_ERROR << "kernel_chain_t: unable to synthesize exponent plan\n";
          ensure_binary(ensure_binary, *it);
          continue;
        }

        if (!best_op) NDA_RUNTIME_ERROR << "kernel_chain_t: unable to synthesize exponent plan\n";
        add_row(best_candidate, *best_op);
      }

      for (std::size_t u = 0; u < required_exponents.size(); ++u) plan.required_row_idx[u] = exp_to_row.at(required_exponents[u]);

      return plan;
    }

    // Keep the temporary Rank-1 table small enough to stay cache-friendly.
    [[nodiscard]] int simd_source_block() const {
      if (table_bytes_per_source_ <= 0) return max_simd_source_block;
      int64_t const ideal = simd_block_cache_budget_bytes / table_bytes_per_source_;
      int64_t const rounded = std::max<int64_t>(simd_size, (ideal / static_cast<int64_t>(simd_size)) * static_cast<int64_t>(simd_size));
      return static_cast<int>(std::clamp<int64_t>(rounded, simd_size, max_simd_source_block));
    }

    template <int TolDigits> void synthesize_simd_block(shared_state_t<Rank> &state, int j_begin, int block_len, double pi_over_beta) {
      constexpr int stride = max_simd_source_block;
      poet::static_for<Rank>([&](const auto r) {
        dcomplex *tbl    = pow_tbl[r].data();
        auto const &plan = plans_[r];

        for (int j = 0; j < block_len; j += simd_size) {
          using rbatch            = xsimd::batch<double>;
          // Row 0 stores z^{1}; the rest of the rows are synthesized from the chain ops.
          // Seed at full precision: the chain multiplies amplify the seed error by
          // the exponent magnitude, so the low-digit buckets would poison the table.
          auto [sin_vec, cos_vec] = triqs::utility::math::sincos<12>(rbatch::load_unaligned(&state.x_arr(r, j_begin + j)) * pi_over_beta);
          cbatch(cos_vec, sin_vec).store_unaligned(tbl + j);
        }

        for (std::size_t op_idx = 0; op_idx < plan.ops.size(); ++op_idx) {
          auto const &op        = plan.ops[op_idx];
          int const out_row     = static_cast<int>(op_idx) + 1;
          dcomplex const *lhs   = tbl + static_cast<int64_t>(op.lhs) * stride;
          dcomplex const *rhs   = tbl + static_cast<int64_t>(op.rhs) * stride;
          dcomplex *out_row_ptr = tbl + static_cast<int64_t>(out_row) * stride;

          for (int j = 0; j < block_len; j += simd_size) {
            cbatch lhs_vec = cbatch::load_unaligned(lhs + j);
            cbatch rhs_vec = cbatch::load_unaligned(rhs + j);
            // a - b means multiply by conj(rhs): z^{a-b} = z^a conj(z^b).
            (op.subtract_rhs ? lhs_vec * xsimd::conj(rhs_vec) : lhs_vec * rhs_vec).store_unaligned(out_row_ptr + j);
          }
        }
      });
    }

    template <int TolDigits> void accumulate_simd_step(shared_state_t<Rank> &state, int j_begin, double pi_over_beta) {
      // For Rank > 1, build one SIMD packet of chain rows and accumulate it immediately.
      std::array<cbatch const *, Rank> row_base;
      std::array<int const *, Rank> row_idx_base;
      auto const *flat_row_idx = unique_row_idx.empty() ? nullptr : unique_row_idx.data();
      poet::static_for<Rank>([&](const auto r) {
        auto *rows        = simd_row_tbl_[r].data();
        auto [sin_vec, cos_vec] = triqs::utility::math::sincos<12>(xsimd::batch<double>::load_unaligned(&state.x_arr(r, j_begin)) * pi_over_beta);
        rows[0]           = cbatch(cos_vec, sin_vec);

        auto const &plan = plans_[r];
        for (std::size_t op_idx = 0; op_idx < plan.ops.size(); ++op_idx) {
          auto const &op              = plan.ops[op_idx];
          cbatch rhs                  = rows[op.rhs];
          rows[static_cast<int>(op_idx) + 1] = op.subtract_rhs ? rows[op.lhs] * xsimd::conj(rhs) : rows[op.lhs] * rhs;
        }

        row_base[r]     = rows;
        row_idx_base[r] = flat_row_idx ? flat_row_idx + unique_entry_offsets[r] : nullptr;
      });

      int const *__restrict__ mp       = target_map.data();
      int64_t const n_tgt              = state.n_targets;
      dcomplex const *__restrict__ fxp = state.fx_arr.data() + j_begin;
      auto load_target = [&](cbatch const *rows, int const *row_idx, int info) {
        cbatch pow = rows[row_idx[info >> 1]];
        return (info & 1) ? xsimd::conj(pow) : pow;
      };

      cbatch fj = cbatch::load_unaligned(fxp);
      if constexpr (Rank == 2) {
        int const *__restrict__ group_key    = rank0_group_key.data();
        int const *__restrict__ group_offset = rank0_group_offset.data();
        int const *__restrict__ rank1_idx    = grouped_rank1_idx.data();
        cbatch *__restrict__ grouped_sum     = sums_buf.data();
        int const n_groups                   = static_cast<int>(rank0_group_key.size());

        for (int g = 0; g < n_groups; ++g) {
          // Reuse the rank-0 row over every target that shares the same rank-0 exponent.
          cbatch const fj_u0 = fj * load_target(row_base[0], row_idx_base[0], group_key[g]);
          for (int p = group_offset[g]; p < group_offset[g + 1]; ++p)
            grouped_sum[p] = xsimd::fma(fj_u0, load_target(row_base[1], row_idx_base[1], rank1_idx[p]), grouped_sum[p]);
        }
      } else {
        cbatch *__restrict__ sp = sums_buf.data();
        for (int64_t d = 0; d < n_tgt; ++d) {
          int const *info = mp + d * Rank;
          cbatch pow      = load_target(row_base[0], row_idx_base[0], info[0]);
          if constexpr (Rank >= 2) pow *= load_target(row_base[1], row_idx_base[1], info[1]);
          if constexpr (Rank > 2) {
            for (int r = 2; r < Rank; ++r) pow *= load_target(row_base[r], row_idx_base[r], info[r]);
          }
          sp[d] = xsimd::fma(fj, pow, sp[d]);
        }
      }
    }

    template <int TolDigits> void execute_phase2_rank1(shared_state_t<Rank> &state, double pi_over_beta, int64_t buf_counter_simd) {
      static_assert(Rank == 1);

      dcomplex *__restrict__ fiw_ptr             = state.fk_vec.data();
      dcomplex const *__restrict__ row_base      = pow_tbl[0].data();
      int64_t const *__restrict__ row_offset_ptr = rank1_target_row_offset.data();
      uint8_t const *__restrict__ conj_ptr       = rank1_target_needs_conj.data();
      int64_t const n_targets                    = state.n_targets;
      int const source_block                     = simd_source_block();

      for (int jb = 0; jb < buf_counter_simd; jb += source_block) {
        int const block_len = std::min(source_block, static_cast<int>(buf_counter_simd - jb));
        // Rank 1 benefits most from synthesizing a larger source block once, then reusing it.
        synthesize_simd_block<TolDigits>(state, jb, block_len, pi_over_beta);

        auto compute_simd_pow                 = [&](int64_t d, int j) {
          cbatch pow = cbatch::load_unaligned(row_base + row_offset_ptr[d] + j);
          return conj_ptr[d] ? xsimd::conj(pow) : pow;
        };

        accumulate_targets_ilp<rank1_target_unroll>(n_targets, block_len, state.fx_arr.data() + jb, fiw_ptr, compute_simd_pow);
      }

      auto &tbl        = scalar_pow_tbl[0];
      auto const &plan = plans_[0];
      for (int j = static_cast<int>(buf_counter_simd); j < state.buf_counter; ++j) {
        double const theta = pi_over_beta * state.x_arr(0, j);
        tbl[0]             = cis<12>(theta);
        for (std::size_t op_idx = 0; op_idx < plan.ops.size(); ++op_idx) {
          auto const &op                   = plan.ops[op_idx];
          dcomplex rhs                     = tbl[op.rhs];
          tbl[static_cast<int>(op_idx) + 1] = op.subtract_rhs ? tbl[op.lhs] * std::conj(rhs) : tbl[op.lhs] * rhs;
        }

        dcomplex const fj = state.fx_arr[j];
        poet::dynamic_for<rank1_target_unroll, 1>(int64_t{0}, n_targets, [&](int64_t d) {
          // Each target picks the synthesized row it needs and optionally conjugates it.
          dcomplex pow = tbl[row_offset_ptr[d] / max_simd_source_block];
          fiw_ptr[d] += fj * (conj_ptr[d] ? std::conj(pow) : pow);
        });
      }
    }

    template <int TolDigits> void accumulate_scalar_tail(shared_state_t<Rank> &state, int j_begin, double pi_over_beta) {
      std::array<int const *, Rank> row_idx_base;
      std::array<dcomplex *, Rank> uq_scalar_base;
      auto const *flat_row_idx = unique_row_idx.empty() ? nullptr : unique_row_idx.data();
      auto *flat_uq_scalar     = uq_scalar_buf.empty() ? nullptr : uq_scalar_buf.data();
      poet::static_for<Rank>([&](const auto r) {
        row_idx_base[r]   = flat_row_idx ? flat_row_idx + unique_entry_offsets[r] : nullptr;
        uq_scalar_base[r] = flat_uq_scalar ? flat_uq_scalar + uq_entry_offsets[r] : nullptr;
      });

      for (int j = j_begin; j < state.buf_counter; ++j) {
        poet::static_for<Rank>([&](const auto r) {
          auto &tbl       = scalar_pow_tbl[r];
          auto const &plan = plans_[r];
          auto const *rows = row_idx_base[r];
          dcomplex *uq     = uq_scalar_base[r];

          double const theta = pi_over_beta * state.x_arr(r, j);
          tbl[0]             = cis<12>(theta);
          for (std::size_t op_idx = 0; op_idx < plan.ops.size(); ++op_idx) {
            auto const &op = plan.ops[op_idx];
            dcomplex rhs   = tbl[op.rhs];
            tbl[static_cast<int>(op_idx) + 1] = op.subtract_rhs ? tbl[op.lhs] * std::conj(rhs) : tbl[op.lhs] * rhs;
          }
          // Materialize the rows that are actually referenced by the targets.
          for (int u = 0; u < n_unique[r]; ++u) {
            uq[u] = tbl[rows[u]];
          }
        });

        dcomplex fj = state.fx_arr[j];
        for (int64_t d = 0; d < state.n_targets; ++d) {
          int const *info = target_map.data() + d * Rank;
          dcomplex pow    = uq_scalar_base[0][info[0] >> 1];
          if (info[0] & 1) pow = std::conj(pow);
          for (int r = 1; r < Rank; ++r) {
            dcomplex val = uq_scalar_base[r][info[r] >> 1];
            pow *= (info[r] & 1) ? std::conj(val) : val;
          }
          state.fk_vec[d] += fj * pow;
        }
      }
    }

    template <int TolDigits> void execute_phase2(shared_state_t<Rank> &state, double pi_over_beta, int64_t buf_counter_simd) {
      if constexpr (Rank == 1) {
        execute_phase2_rank1<TolDigits>(state, pi_over_beta, buf_counter_simd);
        return;
      }

      // Higher ranks keep only one SIMD packet of partial sums live at a time.
      std::fill(sums_buf.begin(), sums_buf.end(), cbatch(dcomplex{0, 0}));
      for (int jb = 0; jb < buf_counter_simd; jb += simd_size) accumulate_simd_step<TolDigits>(state, jb, pi_over_beta);

      dcomplex *fiw_ptr = state.fk_vec.data();
      if constexpr (Rank == 2) {
        if (!grouped_target_idx.empty()) {
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
