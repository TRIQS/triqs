// Copyright (c) 2017--present, The Simons Foundation
// This file is part of TRIQS and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#pragma once
#include "common.hpp"
#include "kernels.hpp"
#include "calibration.hpp"
#include <mutex>
#include <optional>
#include <unordered_map>
#include <variant>

namespace triqs::utility::nfft {

  template <int Rank> struct buffer_t {

    static_assert(Rank >= 1 and Rank <= 3, "buffer_t only supports Rank 1, 2, and 3");

    template <int TolDigits>
    using finufft_kernel_t = kernel_finufft_t<Rank, TolDigits>;

    using finufft_variant_t = std::variant<std::monostate, finufft_kernel_t<6>, finufft_kernel_t<8>, finufft_kernel_t<10>, finufft_kernel_t<12>>;

    using do_nfft_fn_t = void (*)(buffer_t &);

    buffer_t() = default;

    /// Type 1: non-uniform tau -> uniform Matsubara grid (with automatic NAF/FINUFFT dispatch)
    buffer_t(array_view<dcomplex, Rank> fiw_arr_, int buf_size_, double beta_, double tol_ = 1e-8)
       : fiw_arr(std::move(fiw_arr_)),
         niws(nda::stdutil::make_std_array<int64_t>(fiw_arr.shape())),
         fk_arr(fiw_arr.shape()),
         tol(tol_) {

      state_.buf_size = buf_size_;
      state_.beta     = beta_;
      state_.x_arr.resize(Rank, buf_size_);
      state_.fx_arr.resize(buf_size_);

      for (int n : niws) {
        if (n % 2 != 0) NDA_RUNTIME_ERROR << " dimension with uneven frequency count not allowed in NFFT Buffer \n";
        common_factor *= (n / 2) % 2 ? -1 : 1;
      }

      with_tol_digits([&]<int TolDigits>() { init_type1_impl<TolDigits>(beta_); });
    }

    /// Non-uniform target constructor: automatic dispatch, FINUFFT type3, or direct DFT
    buffer_t(nda::array_view<dcomplex, 1> fiw_vec_, std::vector<std::array<target_mf_t, Rank>> target_mf_, int buf_size_,
             double tol_ = 1e-8, type_t type = type_t::automatic)
       : type_(type), fiw_vec(std::move(fiw_vec_)), tol(tol_) {

      state_.buf_size  = buf_size_;
      state_.n_targets = static_cast<int64_t>(target_mf_.size());
      state_.x_arr.resize(Rank, buf_size_);
      state_.fx_arr.resize(buf_size_);
      state_.fk_vec.resize(state_.n_targets);

      with_tol_digits([&]<int TolDigits>() { init_nonuniform_impl<TolDigits>(target_mf_); });
    }

    /// Convenience constructor for Rank=1: accepts vector of matsubara_freq directly
    buffer_t(nda::array_view<dcomplex, 1> fiw_vec_, std::vector<target_mf_t> const &target_mf_, int buf_size_, double tol_ = 1e-8,
             type_t type = type_t::automatic)
      requires(Rank == 1)
       : buffer_t(
            std::move(fiw_vec_),
            [&] {
              std::vector<std::array<target_mf_t, 1>> result;
              result.reserve(target_mf_.size());
              for (auto const &mf : target_mf_) result.push_back({mf});
              return result;
            }(),
            buf_size_, tol_, type) {}

    ~buffer_t() {
      if (state_.buf_counter != 0) std::cout << " WARNING: Points in NFFT Buffer lost \n";
    }

    buffer_t(buffer_t const &)            = delete;
    buffer_t &operator=(buffer_t const &) = delete;
    buffer_t(buffer_t &&)                 = default;
    buffer_t &operator=(buffer_t &&rhs) noexcept {
      // array_view::operator= does deep copy; we need rebind via destroy + placement new
      if (this != &rhs) {
        std::destroy_at(this);
        std::construct_at(this, std::move(rhs));
      }
      return *this;
    }

    void rebind(array_view<dcomplex, Rank> new_fiw_arr) {
      flush();
      TRIQS_ASSERT((new_fiw_arr.shape() == fiw_arr.shape() or fiw_arr.empty())
                   and " Nfft Buffer: Rebind to array of different shape not allowed ");
      fiw_arr.rebind(new_fiw_arr);
    }

    [[gnu::always_inline]] inline void push_back(std::array<double, Rank> const &tau_arr, dcomplex ftau) {
      if (state_.x_arr.empty()) [[unlikely]] throw_uninitialized_buffer_error();

      using x_arr_t = std::remove_reference_t<decltype(state_.x_arr)>;
      static_assert(x_arr_t::is_stride_order_C());

      int const idx = state_.buf_counter;

      if (type_ == type_t::type1 && direct_cutoff_ == 0) {
        // Pure FINUFFT type1 stores points in transformed coordinates so `flush()`
        // can hand them straight to FINUFFT with no extra preprocessing.
        double tau_sum = 0.0;
        double const pi_over_beta     = M_PI / state_.beta;
        double const two_pi_over_beta = 2.0 * pi_over_beta;
        poet::static_for<Rank>([&](auto r) {
          double const tau = tau_arr[r];
          tau_sum += tau;
          state_.x_arr(r, idx) = std::fma(two_pi_over_beta, tau, -M_PI);
        });
        state_.fx_arr[idx] = cis<12>(pi_over_beta * tau_sum) * ftau;
      } else {
        // Every other mode keeps the raw τ values and decides the transform at flush time.
        poet::static_for<Rank>([&](auto r) { state_.x_arr(r, idx) = tau_arr[r]; });
        state_.fx_arr[idx] = ftau;
      }

      if (++state_.buf_counter >= state_.buf_size) flush();
    }

    void flush() {
      if (state_.x_arr.empty()) [[unlikely]] throw_uninitialized_buffer_error();
      if (state_.buf_counter == 0) return;
      if (not do_nfft_fn_) [[unlikely]] throw_uninitialized_backend_error();
      do_nfft_fn_(*this);
      state_.buf_counter = 0;
    }

    private:
    static constexpr int64_t max_type1_dispatch_targets = 100'000;

    [[gnu::cold, noreturn]] static void throw_uninitialized_buffer_error() {
      NDA_RUNTIME_ERROR << " Using a default-constructed NFFT Buffer is not allowed\n";
    }

    [[gnu::cold, noreturn]] static void throw_uninitialized_backend_error() {
      NDA_RUNTIME_ERROR << " Nfft Buffer backend was not initialized\n";
    }

    // Public execution mode selected by the constructor.
    type_t type_ = type_t::type1;
    // Automatic mode keeps a size-dependent direct model, then falls back to the
    // measured FINUFFT cutoffs outside the direct region.
    bool type3_for_small_n_ = false;

    // Buffered source state shared with the kernels.
    shared_state_t<Rank> state_;

    // User-facing output views and type-1 scratch storage.
    nda::array_view<dcomplex, Rank> fiw_arr;
    nda::array_view<dcomplex, 1> fiw_vec;
    std::array<int64_t, Rank> niws{};
    nda::array<dcomplex, Rank> fk_arr;
    int common_factor = 1;
    double tol        = 1e-8;

    // Backend kernels: exactly one FINUFFT tolerance bucket plus the direct candidates.
    finufft_variant_t finufft_kernel_;
    std::optional<kernel_direct_type1_t<Rank>> direct_type1_kernel_;
    std::optional<kernel_chain_t<Rank>> chain_kernel_;
    std::optional<kernel_naf_t<Rank>> naf_kernel_;
    std::optional<direct_dispatch_model_t> direct_dispatch_model_;

    // Cached dispatch thresholds used at flush time.
    int direct_cutoff_    = 0;
    int finufft_switch_n_ = 0;
    do_nfft_fn_t do_nfft_fn_ = nullptr;

    template <int TolDigits> auto &emplace_finufft() {
      return finufft_kernel_.template emplace<finufft_kernel_t<TolDigits>>();
    }

    template <int TolDigits> auto &get_finufft() { return std::get<finufft_kernel_t<TolDigits>>(finufft_kernel_); }

    template <typename Builder> void with_tol_digits(Builder &&builder) {
      // Pick one compile-time tolerance bucket once, then build only that backend.
      auto params = std::make_tuple(poet::dispatch_param<tol_digits_seq_t>{tol_digits_bucket(tol)});
      poet::dispatch(poet::throw_on_no_match, std::forward<Builder>(builder), params);
    }

    template <int TolDigits> void init_type1_impl(double beta_) {
      auto &finufft = emplace_finufft<TolDigits>();
      finufft.init_type1(niws, state_.buf_size, tol);

      int64_t n_targets = 1;
      for (auto n : niws) n_targets *= n;
      if (n_targets <= max_type1_dispatch_targets) {
        state_.n_targets = n_targets;
        state_.fk_vec.resize(n_targets);

        // Only small-ish uniform grids are worth calibrating against the exact direct kernel.
        auto target_mf = build_uniform_target_mf(beta_);
        state_.init_direct_common(target_mf);
        direct_type1_kernel_.emplace(state_, target_mf);
        direct_cutoff_ = calibrate_dispatch_type1<12>(state_, *direct_type1_kernel_, finufft, fk_arr, common_factor);
      }

      // Flush-time dispatch is reduced to one indirect call after construction.
      do_nfft_fn_ = &buffer_t::template do_nfft_impl<TolDigits>;
    }

    template <int TolDigits> void init_nonuniform_impl(std::vector<std::array<target_mf_t, Rank>> const &target_mf) {
      if (type_ == type_t::type3) {
        emplace_finufft<TolDigits>().init_type3(target_mf, state_.n_targets, tol);

      } else if (type_ == type_t::direct_type1) {
        state_.init_direct_common(target_mf);
        direct_type1_kernel_.emplace(state_, target_mf);

      } else if (type_ == type_t::direct_chain) {
        state_.init_direct_common(target_mf);
        chain_kernel_.emplace(state_);

      } else if (type_ == type_t::direct_type3) {
        state_.init_direct_common(target_mf);
        naf_kernel_.emplace(state_, state_.buf_size);

      } else if (type_ == type_t::type1_gather) {
        state_.beta = target_mf[0][0].beta;
        emplace_finufft<TolDigits>().init_type1_gather(target_mf, state_.n_targets, tol);

      } else if (type_ == type_t::automatic) {
        auto &finufft = emplace_finufft<TolDigits>();
        finufft.init_type1_gather_and_type3(target_mf, state_.n_targets, tol);
        state_.init_direct_common(target_mf);
        naf_kernel_.emplace(state_, state_.buf_size);
        direct_type1_kernel_.emplace(state_, target_mf);
        chain_kernel_.emplace(state_);

        auto const key = dispatch_cache_key(target_mf, state_.buf_size, TolDigits);
        static std::mutex cache_mutex;
        static std::unordered_map<uint64_t, dispatch_plan_t> cache;

        auto load_cached_plan = [&]() -> std::optional<dispatch_plan_t> {
          std::lock_guard lock(cache_mutex);
          if (auto it = cache.find(key); it != cache.end()) return it->second;
          return std::nullopt;
        };

        auto store_cached_plan = [&](dispatch_plan_t const &plan) {
          std::lock_guard lock(cache_mutex);
          cache.emplace(key, plan);
        };

        auto const plan = [&]() {
          if (auto cached = load_cached_plan()) return *cached;
          auto measured_plan =
             calibrate_dispatch_nonuniform<TolDigits>(state_, *direct_type1_kernel_, *chain_kernel_, *naf_kernel_, finufft);
          store_cached_plan(measured_plan);
          return measured_plan;
        }();
        direct_cutoff_       = plan.direct_cutoff;
        finufft_switch_n_    = plan.finufft_switch_n;
        type3_for_small_n_   = plan.type3_for_small_n;
        direct_dispatch_model_ = plan.direct_model;

        if (finufft_switch_n_ <= 0) {
          if (type3_for_small_n_)
            finufft.release_type3();
          else
            finufft.release_type1_gather();
        } else if (finufft_switch_n_ > state_.buf_size) {
          if (type3_for_small_n_)
            finufft.release_type1_gather();
          else
            finufft.release_type3();
        }

      } else {
        NDA_RUNTIME_ERROR << "buffer_t: unsupported type_t for non-uniform target constructor\n";
      }

      do_nfft_fn_ = &buffer_t::template do_nfft_impl<TolDigits>;
    }

    direct_backend_t pick_direct_backend(int n) const {
      if (!direct_dispatch_model_) throw_uninitialized_backend_error();
      return direct_dispatch_model_->pick(n);
    }

    bool use_type3_for_n(int n) const {
      if (finufft_switch_n_ <= 0) return !type3_for_small_n_;
      if (finufft_switch_n_ > state_.buf_size) return type3_for_small_n_;
      return type3_for_small_n_ ? (n < finufft_switch_n_) : (n >= finufft_switch_n_);
    }

    template <int TolDigits> static void do_nfft_impl(buffer_t &self) {
      if (self.type_ == type_t::type1) {
        auto &finufft = self.template get_finufft<TolDigits>();
        if (self.state_.buf_counter < self.direct_cutoff_)
          // Small uniform problems stay exact and avoid the coordinate transform entirely.
          self.run_direct_type1(*self.direct_type1_kernel_);
        else {
          if (self.direct_cutoff_ > 0) self.template prepare_type1_coords<12>();
          finufft.execute_type1(self.state_, self.fiw_arr, self.fk_arr, self.common_factor);
        }
      } else if (self.type_ == type_t::type1_gather) {
        auto &finufft = self.template get_finufft<TolDigits>();
        self.template prepare_type1_coords<TolDigits>();
        finufft.execute_type1_gather(self.state_, self.fiw_vec);
      } else if (self.type_ == type_t::automatic) {
        if (self.state_.buf_counter < self.direct_cutoff_) {
          switch (self.pick_direct_backend(self.state_.buf_counter)) {
            case direct_backend_t::direct_type1:
              self.run_direct(*self.direct_type1_kernel_);
              break;
            case direct_backend_t::direct_naf:
              self.template run_direct<TolDigits>(*self.naf_kernel_);
              break;
            case direct_backend_t::direct_chain:
              self.template run_direct<TolDigits>(*self.chain_kernel_);
              break;
          }
        } else {
          auto &finufft = self.template get_finufft<TolDigits>();
          if (self.use_type3_for_n(self.state_.buf_counter)) {
            finufft.execute_type3(self.state_, self.fiw_vec);
          } else {
            self.template prepare_type1_coords<TolDigits>();
            finufft.execute_type1_gather(self.state_, self.fiw_vec);
          }
        }
      } else if (self.type_ == type_t::type3) {
        auto &finufft = self.template get_finufft<TolDigits>();
        finufft.execute_type3(self.state_, self.fiw_vec);
      } else if (self.type_ == type_t::direct_type1)
        self.run_direct(*self.direct_type1_kernel_);
      else if (self.type_ == type_t::direct_type3)
        self.template run_direct<TolDigits>(*self.naf_kernel_);
      else if (self.type_ == type_t::direct_chain)
        self.template run_direct<TolDigits>(*self.chain_kernel_);
      else
        self.run_direct_type1(*self.direct_type1_kernel_);
    }

    void run_direct(kernel_direct_type1_t<Rank> &kernel) {
      state_.fk_vec = 0;
      kernel.template execute<12>(state_);
      fiw_vec += state_.fk_vec;
    }

    template <int TolDigits, typename Kernel> void run_direct(Kernel &kernel) {
      // Sparse direct kernels read full SIMD packets, so pad the inactive tail with zeros.
      int const buf_counter_padded = std::min(shared_state_t<Rank>::round_up_simd(state_.buf_counter), state_.buf_size);

      for (int j = state_.buf_counter; j < buf_counter_padded; ++j) {
        state_.fx_arr[j] = dcomplex{0.0, 0.0};
        for (int r = 0; r < Rank; ++r) state_.x_arr(r, j) = 0.0;
      }

      state_.fk_vec = 0;
      kernel.template execute<TolDigits>(state_);
      fiw_vec += state_.fk_vec;
    }

    // Direct type1 fills a flat buffer; uniform targets then scatter back to the rank-shaped array.
    void run_direct_type1(kernel_direct_type1_t<Rank> &kernel) {
      state_.fk_vec = 0;
      kernel.template execute<12>(state_);
      scatter_to_arr();
    }

    void scatter_to_arr() {
      if constexpr (Rank == 1) {
        fiw_arr += state_.fk_vec;
      } else {
        fiw_arr += nda::reshape(state_.fk_vec, fiw_arr.shape());
      }
    }

    template <int TolDigits> void prepare_type1_coords() { apply_type1_coord_transform<TolDigits>(state_, state_.buf_counter); }

    std::vector<std::array<target_mf_t, Rank>> build_uniform_target_mf(double beta) const {
      std::vector<std::array<target_mf_t, Rank>> target_mf;
      target_mf.reserve(state_.n_targets);

      // Use generic lambda + self-reference instead of deducing-this to work around GCC 15 ICE.
      auto recurse = [&](auto &self, std::array<target_mf_t, Rank> &mf, int r) -> void {
        if (r == Rank) {
          target_mf.push_back(mf);
          return;
        }
        // Uniform type1 targets are the dense fermionic box n_r in [-N_r/2, N_r/2).
        for (int64_t k = 0; k < niws[r]; ++k) {
          mf[r] = target_mf_t(static_cast<int>(k - niws[r] / 2), beta, mesh::Fermion);
          self(self, mf, r + 1);
        }
      };
      std::array<target_mf_t, Rank> mf{};
      recurse(recurse, mf, 0);
      return target_mf;
    }
  };

  template <int Rank> buffer_t(nda::array_view<dcomplex, Rank>, int, double, double) -> buffer_t<Rank>;

  template <std::size_t N>
  buffer_t(nda::array_view<dcomplex, 1>, std::vector<std::array<target_mf_t, N>>, int, double, type_t) -> buffer_t<static_cast<int>(N)>;

  template <std::size_t N>
  buffer_t(nda::array_view<dcomplex, 1>, std::vector<std::array<target_mf_t, N>>, int, double) -> buffer_t<static_cast<int>(N)>;

  template <std::size_t N>
  buffer_t(nda::array_view<dcomplex, 1>, std::vector<std::array<target_mf_t, N>>, int) -> buffer_t<static_cast<int>(N)>;

  buffer_t(nda::array_view<dcomplex, 1>, std::vector<target_mf_t> const &, int, double, type_t) -> buffer_t<1>;

  buffer_t(nda::array_view<dcomplex, 1>, std::vector<target_mf_t> const &, int, double) -> buffer_t<1>;

  buffer_t(nda::array_view<dcomplex, 1>, std::vector<target_mf_t> const &, int) -> buffer_t<1>;

} // namespace triqs::utility::nfft
