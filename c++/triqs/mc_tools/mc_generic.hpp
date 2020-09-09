// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2017 Hugo Strand
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
// Authors: Michel Ferrero, Olivier Parcollet, Priyanka Seth, Hugo U.R. Strand, Nils Wentzell, tayral

#pragma once
#include <triqs/utility/first_include.hpp>
#include <cmath>
#include <triqs/utility/timer.hpp>
#include <triqs/utility/timestamp.hpp>
#include <triqs/utility/report_stream.hpp>
#include <triqs/utility/signal_handler.hpp>
#include <triqs/utility/macros.hpp>
#include <mpi/mpi.hpp>
#include "./mc_measure_aux_set.hpp"
#include "./mc_measure_set.hpp"
#include "./mc_move_set.hpp"
#include "./random_generator.hpp"

namespace triqs::mc_tools {

  /**
  * \brief Generic Monte Carlo class.
  *
  * TBR
  * @include triqs/mc_tools.hpp
  */
  template <typename MCSignType> class mc_generic {

#ifdef TRIQS_MCTOOLS_DEBUG
    static constexpr bool debug = true;
#else
    static constexpr bool debug = false;
#endif

    public:
    /**
    * Constructor
    *
    * @param random_name     Name of the random generator (cf doc).
    * @param random_seed     Seed for the random generator
    * @param verbosity       Verbosity level. 0 : None, ... TBA
    */
    mc_generic(std::string random_name, int random_seed, int verbosity)
       : RandomGenerator(random_name, random_seed), AllMoves(RandomGenerator), AllMeasures(), AllMeasuresAux(), report(&std::cout, verbosity) {}

    /**
   * Register a move
   *
   * If the move m is an rvalue, it is moved into the mc_generic, otherwise is copied into it.
   *
   * @tparam MoveType                Type of the move. Must model Move concept
   * @param m                        The move. Must model Move concept.
   * @param name                     Name of the move
   * @param proposition_probability  Probability that the move will be proposed. Precondition : >0
   *                                 NB it but does not need to be normalized.
   *                                 Normalization is automatically done with all the added moves before starting the run.
   */
    template <typename MoveType> void add_move(MoveType &&m, std::string name, double proposition_probability = 1.0) {
      static_assert(!std::is_pointer<MoveType>::value, "add_move in mc_generic takes ONLY values !");
      AllMoves.add(std::forward<MoveType>(m), name, proposition_probability);
    }

    /**
   * Register a measure
   *
   * If the measure m is an rvalue, it is moved into the mc_generic, otherwise is copied into it.
   *
   * @param M                        The measure. Must model Measure concept
   * @param name                     Name of the measure
   *
   */
    template <typename MeasureType>
    typename measure_set<MCSignType>::measure_ptr_t add_measure(MeasureType &&m, std::string name, bool enable_timer = true) {
      static_assert(!std::is_pointer<MeasureType>::value, "add_measure in mc_generic takes ONLY values !");
      return AllMeasures.insert(std::forward<MeasureType>(m), name, enable_timer);
    }

    /**
   * Register a common part for several measure [EXPERIMENTAL: API WILL CHANGE]
   */
    template <typename MeasureAuxType> void add_measure_aux(std::shared_ptr<MeasureAuxType> p) { AllMeasuresAux.emplace_back(p); }

    /**
   * Deregister a measure
   *
   * @param m      The measure. Must be the return value of add_measure
   *
   */
    void rm_measure(typename measure_set<MCSignType>::measure_ptr_t const &m) { AllMeasures.remove(m); }

    /**
   * Sets a function called after each cycle
   * @param f The function be called.
   */
    void set_after_cycle_duty(std::function<void()> f) { after_cycle_duty = f; }

    int warmup(uint64_t n_warmup_cycles, int64_t length_cycle, std::function<bool()> stop_callback) {
      report << "\nWarming up ..." << std::endl;
      return run(n_warmup_cycles, length_cycle, stop_callback, false);
    }

    /**
     * Warmup the Monte-Carlo configuration
     *
     * @param n_warmup_cycles         Number of QMC cycles in the warmup
     * @param length_cycle            Number of QMC move attempts in one cycle
     * @param stop_callback           A callback function () -> bool. It is called after each cycle
     *                                to and the computation stops when it returns true.
     *                                Typically used to set up the time limit, cf doc.
     * @param sign_init               The sign of the initial configuration's weight [optional]
     *
     * @return
     *    =  =============================================
     *    0  if the computation has run until the end
     *    1  if it has been stopped by stop_callback
     *    2  if it has been stopped by receiving a signal
     *    =  =============================================
     *
     */
    int warmup(uint64_t n_warmup_cycles, int64_t length_cycle, std::function<bool()> stop_callback, MCSignType sign_init) {
      sign = sign_init;
      return warmup(n_warmup_cycles, length_cycle, stop_callback);
    }

    /**
     * Accumulate/Measure
     *
     * @param n_accumulation_cycles   Number of QMC cycles in the accumulation (measures are done after each cycle).
     * @param length_cycle            Number of QMC move attempts in one cycle
     * @param stop_callback           A callback function () -> bool. It is called after each cycle
     *                                to and the computation stops when it returns true.
     *                                Typically used to set up the time limit, cf doc.
     *
     * @return
     *    =  =============================================
     *    0  if the computation has run until the end
     *    1  if it has been stopped by stop_callback
     *    2  if it has been stopped by receiving a signal
     *    =  =============================================
     *
     */
    int accumulate(uint64_t n_accumulation_cycles, int64_t length_cycle, std::function<bool()> stop_callback) {
      report << "\nAccumulating ..." << std::endl;
      return run(n_accumulation_cycles, length_cycle, stop_callback, true);
    }

    int warmup_and_accumulate(uint64_t n_warmup_cycles, uint64_t n_accumulation_cycles, uint64_t length_cycle, std::function<bool()> stop_callback) {
      int status = warmup(n_warmup_cycles, length_cycle, stop_callback);
      if (status == 0) status = accumulate(n_accumulation_cycles, length_cycle, stop_callback);
      return status;
    }

    /**
     * Warmup the Monte-Carlo configuration and accumulate/measure
     *
     * @param n_warmup_cycles         Number of QMC cycles in the warmup
     * @param n_accumulation_cycles   Number of QMC cycles in the accumulation (measures are done after each cycle).
     * @param length_cycle            Number of QMC move attempts in one cycle
     * @param stop_callback           A callback function () -> bool. It is called after each cycle
     *                                to and the computation stops when it returns true.
     *                                Typically used to set up the time limit, cf doc.
     * @param sign_init               The sign of the initial configuration's weight [optional]
     *
     * @return
     *    =  =============================================
     *    0  if the computation has run until the end
     *    1  if it has been stopped by stop_callback
     *    2  if it has been stopped by receiving a signal
     *    =  =============================================
     *
     */
    int warmup_and_accumulate(uint64_t n_warmup_cycles, uint64_t n_accumulation_cycles, uint64_t length_cycle, std::function<bool()> stop_callback,
                              MCSignType sign_init) {
      sign = sign_init; // init the sign
      return warmup_and_accumulate(n_warmup_cycles, n_accumulation_cycles, length_cycle, stop_callback);
    }

    private:
    // implementation

    int run(uint64_t n_cycles, uint64_t length_cycle, std::function<bool()> stop_callback, bool do_measure = true) {
      utility::timer timer;
      timer.start();
      if (n_cycles == 0) return 0;
      triqs::signal_handler::start();
      done_percent = 0;
      nmeasures    = 0;
      bool stop_it = false, finished = false;
      int NC                = 0;
      double next_info_time = 0.1;
      for (; !stop_it; ++NC) { // do NOT reinit NC to 0
        // Metropolis loop. Switch here for HeatBath, etc...
        for (uint64_t k = 1; (k <= length_cycle); k++) {
          if (triqs::signal_handler::received()) goto _final;
          double r = AllMoves.attempt();
          if (RandomGenerator() < std::min(1.0, r)) {
            if (debug) std::cerr << " Move accepted " << std::endl;
            sign *= AllMoves.accept();
            if (debug) std::cerr << " New sign = " << sign << std::endl;
          } else {
            if (debug) std::cerr << " Move rejected " << std::endl;
            AllMoves.reject();
          }
          ++config_id;
        }
        if (after_cycle_duty) { after_cycle_duty(); }
        if (do_measure) {
          nmeasures++;
          for (auto &x : AllMeasuresAux) x();
          AllMeasures.accumulate(sign);
        }
      // recompute fraction done
      _final:
        done_percent = uint64_t(floor(((NC + 1) * 100.0) / n_cycles));
        if (timer > next_info_time) {
          report << utility::timestamp() << " " << std::setfill(' ') << std::setw(3) << done_percent << "%"
                 << " ETA " << estimate_time_left(n_cycles, NC, timer) << " cycle " << NC << " of " << n_cycles << "\n"
                 << std::flush;
          next_info_time = 1.25 * timer + 2.0; // Increase time interval non-linearly
        }
        finished = NC + 1 >= n_cycles;
        stop_it  = (stop_callback() || triqs::signal_handler::received() || finished);
      }
      int status = (finished ? 0 : (triqs::signal_handler::received() ? 2 : 1));
      triqs::signal_handler::stop();
      current_cycle_number += NC;
      timer.stop();
      if (do_measure) {
        timer_accumulation = timer;
      } else {
        timer_warmup = timer;
      }

      // final reporting
      if (status == 1) report << "mc_generic stops because of stop_callback";
      if (status == 2) report << "mc_generic stops because of a signal";
      report << "\n" << std::endl;

      return status;
    }

    public:
    /// Reduce the results of the measures, and reports some statistics
    void collect_results(mpi::communicator const &c) {
      report(3) << "[Rank " << c.rank() << "] Collect results: Waiting for all mpi-threads to finish accumulating...\n";
      AllMeasures.collect_results(c);
      AllMoves.collect_statistics(c);
      uint64_t nmeasures_tot = mpi::reduce(nmeasures, c);

      report(3) << "[Rank " << c.rank() << "] Timings for all measures:\n" << AllMeasures.get_timings();
      report(3) << "[Rank " << c.rank() << "] Acceptance rate for all moves:\n" << AllMoves.get_statistics();
      report(3) << "[Rank " << c.rank() << "] Warmup lasted: " << get_warmup_time() << " seconds [" << get_warmup_time_HHMMSS() << "]\n";
      report(3) << "[Rank " << c.rank() << "] Simulation lasted: " << get_accumulation_time() << " seconds [" << get_accumulation_time_HHMMSS()
                << "]\n";
      report(3) << "[Rank " << c.rank() << "] Number of measures: " << nmeasures << std::endl;
      if (c.rank() == 0) report(2) << "Total number of measures: " << nmeasures_tot << std::endl;
    }

    /**
   * The acceptance rates of all move
   *
   * @return map : name_of_the_move -> acceptance rate of this move
   */
    std::map<std::string, double> get_acceptance_rates() const { return AllMoves.get_acceptance_rates(); }

    /**
   *  The current percents done
   */
    uint64_t get_percent() const { return done_percent; }

    /**
   * An access to the random number generator
   */
    random_generator &get_rng() { return RandomGenerator; }

    /**
   * The current cycle number
   */
    int get_current_cycle_number() const { return current_cycle_number; }

    /**
   * The current number of the visited configuration. Updated after each accept/reject.
   */
    int get_config_id() const { return config_id; }

    /**
   * The duration of the last run in seconds
   */
    double get_duration() const { return get_total_time(); }

    /**
   * The total time of the last run in seconds
   */
    double get_total_time() const { return get_warmup_time() + get_accumulation_time(); }

    /**
   * The time spent on warmup in seconds
   */
    double get_warmup_time() const { return double(timer_warmup); }

    /**
   * The time spent on warmup in hours, minutes, and seconds
   */
    auto get_warmup_time_HHMMSS() const { return hours_minutes_seconds_from_seconds(timer_warmup); }

    /**
   * The time spent on accumulation in seconds
   */
    double get_accumulation_time() const { return double(timer_accumulation); }

    /**
   * The time spent on warmup in hours, minutes, and seconds
   */
    auto get_accumulation_time_HHMMSS() const { return hours_minutes_seconds_from_seconds(timer_accumulation); }

    /// HDF5 interface
    friend void h5_write(h5::group g, std::string const &name, mc_generic const &mc) {
      auto gr = g.create_group(name);
      h5_write(gr, "moves", mc.AllMoves);
      h5_write(gr, "measures", mc.AllMeasures);
      h5_write(gr, "number_cycle_done", mc.current_cycle_number);
      h5_write(gr, "number_measure_done", mc.nmeasures);
      h5_write(gr, "sign", mc.sign);
    }

    /// HDF5 interface
    friend void h5_read(h5::group g, std::string const &name, mc_generic &mc) {
      auto gr = g.open_group(name);
      h5_read(gr, "moves", mc.AllMoves);
      h5_read(gr, "measures", mc.AllMeasures);
      h5_read(gr, "number_cycle_done", mc.current_cycle_number);
      h5_read(gr, "number_measure_done", mc.nmeasures);
      h5_read(gr, "sign", mc.sign);
    }

    private:
    random_generator RandomGenerator;
    move_set<MCSignType> AllMoves;
    measure_set<MCSignType> AllMeasures;
    std::vector<measure_aux> AllMeasuresAux;
    utility::report_stream report;
    uint64_t nmeasures, current_cycle_number = 0;
    utility::timer timer_accumulation, timer_warmup;
    std::function<void()> after_cycle_duty;
    MCSignType sign       = 1;
    uint64_t done_percent = 0;
    uint64_t config_id    = 0;
  };
} // namespace triqs::mc_tools
