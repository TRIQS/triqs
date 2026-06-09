// Copyright (c) 2017--present, The Simons Foundation
// This file is part of TRIQS and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#pragma once
#include "../common.hpp"

namespace triqs::utility::nfft {

  namespace detail {

    struct tuned_finufft_bucket_t {
      int max_n_points;
      double upsampfac;
      int spread_max_sp_size;
    };

    // Buckets are keyed by the number of buffered source points. They only matter when
    // execution actually goes through a FINUFFT path; direct kernels ignore them entirely.
    template <int Rank> auto const &tuned_finufft_buckets() {
      if constexpr (Rank == 1) {
        static constexpr std::array<tuned_finufft_bucket_t, 4> buckets{{
           {16, 1.20, 4500},
           {64, 1.18, 2500},
           {256, 1.16, 10000},
           {std::numeric_limits<int>::max(), 1.20, 6250},
        }};
        return buckets;
      } else {
        static constexpr std::array<tuned_finufft_bucket_t, 2> buckets{{
           {16384, 1.18, 100000},
           {std::numeric_limits<int>::max(), 1.1775, 150000},
        }};
        return buckets;
      }
    }

    template <int Rank, int TolDigits> int tuned_finufft_bucket_index(int n_points) {
      if constexpr (TolDigits > 8) return 0;
      auto const &buckets = tuned_finufft_buckets<Rank>();
      for (int i = 0; i < static_cast<int>(buckets.size()); ++i)
        if (n_points <= buckets[i].max_n_points) return i;
      return static_cast<int>(buckets.size()) - 1;
    }

    template <int Rank, int TolDigits> int tuned_finufft_bucket_count() {
      if constexpr (TolDigits > 8) return 1;
      return static_cast<int>(tuned_finufft_buckets<Rank>().size());
    }

    template <int Rank, int TolDigits> void apply_tuned_finufft_opts(finufft_opts &opts, int bucket_index) {
      if constexpr (TolDigits > 8) return;
      auto const &bucket          = tuned_finufft_buckets<Rank>()[bucket_index];
      opts.upsampfac              = bucket.upsampfac;
      opts.spread_max_sp_size     = bucket.spread_max_sp_size;
    }

  } // namespace detail

  template <int Rank, int TolDigits = 8> struct kernel_finufft_t {

    kernel_finufft_t() = default;

    /// Initialize for type 1 (non-uniform tau -> uniform Matsubara grid)
    void init_type1(std::array<int64_t, Rank> const &niws, int /*buf_size*/, double tol) {
      // FINUFFT orders dimensions from fastest to slowest, opposite to our rank order.
      auto Ns = std::vector(niws.rbegin(), niws.rend());
      init_bucketed_plans(type1_plans_, [&](finufft_opts &local_opts, finufft_plan *raw_plan) {
        return finufft_makeplan(1, Rank, Ns.data(), 1, 1, tol, raw_plan, &local_opts);
      });
    }

    /// Initialize for type 1 + gather (non-uniform targets via bounding uniform grid)
    void init_type1_gather(std::vector<std::array<target_mf_t, Rank>> const &target_mf, int64_t n_targets, double tol) {
      // Find max |n| per dimension to determine bounding uniform grid
      std::array<int64_t, Rank> max_abs_n{};
      for (int64_t d = 0; d < n_targets; ++d)
        for (int r = 0; r < Rank; ++r) max_abs_n[r] = std::max(max_abs_n[r], std::abs(static_cast<int64_t>(target_mf[d][r].n)));

      // Grid size per dimension: covers modes [-N/2, N/2-1], need N/2 > max_abs_n
      for (int r = 0; r < Rank; ++r) gather_niws[r] = 2 * (max_abs_n[r] + 1);

      // After x_r = 2π(τ_r/β - 1/2), the half-grid shift contributes a factor
      // (-1)^{n_r} per rank, so the total gather sign is (-1)^{Σ_r n_r}.
      gather_indices.resize(n_targets);
      gather_signs.resize(n_targets);

      for (int64_t d = 0; d < n_targets; ++d) {
        int64_t flat    = 0;
        int parity      = 0;
        for (int r = 0; r < Rank; ++r) {
          int64_t arr_idx = target_mf[d][r].n + gather_niws[r] / 2;
          if (r > 0) flat *= gather_niws[r];
          flat += arr_idx;
          long const n = target_mf[d][r].n;
          parity ^= static_cast<int>(static_cast<unsigned long>(n < 0 ? -n : n) & 1UL);
        }
        gather_indices[d] = flat;
        gather_signs[d]   = parity ? -1 : 1;
      }

      // Allocate output array for type1 transform
      gather_fk_arr.resize(nda::stdutil::make_std_array<long>(gather_niws));

      // Init FINUFFT type1 plan with the bounding grid
      auto Ns = std::vector(gather_niws.rbegin(), gather_niws.rend());
      init_bucketed_plans(type1_plans_, [&](finufft_opts &local_opts, finufft_plan *raw_plan) {
        return finufft_makeplan(1, Rank, Ns.data(), 1, 1, tol, raw_plan, &local_opts);
      });
    }

    /// Initialize for type 3 (non-uniform tau -> non-uniform Matsubara)
    void init_type3(std::vector<std::array<target_mf_t, Rank>> const &target_mf, int64_t n_targets, double tol) {
      init_type3_targets(target_mf, n_targets);
      init_bucketed_plans(type3_plans_, [&](finufft_opts &local_opts, finufft_plan *raw_plan) {
        return finufft_makeplan(3, Rank, nullptr, 1, 1, tol, raw_plan, &local_opts);
      });
    }

    /// Initialize both type1_gather and type3 paths (for 3-way automatic dispatch)
    void init_type1_gather_and_type3(std::vector<std::array<target_mf_t, Rank>> const &target_mf, int64_t n_targets, double tol) {
      init_type1_gather(target_mf, n_targets, tol);

      init_type3_targets(target_mf, n_targets);
      init_bucketed_plans(type3_plans_, [&](finufft_opts &local_opts, finufft_plan *raw_plan) {
        return finufft_makeplan(3, Rank, nullptr, 1, 1, tol, raw_plan, &local_opts);
      });
    }

    void execute_type1(shared_state_t<Rank> &state, nda::array_view<dcomplex, Rank> fiw_arr, nda::array<dcomplex, Rank> &fk_arr,
                       int common_factor) {
      auto const &plan = select_type1_plan(state.buf_counter);
      set_pts(state, nullptr, plan);
      check_finufft(finufft_execute(plan.get(), state.fx_arr.data(), fk_arr.data()));
      // The half-grid shift contributes one sign per odd mode, hence the parity factor below.
      for (auto idx_tpl : fiw_arr.indices()) {
        auto idx_sum = std::apply([](auto... idx) { return (idx + ... + 0); }, idx_tpl);
        int factor   = common_factor * (idx_sum % 2 ? -1 : 1);
        std::apply(fiw_arr, idx_tpl) += std::apply(fk_arr, idx_tpl) * factor;
      }
    }

    void execute_type1_gather(shared_state_t<Rank> &state, nda::array_view<dcomplex, 1> fiw_vec) {
      auto const &plan = select_type1_plan(state.buf_counter);
      set_pts(state, nullptr, plan);
      check_finufft(finufft_execute(plan.get(), state.fx_arr.data(), gather_fk_arr.data()));
      // Gather only the requested odd modes from the enclosing uniform grid.
      auto const *fk_data = gather_fk_arr.data();
      for (int64_t d = 0; d < static_cast<int64_t>(gather_indices.size()); ++d)
        fiw_vec(d) += fk_data[gather_indices[d]] * static_cast<double>(gather_signs[d]);
    }

    void execute_type3(shared_state_t<Rank> &state, nda::array_view<dcomplex, 1> fiw_vec) {
      auto const &p = select_type3_plan(state.buf_counter);
      set_pts(state, &s_arr, p);
      check_finufft(finufft_execute(p.get(), state.fx_arr.data(), state.fk_vec.data()));
      fiw_vec += state.fk_vec;
    }
    /// Release the type3 plan (after calibration picks type1_gather)
    void release_type3() {
      type3_plans_.clear();
      s_arr = {};
    }

    /// Release the type1_gather plan/data once automatic dispatch decides to stay on type3.
    void release_type1_gather() {
      if (!type3_plans_.empty()) type1_plans_ = std::move(type3_plans_);
      gather_indices.clear();
      gather_signs.clear();
      gather_fk_arr = {};
      gather_niws   = {};
    }

    private:
    // FINUFFT plans bucketed by source count.
    std::vector<finufft_plan_ptr> type1_plans_;
    std::vector<finufft_plan_ptr> type3_plans_; // separate type3 plans (when both paths coexist)

    // Type-3 target frequencies.
    nda::array<double, 2> s_arr;

    // Type1-gather metadata: enclosing uniform grid, gather map, and output buffer.
    std::array<int64_t, Rank> gather_niws{};
    std::vector<int64_t> gather_indices;
    std::vector<int> gather_signs;
    nda::array<dcomplex, Rank> gather_fk_arr;

    void init_type3_targets(std::vector<std::array<target_mf_t, Rank>> const &target_mf, int64_t n_targets) {
      s_arr.resize(Rank, n_targets);
      for (int r = 0; r < Rank; ++r)
        for (int64_t d = 0; d < n_targets; ++d) s_arr(r, d) = std::imag(dcomplex(target_mf[d][r]));
    }

    template <typename MakePlanFn> void init_bucketed_plans(std::vector<finufft_plan_ptr> &plans, MakePlanFn &&make_plan) {
      plans.clear();
      plans.reserve(detail::tuned_finufft_bucket_count<Rank, TolDigits>());
      for (int bucket = 0; bucket < detail::tuned_finufft_bucket_count<Rank, TolDigits>(); ++bucket) {
        finufft_opts local_opts{};
        finufft_default_opts(&local_opts);
        local_opts.nthreads = 1;
        detail::apply_tuned_finufft_opts<Rank, TolDigits>(local_opts, bucket);
        finufft_plan raw_plan = nullptr;
        check_finufft(make_plan(local_opts, &raw_plan));
        plans.emplace_back(raw_plan);
      }
    }

    int select_bucket_index(int n_points, std::vector<finufft_plan_ptr> const &plans) const {
      if (plans.size() <= 1) return 0;
      // Pick the plan tuned for the current source count.
      return detail::tuned_finufft_bucket_index<Rank, TolDigits>(n_points);
    }

    finufft_plan_ptr const &select_plan(std::vector<finufft_plan_ptr> const &plans, int n_points) const {
      return plans[select_bucket_index(n_points, plans)];
    }

    finufft_plan_ptr const &select_type1_plan(int n_points) const { return select_plan(type1_plans_, n_points); }

    finufft_plan_ptr const &select_type3_plan(int n_points) const {
      // After calibration, the surviving FINUFFT path may be stored in either vector.
      auto const &plans = type3_plans_.empty() ? type1_plans_ : type3_plans_;
      return select_plan(plans, n_points);
    }

    // FINUFFT expects coordinates in reverse rank order.
    void set_pts(shared_state_t<Rank> &state, nda::array<double, 2> *tgt, finufft_plan_ptr const &p) {
      auto _ = nda::range::all;
      auto n_tgt = tgt ? state.n_targets : int64_t{0};
      auto t     = [&](int r) -> double * { return tgt ? (*tgt)(r, _).data() : nullptr; };
      if constexpr (Rank == 1)
        check_finufft(finufft_setpts(p.get(), state.buf_counter, state.x_arr(0, _).data(), nullptr, nullptr, n_tgt, t(0), nullptr, nullptr));
      else if constexpr (Rank == 2)
        check_finufft(
           finufft_setpts(p.get(), state.buf_counter, state.x_arr(1, _).data(), state.x_arr(0, _).data(), nullptr, n_tgt, t(1), t(0), nullptr));
      else
        check_finufft(finufft_setpts(p.get(), state.buf_counter, state.x_arr(2, _).data(), state.x_arr(1, _).data(), state.x_arr(0, _).data(), n_tgt,
                                     t(2), t(1), t(0)));
    }
  };

} // namespace triqs::utility::nfft
