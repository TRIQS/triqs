// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2023 Simons Foundation
// Copyright (c) 2017 Hugo U.R. Strand
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
// Authors: Michel Ferrero, Henri Menke, Olivier Parcollet, Priyanka Seth, Hugo U. R. Strand, Nils Wentzell, Thomas Ayral

#pragma once

#include "./concepts.hpp"
#include "./mc_measure_aux_set.hpp"
#include "./mc_measure_set.hpp"
#include "./mc_move_set.hpp"
#include "./random_generator.hpp"
#include "../utility/report_stream.hpp"
#include "../utility/timer.hpp"
#include "../utility/timestamp.hpp"

#include <h5/h5.hpp>

#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace triqs::mc_tools {

  /**
   * @brief Generic Monte Carlo class.
   *
   * @details This class provides a generic Monte Carlo simulation framework. It allows to register MC moves and
   * measures that fulfill the triqs::mc_tools::MCMove and triqs::mc_tools::MCMeasure concepts, respectively.
   * Additionally, one can register auxiliary measures that are callable objects (see triqs::mc_tools::measure_aux).
   *
   * Most MC simulations consist of two phases:
   * - warmup phase: No measurements are done during this phase. It is used to equilibrate the underlying Markov chain
   * and to optionally calibrate the MC moves.
   * - accumulation phase: This phase is used to accumulate measurements. The Markov chain is assumed to be already
   * warmed up such that it generates MC configurations distrubuted according to the desired probability distribution.
   *
   * Both phases follow the same overall procedure:
   * ```
   * while not stop_flag do
   *   for i in 1..cycle_length do
   *     perform a Metropolis accept/reject step to update MC configuration;
   *   end
   *   perform after-cycle duties, e.g. measurements, calibration, etc.;
   *   update stop_flag;
   * end
   * ```
   *
   * As you can see, a cycle consists of `cycle_length` MC steps each of which does a single Metropolis accept/reject
   * step. The after-cycle duties depend on the phase of the simulation and on input parameters. For example, in the
   * accumulation phase, measurements are performed after each cycle, while in the warmup phase, moves might be
   * calibrated to optimize the sampling procedure.
   *
   * The simulation stops when one of the following conditions is met:
   * - The number of requested cycles is done (if the specified number is < 1, the simulation runs indefinitely).
   * - A user provided callback function returns true.
   * - A signal is caught by the triqs::utility::signal_handler.
   * - An exception is caught.
   *
   * @tparam MCSignType triqs::mc_tools::DoubleOrComplex type of the sign/weight of a MC configuration.
   */
  template <DoubleOrComplex MCSignType> class mc_generic {
    public:
    /**
     * @brief Construct a generic Monte Carlo class.
     *
     * @param rng_name Name of the RNG to be used (see triqs::mc_tools::random_generator).
     * @param rng_seed Seed for the RNG.
     * @param verbosity_lvl Verbosity level (see triqs::utility::report_stream).
     * @param propagate_exception Should we propagate an exception to all MPI ranks or abort immediately?
     */
    mc_generic(const std::string &rng_name, int rng_seed, int verbosity_lvl, bool propagate_exception = true)
       : rng_(rng_name, rng_seed),
         moves_(rng_),
         report_(&std::cout, verbosity_lvl),
         propagate_exception_(propagate_exception) {}

    /**
     * @brief Register a new MC move.
     *
     * @tparam T triqs::mc_tools::MCMove type.
     * @param m MC move to register.
     * @param name Name of the move.
     * @param weight Weight of the move which is proportional to its proposal probability (>= 0).
     */
    template <typename T> void add_move(T &&m, std::string name, double weight = 1.0) { moves_.add(std::forward<T>(m), name, weight); }

    /**
     * @brief Register a new MC measure.
     *
     * @tparam T triqs::mc_tools::MCMeasure type.
     * @param m MC measure to register.
     * @param name Name of the measure.
     * @param enable_timer Enable the timer in the measure::accumulate and measure::collect_results methods.
     * @param enable_report Enable the measure::report method.
     * @return Iterator to the registered measure.
     */
    template <typename T> auto add_measure(T &&m, std::string name, bool enable_timer = true, bool enable_report = false) {
      return measures_.insert(std::forward<T>(m), name, enable_timer, enable_report);
    }

    /**
     * @brief Register a new auxiliary MC measure.
     *
     * @tparam T Type of the auxiliary measure.
     * @param m_ptr Shared pointer to the auxiliary MC measure to register.
     */
    template <typename T> void add_measure_aux(std::shared_ptr<T> const &m_ptr) { measures_aux_.emplace_back(m_ptr); }

    /**
     * @brief Remove a registered MC measure.
     * @param it Iterator to the measure to remove.
     */
    void rm_measure(typename measure_set<MCSignType>::measure_itr_t const &it) { measures_.remove(it); }

    /// Remove all registered measures.
    void clear_measures() { measures_.clear(); }

    /**
     * @brief Set the callback function to be called after each cycle.
     * @param f Callback function.
     */
    void set_after_cycle_duty(std::function<void()> f) { after_cycle_duty_ = f; }

    /**
     * @brief Run a generic MC simulation.
     *
     * @param ncycles Number of MC cycles to run (< 1 to run indefinitely).
     * @param cycle_length Number of MC steps per cycle (> 0).
     * @param stop_callback Callback function to check if the simulation should be stopped (returns true to stop).
     * @param enable_measures Enable measurements at the end of each cycle (false during warmup phase).
     * @param c MPI communicator.
     * @param enable_calibration Enable calibration of the moves after each cycle (false during accumulation phase).
     * @return 0 if the simulation has done all requested cycles, 1 if it has been stopped due to `stop_callback()`
     * returned true, 2 if it has been stopped due to a signal.
     */
    int run(std::int64_t ncycles, std::int64_t cycle_length, std::function<bool()> stop_callback, bool enable_measures,
            mpi::communicator c = mpi::communicator{}, bool enable_calibration = false);

    /**
     * @brief Run the warumup phase of the MC simulation.
     *
     * @param ncycles Number of warumup cycles to run (< 1 to run indefinitely).
     * @param cycle_length Number of MC steps per cycle (> 0).
     * @param stop_callback Callback function to check if the simulation should be stopped (returns true to stop).
     * @param initial_sign Sign of the initial MC configuration.
     * @param c MPI communicator.
     * @return 0 if the simulation has done all requested cycles, 1 if it has been stopped due to `stop_callback()`
     * returned true, 2 if it has been stopped due to a signal.
     */
    int warmup(std::int64_t ncycles, std::int64_t cycle_length, std::function<bool()> stop_callback, MCSignType initial_sign,
               mpi::communicator c = mpi::communicator{});

    /// The same as warmup(std::int64_t, std::int64_t, std::function<bool()>, MCSignType, mpi::communicator) but without
    /// specifying the initial sign.
    int warmup(std::int64_t ncycles, std::int64_t cycle_length, std::function<bool()> stop_callback, mpi::communicator c = mpi::communicator{});

    /**
     * @brief Run the accumulation phase of the MC simulation.
     *
     * @param ncycles Number of accumulation cycles to run (< 1 to run indefinitely).
     * @param cycle_length Number of MC steps per cycle (> 0).
     * @param stop_callback Callback function to check if the simulation should be stopped (returns true to stop).
     * @param initial_sign Sign of the initial MC configuration.
     * @param c MPI communicator.
     * @return 0 if the simulation has done all requested cycles, 1 if it has been stopped due to `stop_callback()`
     * returned true, 2 if it has been stopped due to a signal.
     */
    int accumulate(std::int64_t ncycles, std::int64_t cycle_length, std::function<bool()> stop_callback, mpi::communicator c = mpi::communicator{});

    /**
     * @brief Run the warumup and accumulation phases of the MC simulation.
     *
     * @param ncycles_warmup Number of warumup cycles to run (< 1 to run indefinitely).
     * @param ncycles_acc Number of accumulation cycles to run (< 1 to run indefinitely).
     * @param cycle_length Number of MC steps per cycle (> 0).
     * @param stop_callback Callback function to check if the simulation should be stopped (returns true to stop).
     * @param initial_sign Sign of the initial MC configuration.
     * @param c MPI communicator.
     * @return 0 if the simulation has done all requested cycles, 1 if it has been stopped due to `stop_callback()`
     * returned true, 2 if it has been stopped due to a signal.
     */
    int warmup_and_accumulate(std::int64_t ncycles_warmup, std::int64_t ncycles_acc, std::int64_t cycle_length, std::function<bool()> stop_callback,
                              MCSignType initial_sign, mpi::communicator c = mpi::communicator{});

    /// The same as warmup_and_accumulate(std::int64_t, std::int64_t, std::int64_t, std::function<bool()>, MCSignType,
    /// mpi::communicator) but without specifying the initial sign.
    int warmup_and_accumulate(std::int64_t ncycles_warmup, std::int64_t ncycles_acc, std::int64_t cycle_length, std::function<bool()> stop_callback,
                              mpi::communicator c = mpi::communicator{});

    /**
     * @brief Collect results from multiple MPI processes.
     * @param c MPI communicator.
     */
    void collect_results(mpi::communicator const &c);

    /// Get the acceptance rates of all MC moves (see move_set::get_acceptance_rates).
    [[nodiscard]] std::map<std::string, double> get_acceptance_rates() const { return moves_.get_acceptance_rates(); }

    /// Get the percentage of the requested number of cycles done.
    [[nodiscard]] double get_percent() const { return percentage_done_; }

    /// Get a reference to the random number generator.
    random_generator &get_rng() { return rng_; }

    /// Get the number of cycles done.
    [[nodiscard]] int get_current_cycle_number() const { return ncycles_done_; }

    /// Get the ID of the current MC configuration.
    [[nodiscard]] int get_config_id() const { return config_id_; }

    /// Get the total time, i.e. the sum of the warmup and accumulation times, in seconds.
    [[nodiscard]] double get_duration() const { return get_total_time(); }

    /// Get the number of measurements done.
    [[nodiscard]] int get_nmeasures() const { return nmeasures_done_; }

    /// Get the total time, i.e. the sum of the warmup and accumulation times, in seconds.
    [[nodiscard]] double get_total_time() const { return get_warmup_time() + get_accumulation_time(); }

    /// Get the time spent in the warmup phase in seconds.
    [[nodiscard]] double get_warmup_time() const { return static_cast<double>(warmup_timer_); }

    /// Get the time spent in the warmup phase in hours, minutes, and seconds.
    [[nodiscard]] auto get_warmup_time_HHMMSS() const { return hours_minutes_seconds_from_seconds(warmup_timer_); }

    /// Get the time spent in the accumulation phase in seconds.
    [[nodiscard]] double get_accumulation_time() const { return static_cast<double>(acc_timer_); }

    /// Get the time spent in the accumulation phase in hours, minutes and seconds.
    [[nodiscard]] std::string get_accumulation_time_HHMMSS() const { return hours_minutes_seconds_from_seconds(acc_timer_); }

    /**
     * @brief Write the MC simulation object to HDF5.
     *
     * @details It writes the registered moves and measures as well as the number of cycles and measures that have been
     * done and the sign of the current configuration.
     *
     * @param g h5::group to be written to.
     * @param name Name of the subgroup.
     * @param mc MC simulation object to be written.
     */
    friend void h5_write(h5::group g, std::string const &name, mc_generic const &mc) {
      auto gr = g.create_group(name);
      h5_write(gr, "moves", mc.moves_);
      h5_write(gr, "measures", mc.measures_);
      h5_write(gr, "number_cycle_done", mc.ncycles_done_);
      h5_write(gr, "number_measure_done", mc.nmeasures_done_);
      h5_write(gr, "sign", mc.sign_);
    }

    /**
     * @brief Read the MC simulation object from HDF5.
     *
     * @details It reads the registered moves and measures as well as the number of cycles and measures that have been
     * done and the sign of the last configuration.
     *
     * @param g h5::group to be read from.
     * @param name Name of the subgroup.
     * @param mc MC simulation object to be read into.
     */
    friend void h5_read(h5::group g, std::string const &name, mc_generic &mc) {
      auto gr = g.open_group(name);
      h5_read(gr, "moves", mc.moves_);
      h5_read(gr, "measures", mc.measures_);
      h5_read(gr, "number_cycle_done", mc.ncycles_done_);
      h5_read(gr, "number_measure_done", mc.nmeasures_done_);
      h5_read(gr, "sign", mc.sign_);
    }

    private:
    random_generator rng_;
    move_set<MCSignType> moves_;
    measure_set<MCSignType> measures_;
    std::vector<measure_aux> measures_aux_;
    utility::report_stream report_;
    utility::timer run_timer_;
    utility::timer acc_timer_;
    utility::timer warmup_timer_;
    MCSignType sign_{1};
    std::int64_t nmeasures_done_{0};
    std::int64_t ncycles_done_{0};
    std::int64_t percentage_done_{0};
    std::int64_t config_id_{0};
    std::function<void()> after_cycle_duty_{nullptr};
    bool propagate_exception_{true};
  };

  // Explicit template instantiation declarations.
  extern template class mc_generic<double>;
  extern template class mc_generic<std::complex<double>>;

} // namespace triqs::mc_tools
