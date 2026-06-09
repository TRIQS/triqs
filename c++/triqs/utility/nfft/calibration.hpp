// Copyright (c) 2017--present, The Simons Foundation
// This file is part of TRIQS and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#pragma once
#include "common.hpp"
#include "kernels/direct_type1.hpp"
#include "kernels/finufft.hpp"
#include "kernels/naf.hpp"
#include <chrono>
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <random>
#include <vector>

namespace triqs::utility::nfft {

  // ---- Calibration helpers ----

  enum class direct_backend_t { direct_type1, direct_naf, direct_chain };

  struct cost_model_t {
    double intercept = 0.0;
    double linear    = 0.0;

    double eval(int n) const {
      double const n_d = static_cast<double>(std::max(n, 1));
      return std::max(intercept + linear * n_d, 0.0);
    }
  };

  struct direct_dispatch_model_t {
    cost_model_t direct_type1;
    cost_model_t direct_naf;
    cost_model_t direct_chain;

    direct_backend_t pick(int n) const {
      auto best_backend = direct_backend_t::direct_type1;
      double best_cost  = direct_type1.eval(n);

      auto consider = [&](direct_backend_t backend, double cost) {
        if (cost < best_cost) {
          best_backend = backend;
          best_cost    = cost;
        }
      };

      consider(direct_backend_t::direct_naf, direct_naf.eval(n));
      consider(direct_backend_t::direct_chain, direct_chain.eval(n));
      return best_backend;
    }
  };

  constexpr int calibration_base_reps   = 3;
  constexpr int calibration_refine_reps = 2;
  constexpr double calibration_close_margin = 0.15;
  constexpr double calibration_prune_margin = 0.15;

  inline cost_model_t fit_affine_model(double t_lo, double t_hi, int n_lo, int n_hi) {
    if (n_lo == n_hi) return {.intercept = t_lo};

    double const linear    = (t_hi - t_lo) / (n_hi - n_lo);
    double const intercept = t_lo - linear * n_lo;
    return {.intercept = intercept, .linear = linear};
  }

  inline cost_model_t fit_affine_model(std::vector<int> const &ns, std::vector<double> const &ts) {
    if (ns.empty() || ns.size() != ts.size()) return {};
    if (ns.size() == 1) return {.intercept = ts.front()};

    long double sum_n  = 0.0L;
    long double sum_t  = 0.0L;
    long double sum_n2 = 0.0L;
    long double sum_nt = 0.0L;

    for (size_t i = 0; i < ns.size(); ++i) {
      long double const n = static_cast<long double>(ns[i]);
      long double const t = static_cast<long double>(ts[i]);
      sum_n += n;
      sum_t += t;
      sum_n2 += n * n;
      sum_nt += n * t;
    }

    long double const m     = static_cast<long double>(ns.size());
    long double const denom = m * sum_n2 - sum_n * sum_n;
    if (std::abs(denom) < std::numeric_limits<long double>::epsilon())
      return {.intercept = static_cast<double>(sum_t / m)};

    long double const linear    = (m * sum_nt - sum_n * sum_t) / denom;
    long double const intercept = (sum_t - linear * sum_n) / m;
    return {.intercept = static_cast<double>(intercept), .linear = static_cast<double>(linear)};
  }

  inline double median_from_samples(std::vector<double> samples) {
    if (samples.empty()) return std::numeric_limits<double>::max();
    auto const mid = samples.begin() + samples.size() / 2;
    std::nth_element(samples.begin(), mid, samples.end());
    return *mid;
  }

  template <std::size_t N, typename MeasureFn>
  std::array<double, N> adaptive_measure_candidates(std::array<bool, N> const &active, MeasureFn &&measure, int n) {
    std::array<std::vector<double>, N> timings;
    std::array<double, N> medians;
    medians.fill(std::numeric_limits<double>::max());

    auto recompute_medians = [&] {
      for (std::size_t i = 0; i < N; ++i)
        if (active[i]) medians[i] = median_from_samples(timings[i]);
    };

    for (std::size_t i = 0; i < N; ++i)
      if (active[i])
        for (int rep = 0; rep < calibration_base_reps; ++rep) timings[i].push_back(measure(i, n));

    recompute_medians();

    double best = std::numeric_limits<double>::max();
    for (std::size_t i = 0; i < N; ++i)
      if (active[i]) best = std::min(best, medians[i]);

    std::array<bool, N> refine{};
    int refine_count = 0;
    for (std::size_t i = 0; i < N; ++i)
      if (active[i] && medians[i] <= best * (1.0 + calibration_close_margin)) {
        refine[i] = true;
        ++refine_count;
      }

    if (refine_count > 1) {
      for (std::size_t i = 0; i < N; ++i)
        if (refine[i])
          for (int rep = 0; rep < calibration_refine_reps; ++rep) timings[i].push_back(measure(i, n));
      recompute_medians();
    }

    return medians;
  }

  inline std::vector<int> direct_model_sample_sizes(int n_lo, int n_hi) {
    std::vector<int> sizes;
    sizes.reserve(4);

    auto push_unique = [&](int n) {
      n = std::clamp(n, 1, n_hi);
      if (sizes.empty() || sizes.back() != n) sizes.push_back(n);
    };

    push_unique(n_lo);
    push_unique(4 * n_lo);
    push_unique(16 * n_lo);
    push_unique(n_hi);
    return sizes;
  }

  template <std::size_t N>
  void prune_dominated_candidates(std::array<bool, N> &active, std::array<std::vector<int>, N> &sample_ns,
                                  std::array<std::vector<double>, N> &sample_ts, int current_n, int n_hi) {
    if (std::count(active.begin(), active.end(), true) <= 1) return;

    std::array<cost_model_t, N> models;
    for (std::size_t i = 0; i < N; ++i)
      if (active[i] && sample_ns[i].size() >= 2) models[i] = fit_affine_model(sample_ns[i], sample_ts[i]);

    for (std::size_t i = 0; i < N; ++i) {
      if (!active[i] || sample_ns[i].size() < 2) continue;

      bool dominated      = false;
      double prune_anchor = 0.0;

      for (std::size_t j = 0; j < N; ++j) {
        if (i == j || !active[j] || sample_ns[j].size() < 2) continue;

        double const current_cost = models[i].eval(current_n);
        double const rival_cost   = models[j].eval(current_n);
        double const high_cost    = models[i].eval(n_hi);
        double const rival_high   = models[j].eval(n_hi);

        if (current_cost > rival_cost * (1.0 + calibration_prune_margin) &&
            high_cost > rival_high * (1.0 + calibration_prune_margin)) {
          dominated    = true;
          prune_anchor = std::max(high_cost, rival_high * (1.0 + calibration_prune_margin));
          break;
        }
      }

      if (!dominated) continue;

      if (sample_ns[i].empty() || sample_ns[i].back() != n_hi) {
        sample_ns[i].push_back(n_hi);
        sample_ts[i].push_back(prune_anchor);
      } else {
        sample_ts[i].back() = std::max(sample_ts[i].back(), prune_anchor);
      }
      active[i] = false;
    }
  }

  // Linear-fit crossover: given timings of two kernels at n_lo and n_hi,
  // find the buffer size where they cross. Returns threshold clamped to [0, buf_size+1].
  inline int linear_crossover(double a_lo, double a_hi, double b_lo, double b_hi, int n_lo, int n_hi, int buf_size) {
    // Approximate both timings by affine models:
    //   t_a(n) ≈ a_slope * n + a_intercept,
    //   t_b(n) ≈ b_slope * n + b_intercept.
    double a_slope     = (a_hi - a_lo) / (n_hi - n_lo);
    double a_intercept = a_lo - a_slope * n_lo;
    double b_slope     = (b_hi - b_lo) / (n_hi - n_lo);
    double b_intercept = b_lo - b_slope * n_lo;

    int threshold;
    if (a_slope > b_slope && b_intercept > a_intercept)
      threshold = static_cast<int>((b_intercept - a_intercept) / (a_slope - b_slope));
    else if (a_slope <= b_slope)
      threshold = buf_size + 1; // a always wins
    else
      threshold = 0; // b always wins

    return std::clamp(threshold, 0, buf_size + 1);
  }

  // FINUFFT is sensitive to source-point geometry, so calibration uses a
  // deterministic pseudo-random cloud that matches the benchmark/workload
  // distribution better than evenly spaced taus.
  template <int Rank> void fill_dummy_tau(shared_state_t<Rank> &state, int n) {
    std::mt19937_64 rng(0x9f5bd2c6e17a42d1ULL + static_cast<uint64_t>(Rank) * 0x100000001b3ULL);
    std::uniform_real_distribution<double> tau_dist(0.0, state.beta);
    std::normal_distribution<double> val_dist(0.0, 1.0);

    for (int j = 0; j < n; ++j) {
      for (int r = 0; r < Rank; ++r) state.x_arr(r, j) = tau_dist(rng);
      state.fx_arr[j] = dcomplex(val_dist(rng), val_dist(rng));
    }
  }

  // Median of n_reps evaluations of fn(n), which is less optimistic than
  // best-of-n and gives more stable dispatch thresholds under CPU jitter.
  template <typename F> double median_of_n(F &&fn, int n, int n_reps = calibration_base_reps + calibration_refine_reps) {
    std::vector<double> timings;
    timings.reserve(n_reps);
    for (int rep = 0; rep < n_reps; ++rep) timings.push_back(fn(n));
    return median_from_samples(std::move(timings));
  }

  // Calibration reuses one dummy buffer and rewinds it before each timing.
  template <int Rank> struct state_saver_t {
    shared_state_t<Rank> &state;
    int n;
    nda::array<double, 2> x_save;
    nda::vector<dcomplex> fx_save;

    state_saver_t(shared_state_t<Rank> &state_, int n_) : state(state_), n(n_), x_save(Rank, n_), fx_save(n_) { save(); }
    ~state_saver_t() { cleanup(); }

    void save() {
      x_save  = state.x_arr(nda::range(0, Rank), nda::range(0, n));
      fx_save = state.fx_arr(nda::range(0, n));
    }

    void restore() {
      state.x_arr(nda::range(0, Rank), nda::range(0, n)) = x_save;
      state.fx_arr(nda::range(0, n))                     = fx_save;
    }

    void cleanup() {
      restore();
      state.fk_vec      = 0;
      state.buf_counter = 0;
    }
  };

  // ---- Calibration functions ----

  // Automatic non-uniform dispatch uses measured outer cutoffs:
  //   n < direct_cutoff   -> direct kernel
  //   n >= direct_cutoff  -> FINUFFT
  // with a second split between type3 and type1_gather inside the FINUFFT region.
  // Inside the direct region we use an analytical size-dependent model because
  // Google Benchmark shows the direct kernels scale linearly with the number of sources.
  struct dispatch_plan_t {
    int direct_cutoff;                // direct vs best FINUFFT cutoff
    int finufft_switch_n;             // type3 vs type1_gather cutoff inside the FINUFFT region
    bool type3_for_small_n;           // otherwise type1_gather handles the small-n side
    direct_dispatch_model_t direct_model;
  };

  inline void hash_mix(uint64_t &seed, uint64_t value) {
    seed ^= value + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
  }

  template <typename Int> inline uint64_t hash_int(Int value) { return std::bit_cast<uint64_t>(static_cast<int64_t>(value)); }

  inline uint64_t hash_double(double value) { return std::bit_cast<uint64_t>(value); }

  template <std::size_t Rank>
  uint64_t dispatch_cache_key(std::vector<std::array<target_mf_t, Rank>> const &target_mf, int buf_size, int tol_digits) {
    uint64_t seed = 0x84222325cbf29ce4ULL;
    hash_mix(seed, static_cast<uint64_t>(Rank));
    hash_mix(seed, static_cast<uint64_t>(buf_size));
    hash_mix(seed, static_cast<uint64_t>(tol_digits));
    hash_mix(seed, static_cast<uint64_t>(target_mf.size()));

    for (auto const &mf_arr : target_mf)
      for (auto const &mf : mf_arr) {
        hash_mix(seed, hash_int(mf.n));
        hash_mix(seed, hash_double(mf.beta));
      }

    return seed;
  }

  template <int TolDigits = 12, int Rank, typename ChainKernel, typename NafKernel, typename FinufftKernel>
  dispatch_plan_t calibrate_dispatch_nonuniform(shared_state_t<Rank> &state, kernel_direct_type1_t<Rank> &direct_type1_kernel,
                                                ChainKernel &chain_kernel, NafKernel &naf_kernel, FinufftKernel &finufft_kernel) {
    using clock = std::chrono::steady_clock;
    int const requested_n_lo = [] {
      if constexpr (Rank == 1)
        return 8;
      else
        return 64;
    }();
    int const n_hi = std::min(4096, state.buf_size);
    int const n_lo = std::min(requested_n_lo, n_hi);
    fill_dummy_tau(state, n_hi);
    state_saver_t saver(state, n_hi);

    auto measure_direct_type1 = [&](int n) {
      saver.restore();
      state.buf_counter = n;
      state.fk_vec      = 0;
      auto t0           = clock::now();
      direct_type1_kernel.template execute<12>(state);
      return std::chrono::duration<double>(clock::now() - t0).count();
    };

    auto measure_sparse_direct = [&](auto &kernel, int n) {
      saver.restore();
      state.buf_counter = n;
      state.fk_vec      = 0;
      auto t0           = clock::now();
      kernel.template execute<TolDigits>(state);
      return std::chrono::duration<double>(clock::now() - t0).count();
    };

    nda::array<dcomplex, 1> dummy_fiw(state.n_targets);

    auto measure_type1_gather = [&](int n) {
      saver.restore();
      state.buf_counter = n;
      // type1_gather needs the type-1 coordinate transform before FINUFFT sees the points.
      apply_type1_coord_transform<TolDigits>(state, n);
      dummy_fiw = 0;
      auto t0   = clock::now();
      finufft_kernel.execute_type1_gather(state, dummy_fiw);
      return std::chrono::duration<double>(clock::now() - t0).count();
    };

    auto measure_type3 = [&](int n) {
      saver.restore();
      state.buf_counter = n;
      dummy_fiw         = 0;
      auto t0           = clock::now();
      finufft_kernel.execute_type3(state, dummy_fiw);
      return std::chrono::duration<double>(clock::now() - t0).count();
    };

    // Warmup
    measure_type1_gather(n_lo);
    measure_type3(n_lo);
    measure_direct_type1(n_lo);
    measure_sparse_direct(chain_kernel, n_lo);
    measure_sparse_direct(naf_kernel, n_lo);

    auto const direct_ns = direct_model_sample_sizes(n_lo, n_hi);
    std::array<std::vector<int>, 3> direct_sample_ns;
    std::array<std::vector<double>, 3> direct_sample_ts;
    std::array<bool, 3> direct_active{true, true, true};

    for (std::size_t sample_idx = 0; sample_idx < direct_ns.size(); ++sample_idx) {
      int const n = direct_ns[sample_idx];
      auto const medians = adaptive_measure_candidates<3>(
         direct_active,
         [&](std::size_t backend_idx, int m) {
           switch (backend_idx) {
             case 0: return measure_direct_type1(m);
             case 1: return measure_sparse_direct(chain_kernel, m);
             default: return measure_sparse_direct(naf_kernel, m);
           }
         },
         n);

      for (std::size_t backend_idx = 0; backend_idx < direct_active.size(); ++backend_idx) {
        if (!direct_active[backend_idx]) continue;
        direct_sample_ns[backend_idx].push_back(n);
        direct_sample_ts[backend_idx].push_back(medians[backend_idx]);
      }

      if (sample_idx + 1 >= 2 && n < n_hi) {
        // Once two points are available, drop direct backends that are already
        // clearly dominated over the remaining calibration interval [n, n_hi].
        prune_dominated_candidates(direct_active, direct_sample_ns, direct_sample_ts, n, n_hi);
      }
    }

    auto const &direct_type1_samples = direct_sample_ts[0];
    auto const &direct_chain_samples = direct_sample_ts[1];
    auto const &direct_naf_samples   = direct_sample_ts[2];

    double const direct_type1_lo = direct_type1_samples.front();
    double const direct_type1_hi = direct_type1_samples.back();
    double const direct_chain_lo = direct_chain_samples.front();
    double const direct_chain_hi = direct_chain_samples.back();
    double const direct_naf_lo   = direct_naf_samples.front();
    double const direct_naf_hi   = direct_naf_samples.back();

    double const dir_lo = std::min({direct_type1_lo, direct_chain_lo, direct_naf_lo});
    double const dir_hi = std::min({direct_type1_hi, direct_chain_hi, direct_naf_hi});

    auto const finufft_pair_lo =
       adaptive_measure_candidates<2>(std::array<bool, 2>{true, true},
                                      [&](std::size_t idx, int m) { return idx == 0 ? measure_type1_gather(m) : measure_type3(m); }, n_lo);
    auto const finufft_pair_hi =
       adaptive_measure_candidates<2>(std::array<bool, 2>{true, true},
                                      [&](std::size_t idx, int m) { return idx == 0 ? measure_type1_gather(m) : measure_type3(m); }, n_hi);

    double tg_lo = finufft_pair_lo[0];
    double tg_hi = finufft_pair_hi[0];
    double t3_lo = finufft_pair_lo[1];
    double t3_hi = finufft_pair_hi[1];

    bool const type3_for_small_n = t3_lo <= tg_lo;
    int const finufft_switch_n   =
       type3_for_small_n ? linear_crossover(t3_lo, t3_hi, tg_lo, tg_hi, n_lo, n_hi, state.buf_size) :
                           linear_crossover(tg_lo, tg_hi, t3_lo, t3_hi, n_lo, n_hi, state.buf_size);

    double const finufft_lo = std::min(t3_lo, tg_lo);
    double const finufft_hi = std::min(t3_hi, tg_hi);

    int direct_cutoff = linear_crossover(dir_lo, dir_hi, finufft_lo, finufft_hi, n_lo, n_hi, state.buf_size);
    if constexpr (Rank == 1) direct_cutoff = std::max(direct_cutoff, 96);

    if (std::getenv("TRIQS_NFFT_DEBUG_AUTO")) {
      std::fprintf(stderr,
                   "[nfft auto] rank=%d n_targets=%lld n_lo=%d n_hi=%d "
                   "dt1=(%.3e,%.3e) chain=(%.3e,%.3e) naf=(%.3e,%.3e) "
                   "tg=(%.3e,%.3e) t3=(%.3e,%.3e) direct_cutoff=%d finufft_switch=%d type3_small=%d\n",
                   Rank, static_cast<long long>(state.n_targets), n_lo, n_hi, direct_type1_lo, direct_type1_hi, direct_chain_lo,
                   direct_chain_hi, direct_naf_lo, direct_naf_hi, tg_lo, tg_hi, t3_lo, t3_hi, direct_cutoff, finufft_switch_n,
                   type3_for_small_n);
    }

    saver.cleanup();
    return {.direct_cutoff = direct_cutoff,
            .finufft_switch_n = finufft_switch_n,
            .type3_for_small_n = type3_for_small_n,
            .direct_model = {.direct_type1 = fit_affine_model(direct_sample_ns[0], direct_type1_samples),
                             .direct_naf = fit_affine_model(direct_sample_ns[2], direct_naf_samples),
                             .direct_chain = fit_affine_model(direct_sample_ns[1], direct_chain_samples)}};
  }

  // Calibrate dispatch threshold for type1 automatic mode (direct_type1 vs FINUFFT type1).
  // The direct path uses raw tau, while the FINUFFT path needs coordinate transformation.
  template <int TolDigits = 12, int Rank, typename FinufftKernel>
  int calibrate_dispatch_type1(shared_state_t<Rank> &state, kernel_direct_type1_t<Rank> &direct_kernel, FinufftKernel &finufft_kernel,
                               nda::array<dcomplex, Rank> &fk_arr, int common_factor) {
    using clock = std::chrono::steady_clock;

    int const n_hi = std::min(4096, state.buf_size);
    int const n_lo = std::clamp(n_hi / 64, 1, 64);

    fill_dummy_tau(state, n_hi);
    state_saver_t saver(state, n_hi);

    auto measure_direct = [&](int n) {
      saver.restore();
      state.buf_counter = n;
      state.fk_vec      = 0;
      auto t0           = clock::now();
      direct_kernel.execute(state);
      return std::chrono::duration<double>(clock::now() - t0).count();
    };

    nda::array<dcomplex, Rank> dummy_fiw(fk_arr.shape());

    auto measure_type1 = [&](int n) {
      saver.restore();
      state.buf_counter = n;
      // FINUFFT type1 works on transformed coordinates and rephased source values.
      apply_type1_coord_transform<TolDigits>(state, n);
      dummy_fiw = 0;
      auto t0   = clock::now();
      finufft_kernel.execute_type1(state, dummy_fiw, fk_arr, common_factor);
      return std::chrono::duration<double>(clock::now() - t0).count();
    };

    // Warmup
    measure_type1(n_lo);
    measure_direct(n_lo);

    auto const type1_pair_lo =
       adaptive_measure_candidates<2>(std::array<bool, 2>{true, true},
                                      [&](std::size_t idx, int m) { return idx == 0 ? measure_direct(m) : measure_type1(m); }, n_lo);
    auto const type1_pair_hi =
       adaptive_measure_candidates<2>(std::array<bool, 2>{true, true},
                                      [&](std::size_t idx, int m) { return idx == 0 ? measure_direct(m) : measure_type1(m); }, n_hi);

    double dir_lo = type1_pair_lo[0];
    double dir_hi = type1_pair_hi[0];
    double t1_lo  = type1_pair_lo[1];
    double t1_hi  = type1_pair_hi[1];

    int direct_cutoff = linear_crossover(dir_lo, dir_hi, t1_lo, t1_hi, n_lo, n_hi, state.buf_size);

    saver.cleanup();
    return direct_cutoff;
  }

} // namespace triqs::utility::nfft
