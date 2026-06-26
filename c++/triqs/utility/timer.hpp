// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018 Simons Foundation
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
// Authors: Olivier Parcollet, Hugo U. R. Strand, Nils Wentzell

/**
 * @file
 * @brief A wall-clock timer that accumulates elapsed seconds across start/stop intervals.
 */

#pragma once

#include <chrono>

namespace triqs::utility {

  /**
   * @ingroup triqs-utility-runtime
   * @brief Accumulating wall-clock timer based on `std::chrono::high_resolution_clock`.
   *
   * @details It accumulates total elapsed time across multiple start() and stop() intervals. The current total time in
   * seconds, including any in-progress interval, can be retrieved by conversion to `double`.
   */
  class timer {
    public:
    /// Underlying clock type.
    using clock_t = std::chrono::high_resolution_clock;

    private:
    clock_t::time_point start_time;
    clock_t::duration total_time = clock_t::duration(0);
    bool running                 = false;

    public:
    /// Start (or resume) the timer.
    void start() {
      running    = true;
      start_time = clock_t::now();
    }

    /// Stop the timer and add the elapsed interval to the running total.
    void stop() {
      total_time += clock_t::now() - start_time;
      running = false;
    }

    /// Reset the accumulated time to zero and put the timer in the stopped state.
    void reset() {
      total_time = clock_t::duration(0);
      running    = false;
    }

    /**
     * @brief Whether the timer is currently running.
     * @return True if the timer is running, false if it is stopped.
     */
    [[nodiscard]] bool is_running() const { return running; }

    /**
     * @brief Implicit conversion to `double` (seconds).
     * @return The total accumulated time in seconds, including any in-progress interval if the timer is running.
     */
    operator double() const {
      std::chrono::duration<double> total_time_seconds(total_time);
      if (is_running()) total_time_seconds += clock_t::now() - start_time;
      return total_time_seconds.count();
    }
  };

} // namespace triqs::utility
