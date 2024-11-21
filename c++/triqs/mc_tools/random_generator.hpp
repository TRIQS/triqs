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

#pragma once

#include <h5/h5.hpp>

#include <cassert>
#include <cmath>
#include <concepts>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <utility>
#include <vector>

namespace triqs::mc_tools {

  /**
   * @brief Wrapper that erases the type of a random number generator.
   *
   * @details Various RNGs from the Boost library are supported (see triqs::mc_tools::random_generator_names() or
   * triqs::mc_tools::random_generator_names_list()) as well as the triqs::mc_tools::RandomGenerators::RandMT. The RNG
   * is specified in the constructor by giving its name.
   *
   * For performance reasons, we use a triqs::utility::buffered_function to avoid some of the costs of repeated function
   * calls to the RNG.
   */
  class random_generator {
    private:
    // RNG concept defines the interface for RNGs.
    struct rng_concept {
      virtual ~rng_concept()                                 = default;
      virtual double operator()()                            = 0;
      virtual void refill(std::vector<double> &)             = 0;
      virtual std::ostream &to_ostream(std::ostream &) const = 0;
      virtual std::istream &from_istream(std::istream &)     = 0;
    };

    // RNG model implements the RNG concept by calling the appropriate methods of the type erased object.
    template <typename T> struct rng_model : public rng_concept {
      T rng_;
      rng_model(T rng) : rng_{std::move(rng)} {}
      double operator()() override { return rng_(); }
      void refill(std::vector<double> &buffer) override {
        for (auto &x : buffer) x = rng_();
      }
      std::ostream &to_ostream(std::ostream &os) const override {
        os << rng_.engine();
        return os;
      }
      std::istream &from_istream(std::istream &is) override {
        is >> rng_.engine();
        return is;
      }
    };

    public:
    /// Default seed for the underlying RNG.
    static constexpr std::uint32_t default_seed = 198;

    /// Default constructor uses Boost's Mersenne Twister 19937 RNG.
    random_generator() : random_generator("mt19937", default_seed) {}

    /**
     * @brief Construct a random generator by wrapping the specified RNG and seeding it with the given seed.
     *
     * @details The given name has to correspond to one of the supported RNGs (see
     * triqs::mc_tools::random_generator_names() or triqs::mc_tools::random_generator_names_list()). If the name does
     * not match any of the supported RNGs, a `std::runtime_error` is thrown.
     *
     * An empty name corresponds to the triqs::mc_tools::RandomGenerators::RandMT RNG.
     *
     * @param name Name of the RNG to be used.
     * @param seed Seed for the RNG.
     * @param buffer_size Size of the buffer used to store random numbers.
     */
    random_generator(std::string name, std::uint32_t seed, std::size_t buffer_size = 1000);

    /// Deleted copy constructor.
    random_generator(random_generator const &) = delete;

    /// Deleted copy assignment operator.
    random_generator(random_generator &&) = default;

    /// Default move assignment operator.
    random_generator &operator=(random_generator &&) = default;

    /// Get the name of the underlying RNG.
    [[nodiscard]] std::string name() const { return name_; }

    /**
     * @brief Generate a random sample from the uniform integer distribution defined on the set `{0, ..., i-1}`.
     *
     * @tparam T Integral type.
     * @param i Upper bound (excluded).
     * @return Uniform random integer.
     */
    template <typename T>
      requires std::integral<T>
    T operator()(T i) {
      return (i == 1 ? 0 : static_cast<T>(std::floor(i * this->operator()())));
    }

    /**
     * @brief Look ahead at the next value that will be generated with a call to operator()().
     * @return Uniform random double from the interval `[0, 1)`.
     */
    [[nodiscard]] double preview() {
      if (idx_ > buffer_.size() - 1) refill();
      return buffer_[idx_];
    }

    /**
     * @brief Generate a random sample from the uniform distribution defined on the interval `[0, 1)`.
     * @return Uniform random double from the interval `[0, 1)`.
     */
    double operator()() {
      if (idx_ > buffer_.size() - 1) refill();
      return buffer_[idx_++];
    }

    /**
     * @brief Generate a random sample from the uniform distribution defined on the interval `[0, b)`.
     * @param b Upper bound of the interval.
     * @return Uniform random double from the interval `[0, b)`.
     */
    double operator()(double b) { return b * (this->operator()()); }

    /**
     * @brief Generate a random sample from the uniform distribution defined on the interval `[a, b)`.
     *
     * @param a Lower bound of the interval.
     * @param b Upper bound of the interval.
     * @return Uniform random double from the interval `[a, b)`.
     */
    double operator()(double a, double b) {
      assert(b > a);
      return a + (b - a) * (this->operator()());
    }

    /// Get the HDF5 format tag.
    [[nodiscard]] static std::string hdf5_format() { return "random_generator"; }

    /**
     * @brief Write the RNG object to HDF5.
     *
     * @param g h5::group to be written to.
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
     * @param g h5::group to be read from.
     * @param name Name of the dataset/subgroup.
     * @param rng RNG object to be read into.
     */
    friend void h5_read(h5::group g, std::string const &name, random_generator &rng) {
      auto gr = g.open_group(name);
      h5::assert_hdf5_format(gr, rng);
      h5::read(gr, "name", rng.name_);
      h5::read(gr, "buffer", rng.buffer_);
      h5::read(gr, "idx", rng.idx_);
      rng.initialize_rng(rng.name_, default_seed);
      std::string rng_state;
      h5::read(gr, "rng", rng_state);
      std::istringstream is{rng_state};
      rng.ptr_->from_istream(is);
    }

    private:
    // Refill the buffer.
    void refill() {
      ptr_->refill(buffer_);
      idx_ = 0;
    }

    // Initialize the RNG.
    void initialize_rng(std::string const &name, std::uint32_t seed);

    private:
    std::unique_ptr<rng_concept> ptr_;
    size_t idx_{0};
    std::vector<double> buffer_;
    std::string name_;
  };

  /**
   * @brief Get a string containing the names of all available RNGs.
   * @param sep Separator between the names.
   * @return `std::string` containing the available RNGs separated by the given separator.
   */
  [[nodiscard]] std::string random_generator_names(std::string const &sep = " ");

  /// Get a `std::vector<std::string>` containing all available RNG names.
  [[nodiscard]] std::vector<std::string> random_generator_names_list();

} // namespace triqs::mc_tools
