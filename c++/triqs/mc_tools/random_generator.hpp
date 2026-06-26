// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2020 Simons Foundation
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You may obtain a copy of the License at
//     https://www.gnu.org/licenses/gpl-3.0.txt
//
// Authors: Michel Ferrero, Olivier Parcollet, Nils Wentzell

/**
 * @file
 * @brief Provides a type erased random number generator based on 64-bit standard library engines.
 */

#pragma once

#include <triqs/utility/macros.hpp>
#include <h5/h5.hpp>
#include <mpi/mpi.hpp>

#include <cassert>
#include <concepts>
#include <cstdint>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <span>
#include <stdexcept>
#include <string>
#include <sstream>
#include <thread>
#include <utility>
#include <vector>

namespace triqs::mc_tools {

  /**
   * @addtogroup triqs-mc-utils
   * @{
   */

  /**
   * @brief Seed sequence deriving well-mixed engine states from a seed and a hierarchical spawn key.
   *
   * @details Follows the same design principles as numpy's `SeedSequence` -- hierarchical spawn
   * keys plus avalanche mixing of the seed and each key level -- but with a different mixer (the
   * splitmix64 finalizer, a strong 64-bit avalanche function) and a single 64-bit internal state
   * rather than numpy's entropy pool. It is therefore *not* stream-compatible with numpy: it does
   * not reproduce numpy's streams.
   *
   * The seed and each spawn-key level are chained through the finalizer, and the words filling the
   * engine state are derived counter-style, re-applying the finalizer per word. Distinct spawn keys,
   * e.g. `{mpi_rank, thread_id}`, thus yield decorrelated engine states for any engine, without
   * coordination between the parallel streams. Because the whole `(seed, spawn_key)` pair is
   * compressed into a single 64-bit state, the effective seeding entropy is at most 64 bits,
   * independent of the engine's state size.
   *
   * Satisfies the part of the standard *SeedSequence* requirements used by the engine
   * constructors (in particular `generate()`).
   */
  class C2PY_IGNORE splitmix_seed_seq {
    public:
    /// 32-bit output words, as required for standard library engine seeding.
    using result_type = std::uint32_t;

    /**
     * @brief Construct from a seed and a spawn key.
     * @param seed Base seed shared by all streams of one simulation.
     * @param spawn_key Hierarchical stream identifier, e.g. `{mpi_rank, thread_id}`.
     */
    splitmix_seed_seq(std::uint64_t seed, std::span<std::uint64_t const> spawn_key = {}) : state_(mix(seed + gamma)) {
      for (auto k : spawn_key) state_ = mix(state_ ^ k) + state_; // fold: keeps the chain from being a plain sequence of invertible mixes
    }

    /// Construct from a seed and a spawn key given as an initializer list.
    splitmix_seed_seq(std::uint64_t seed, std::initializer_list<std::uint64_t> spawn_key)
       : splitmix_seed_seq(seed, std::span{spawn_key.begin(), spawn_key.size()}) {}

    /// Fill the given range with 32-bit words derived from the internal state.
    template <typename It> void generate(It first, It last) const {
      std::uint64_t n = 0, acc = state_;
      for (auto it = first; it != last;) {
        // counter-style derivation; the XOR injection (instead of advancing the state additively)
        // prevents different streams from producing shifted copies of the same word sequence. The
        // running accumulator (fold) chains the words so they are not independent mixes of a counter.
        acc   = mix(state_ ^ ++n * gamma) + acc;
        *it++ = static_cast<result_type>(acc);
        if (it != last) *it++ = static_cast<result_type>(acc >> 32);
      }
    }

    private:
    // golden ratio increment of splitmix64
    static constexpr std::uint64_t gamma = 0x9E3779B97F4A7C15;

    // splitmix64 finalizer (Stafford mix 13)
    static constexpr std::uint64_t mix(std::uint64_t z) {
      z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9;
      z = (z ^ (z >> 27)) * 0x94D049BB133111EB;
      return z ^ (z >> 31);
    }

    std::uint64_t state_;
  };

  /**
   * @brief Random number generator with a selectable underlying engine.
   *
   * @details All supported engines produce 64-bit unsigned integers internally via the standard library.
   * Engines with native output smaller than 64 bits are wrapped using `std::independent_bits_engine`.
   *
   * The following engines are supported (see also triqs::mc_tools::random_generator_names_list()):
   *
   * - *empty string* or *mt19937_64*: uses `std::mt19937_64` (default, native 64-bit)
   * - *mt19937*: uses `std::mt19937` (32-bit, combined to 64-bit)
   * - *ranlux48*: uses `std::ranlux48` (48-bit, combined to 64-bit)
   * - *ranlux24*: uses `std::ranlux24` (24-bit, combined to 64-bit)
   * - *minstd_rand*: uses `std::minstd_rand` (31-bit, combined to 64-bit)
   * - *knuth_b*: uses `std::knuth_b` (31-bit, combined to 64-bit)
   *
   * For performance, raw `uint64_t` values are generated in batches and stored in a buffer.
   * Doubles in [0, 1) are derived using the standard 53-bit technique.
   * Integers in [0, i) are generated using Lemire's nearly divisionless method (unbiased for all ranges).
   *
   * All engines have their full state initialized through triqs::mc_tools::splitmix_seed_seq from the
   * seed and the MPI rank. The only public seeding constructor takes an `mpi::communicator`: every
   * rank passes the same seed and the rank is folded in as the spawn key `{rank}`, so the Markov
   * chains across ranks are decorrelated. When `c.size() > 1` the constructor collectively checks
   * (via `all_reduce`) that all ranks passed the same seed and throws otherwise.
   *
   * @note A random_generator is not thread-safe and exposes no per-thread stream. All instances must
   * live on one thread: constructing a random_generator on a thread other than the one that built the
   * first instance throws `std::runtime_error` (so instantiating one RNG per worker thread, or inside
   * a parallel region, is rejected -- it would otherwise yield identical or racy streams). In addition,
   * drawing from a generator on a thread other than the one that created it triggers an assertion in
   * debug builds (compiled out when `NDEBUG` is set). Per-thread streams, when needed, must be seeded
   * directly via `splitmix_seed_seq{seed, {rank, thread_id}}` (the internal seed sequence still
   * supports multi-level spawn keys).
   */
  class random_generator {
    private:
    // RNG concept defines the interface for RNGs producing uint64_t.
    struct rng_concept {
      virtual ~rng_concept()                                 = default;
      virtual std::uint64_t operator()()                     = 0;
      virtual void refill(std::vector<std::uint64_t> &)      = 0;
      virtual std::ostream &to_ostream(std::ostream &) const = 0;
      virtual std::istream &from_istream(std::istream &)     = 0;
    };

    // RNG model wraps a concrete engine that produces uint64_t.
    template <typename T> struct rng_model : public rng_concept {
      T engine_;
      rng_model(T engine) : engine_{std::move(engine)} {}
      std::uint64_t operator()() override { return engine_(); }
      void refill(std::vector<std::uint64_t> &buffer) override {
        for (auto &x : buffer) x = engine_();
      }
      std::ostream &to_ostream(std::ostream &os) const override {
        os << engine_;
        return os;
      }
      std::istream &from_istream(std::istream &is) override {
        is >> engine_;
        return is;
      }
    };

    public:
    /**
     * @brief Construct a random generator with an independent stream for each MPI rank.
     *
     * @details The given name has to correspond to one of the supported engines. If the name does not
     * match any of the supported engines, a runtime error is raised. An empty name selects the default
     * engine `std::mt19937_64`.
     *
     * All ranks of `c` must pass the same seed; the rank is folded in as the spawn key `{rank}`
     * (see triqs::mc_tools::splitmix_seed_seq), so the ranks obtain decorrelated streams. When
     * `c.size() > 1` this is a collective call: all ranks construct together and the constructor
     * checks (via `all_reduce`) that the seed matches across ranks, throwing `std::runtime_error`
     * otherwise.
     *
     * @param name Name of the RNG to be used.
     * @param seed Seed shared by all ranks.
     * @param c MPI communicator whose rank identifies the stream (defaults to the world communicator).
     */
    random_generator(std::string name, std::uint64_t seed, mpi::communicator c = {});

    /// Deleted copy constructor.
    random_generator(random_generator const &) = delete;

    /// Default move constructor.
    random_generator(random_generator &&) = default;

    /// Default move assignment operator.
    random_generator &operator=(random_generator &&) = default;

    /// Get the name of the underlying RNG.
    [[nodiscard]] std::string name() const { return name_; }

    /**
     * @brief Generate a random sample from the uniform integer distribution defined on \f$ \{0, ..., i-1 \}\f$.
     *
     * @details Uses Lemire's nearly divisionless method for unbiased generation across all ranges,
     * including very large ranges up to UINT64_MAX.
     *
     * @tparam T Integral type.
     * @param i Upper bound (excluded).
     * @return Uniform random integer in [0, i).
     */
    template <typename T>
      requires(std::integral<T>)
    T operator()(T i) {
      if (i <= 1) return 0;
      auto range = static_cast<std::uint64_t>(i);
      // Lemire's nearly divisionless method
      __uint128_t m = __uint128_t(raw_uint64()) * range;
      if (auto l = static_cast<std::uint64_t>(m); l < range) {
        auto t = -range % range; // rejection threshold: 2^64 mod range
        while (l < t) {
          m = __uint128_t(raw_uint64()) * range;
          l = static_cast<std::uint64_t>(m);
        }
      }
      return static_cast<T>(m >> 64);
    }

    /**
     * @brief Look ahead at the next value that will be generated when the generator is called.
     * @return Uniform random double from the interval \f$ [0, 1) \f$.
     */
    [[nodiscard]] double preview() {
      ensure_available();
      return to_double(buffer_[idx_]);
    }

    /**
     * @brief Generate a random sample from the uniform distribution defined on the interval \f$ [0, 1) \f$.
     *
     * @details Uses the standard 53-bit technique: the upper 53 bits of a 64-bit integer are scaled
     * to produce a double with full mantissa precision.
     *
     * @return Uniform random double from the interval \f$ [0, 1) \f$.
     */
    double operator()() { return to_double(raw_uint64()); }

    /**
     * @brief Generate a random sample from the uniform distribution defined on the interval \f$ [0, b) \f$.
     * @param b Upper bound of the interval.
     * @return Uniform random double from the interval \f$ [0, b) \f$.
     */
    double operator()(double b) { return b * (this->operator()()); }

    /**
     * @brief Generate a random sample from the uniform distribution defined on the interval \f$ [a, b) \f$.
     *
     * @param a Lower bound of the interval.
     * @param b Upper bound of the interval.
     * @return Uniform random double from the interval \f$ [a, b) \f$.
     */
    double operator()(double a, double b) {
      assert(b > a);
      return a + (b - a) * (this->operator()());
    }

    /// Get the HDF5 format tag. Bumped to "_v2" with the switch to 64-bit std engines: the on-disk
    /// layout (uint64 buffer, std engine state) is incompatible with archives written before that.
    [[nodiscard]] static std::string hdf5_format() { return "random_generator_v2"; }

    /**
     * @brief Write the RNG object to HDF5.
     *
     * @param g `h5::group` to be written to.
     * @param name Name of the dataset/subgroup.
     * @param rng RNG object to be written.
     */
    friend void h5_write(h5::group g, std::string const &name, random_generator const &rng) {
      auto gr = g.create_group(name);
      h5::write_hdf5_format(gr, rng); // NOLINT (downcasting to base class)
      h5::write(gr, "name", rng.name_);
      h5::write(gr, "buffer", rng.buffer_);
      h5::write(gr, "idx", rng.idx_);
      std::ostringstream os;
      rng.ptr_->to_ostream(os);
      h5::write(gr, "rng", os.str());
    }

    /**
     * @brief Read the RNG object from HDF5.
     *
     * @param g `h5::group` to be read from.
     * @param name Name of the dataset/subgroup.
     * @param rng RNG object to be read into.
     */
    friend void h5_read(h5::group g, std::string const &name, random_generator &rng) {
      auto gr = g.open_group(name);
      // Reject archives written by the pre-64-bit-engine version: their uint64 buffer / engine-state
      // layout and Boost engine names are incompatible and cannot be migrated.
      if (auto stored = h5::read_hdf5_format(gr); stored == "random_generator") // NOLINT (slicing to base class)
        throw std::runtime_error("Error in h5_read for random_generator: archive uses the legacy format '" + stored
                                 + "', which predates the switch to 64-bit standard-library engines and cannot be read.");
      h5::assert_hdf5_format(gr, rng); // NOLINT (downcasting to base class)
      h5::read(gr, "name", rng.name_);
      h5::read(gr, "buffer", rng.buffer_);
      h5::read(gr, "idx", rng.idx_);
      rng.initialize_rng(rng.name_, 0, {}); // seed is irrelevant: the engine state is overwritten from the stored stream below
      std::string rng_state;
      h5::read(gr, "rng", rng_state);
      std::istringstream is{rng_state};
      rng.ptr_->from_istream(is);
    }

    /// HDF5 construction hook. random_generator is intentionally not default-constructible (a seed is
    /// always required), so h5::h5_read uses this instead. The seed/communicator passed here are
    /// placeholders: the engine state is immediately overwritten from the stored stream by h5_read.
    static random_generator h5_read_construct(h5::group g, std::string const &key) {
      random_generator rng{"mt19937_64", 0, mpi::communicator{}};
      h5_read(g, key, rng);
      return rng;
    }

    private:
    // Convert a raw uint64_t to a double in [0, 1) using the 53-bit technique.
    static double to_double(std::uint64_t x) { return (x >> 11) * 0x1.0p-53; }

    // Ensure idx_ points at an unused buffer value (refilling if exhausted) and that we are on the
    // owning thread. Precondition for reading buffer_[idx_].
    void ensure_available() {
      check_thread();
      if (idx_ >= buffer_.size()) refill();
    }

    // Get the next raw uint64_t from the buffer.
    std::uint64_t raw_uint64() {
      ensure_available();
      return buffer_[idx_++];
    }

    // A random_generator is not thread-safe: it must be used by a single thread for its whole
    // lifetime (use one generator per thread for parallel streams). owner_thread_ records the thread
    // that created it (preserved across moves) and check_thread() asserts every draw happens on it.
    // The check is compiled out when NDEBUG is defined, so it has no cost in release builds and
    // catches accidental cross-thread sharing in debug/sanitizer builds.
    void check_thread() const {
#ifndef NDEBUG
      assert(std::this_thread::get_id() == owner_thread_
             && "random_generator drawn from a thread other than the one that created it: it is not "
                "thread-safe; construct one generator per thread");
#endif
    }

    // Refill the buffer.
    void refill() {
      ptr_->refill(buffer_);
      idx_ = 0;
    }

    // Initialize the RNG.
    void initialize_rng(std::string const &name, std::uint64_t seed, std::span<std::uint64_t const> spawn_key);

    private:
    // raw uint64_t values are generated in batches of this size; an internal performance detail.
    static constexpr std::size_t buffer_size = 1000;

    std::unique_ptr<rng_concept> ptr_;
    size_t idx_{0};
    std::vector<std::uint64_t> buffer_;
    std::string name_;
    std::thread::id owner_thread_ = std::this_thread::get_id();
  };

  /**
   * @brief Get a string containing the names of all available RNGs.
   * @param sep Separator between the names.
   * @return String containing the available RNGs separated by the given separator.
   */
  [[nodiscard]] std::string random_generator_names(std::string const &sep = " ");

  /// Get a list of all available RNG names.
  [[nodiscard]] std::vector<std::string> random_generator_names_list();

  /** @} */

} // namespace triqs::mc_tools
