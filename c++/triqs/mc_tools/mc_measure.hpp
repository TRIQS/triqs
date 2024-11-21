// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2022 Simons Foundation
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

#include "./concepts.hpp"
#include "../utility/timer.hpp"

#include <h5/h5.hpp>
#include <mpi/communicator.hpp>

#include <complex>
#include <concepts>
#include <cstdint>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace triqs::mc_tools {

  // Forward declaration.
  template <DoubleOrComplex MCSignType> class measure_set;

  /**
   * @brief Type erasure class for MC measures.
   *
   * @details It takes any type that models the triqs::mc_tools::MCMeasure concept and erases its type.
   *
   * Any MC measure must define the following methods:
   * - `void accumulate(MCSignType)`: Performs a measurement on the current MC configuration given the sign of its
   * weight.
   * - `void collect_results(mpi::communicator const &)`: Collects results from multiple MPI processes.
   *
   * Optionally, the following method can be defined:
   * - `std::string report() const`: Reports information about the measure in a `std::string` object.
   *
   * Optionally, the following free functions can be defined:
   * - `void h5_write(h5::group, std::string const &, T const &) const`: Writes the measure object of type `T` to HDF5.
   * - `void h5_read(h5::group, std::string const &, T &)`: Reads the measure object of type `T` from HDF5.
   *
   * @tparam MCSignType triqs::mc_tools::DoubleOrComplex type of the sign/weight of a MC configuration.
   */
  template <DoubleOrComplex MCSignType> class measure {
    private:
    // MC measure concept defines the interface for MC measures.
    struct measure_concept {
      virtual ~measure_concept()                                 = default;
      virtual void accumulate(MCSignType)                        = 0;
      virtual void collect_results(mpi::communicator const &)    = 0;
      [[nodiscard]] virtual std::string report() const           = 0;
      virtual void h5write(h5::group, std::string const &) const = 0;
      virtual void h5read(h5::group, std::string const &)        = 0;

      // // Methods only implemented in measure_set.
      [[nodiscard]] virtual std::string ms_get_timings(std::string const &) const = 0;
      [[nodiscard]] virtual std::vector<std::string> ms_names() const             = 0;
    };

    // MC measure model implements the MC measure concept by calling the appropriate methods of the type erased object.
    template <typename T>
      requires MCMeasure<T, MCSignType>
    struct measure_model : public measure_concept {
      static constexpr bool is_measure_set = std::is_same_v<T, measure_set<MCSignType>>;
      T measure_;
      measure_model(T m) : measure_{std::move(m)} {}
      void accumulate(MCSignType sign) override { return measure_.accumulate(sign); }
      void collect_results(mpi::communicator const &c) override { measure_.collect_results(c); }
      [[nodiscard]] std::string report() const override {
        if constexpr (requires {
                        { measure_.report() } -> std::convertible_to<std::string>;
                      })
          return measure_.report();
        return {};
      }
      void h5write(h5::group g, std::string const &name) const override {
        if constexpr (h5::Storable<T>) h5::write(g, name, measure_);
      }
      void h5read(h5::group g, std::string const &name) override {
        if constexpr (h5::Storable<T>) h5::read(g, name, measure_);
      }

      // Methods only implemented in measure_set.
      [[nodiscard]] std::string ms_get_timings(std::string const &prefix) const override {
        if constexpr (is_measure_set) return measure_.get_timings(prefix);
        return {};
      }
      [[nodiscard]] std::vector<std::string> ms_names() const override {
        if constexpr (is_measure_set) return measure_.names();
        return {};
      }
    };

    public:
    /**
     * @brief Constructor takes an object that models the triqs::mc_tools::MCMeasure concept and erases its type.
     *
     * @tparam T Original type of the MC measure object.
     * @param m MC measure object to have its type erased.
     * @param enable_timer Enable timer for measuring the duration of the accumulate() and collect_results() calls.
     * @param enable_report Enable the report() callback method.
     */
    template <typename T>
      requires(MCMeasure<T, MCSignType> && !std::is_same_v<T, measure>)
    measure(T m, bool enable_timer, bool enable_report)
       : ptr_{std::make_unique<measure_model<T>>(std::move(m))},
         enable_timer_(enable_timer),
         enable_report_(enable_report),
         is_measure_set_{std::is_same_v<T, measure_set<MCSignType>>} {}

    /// Deleted copy constructor.
    measure(measure const &) = delete;

    /// Deleted copy assignment operator.
    measure &operator=(measure const &) = delete;

    /// Default move constructor leaves the moved from object in an empty state.
    measure(measure &&) = default;

    /// Default move assignment operator leaves the moved from object in an empty state.
    measure &operator=(measure &&) = default;

    /**
     * @brief Perform the measurement on the current MC configuration.
     * @details It calls the `accumulate` method of the original measure object.
     * @param sign Sign of the current MC configuration.
     */
    void accumulate(MCSignType sign) {
      ++count_;
      if (enable_timer_) timer_.start();
      ptr_->accumulate(sign);
      if (enable_timer_) timer_.stop();
    }

    /**
     * @brief Collect results from multiple MPI processes.
     * @details It calls the `collect_results` method of the original measure object.
     * @param c MPI communicator.
     */
    void collect_results(mpi::communicator const &c);

    /**
     * @brief Report information about the measure.
     * @details It calls the `report` method of the original measure object (if reports are enabled in the ctor).
     * @return If the original measure object has a `report` method, it returns the resulting report string, otherwise
     * an empty string.
     */
    [[nodiscard]] std::string report() const;

    /**
     * @brief Get a formatted string showing the runtime of the accumulate() and collect_results() calls.
     *
     * @param name Name of the measure.
     * @param prefix Prefix string to be added to the beginning of each line.
     * @return String containing the durations of those calls (if the timer is disabled, the results might make no
     * sense).
     */
    [[nodiscard]] std::string get_timings(std::string const &name, std::string const &prefix = "") const;

    /**
     * @brief Get a vector of all the measure names in a measure set.
     * @return `std::vector<std::string>` containing the names of all registered measures if the measure is a measure
     * set. Otherwise, an empty vector.
     */
    [[nodiscard]] std::vector<std::string> names() const;

    /// Get the duration of the cumulative accumulate() and collect_results() calls.
    [[nodiscard]] double duration() const;

    /// Get the number of measurements performed.
    [[nodiscard]] auto count() const { return count_; }

    /// Is the measure object a measure set?
    [[nodiscard]] auto is_set() const { return is_measure_set_; }

    /**
     * @brief Write the measure object to HDF5.
     *
     * @details Does nothing if there is no specialized `h5_write` function for the original measure object.
     *
     * @param g h5::group to be written to.
     * @param name Name of the dataset/subgroup.
     * @param m Measure object to be written.
     */
    friend void h5_write(h5::group g, std::string const &name, measure const &m) { m.ptr_->h5write(g, name); }

    /**
     * @brief Read the measure object from HDF5.
     *
     * @details Does nothing if there is no specialized `h5_read` function for the original measure object.
     *
     * @param g h5::group to be read from.
     * @param name Name of the dataset/subgroup.
     * @param m Measure object to be read into.
     */
    friend void h5_read(h5::group g, std::string const &name, measure &m) { m.ptr_->h5read(g, name); }

    private:
    std::unique_ptr<measure_concept> ptr_;
    std::uint64_t count_{0};
    triqs::utility::timer timer_;
    bool enable_timer_{false};
    bool enable_report_{false};
    bool is_measure_set_{false};
  };

  // Explicit template instantiation declarations.
  extern template class measure<double>;
  extern template class measure<std::complex<double>>;

} // namespace triqs::mc_tools
