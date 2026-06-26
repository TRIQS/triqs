// Copyright (c) 2017-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2017-2018 Centre national de la recherche scientifique (CNRS)
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
// Authors: Hugo U. R. Strand, Nils Wentzell

/**
 * @file
 * @brief Small helpers that format wall-clock timestamps and durations for human-readable logs.
 */

#pragma once

#include "./timer.hpp"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

namespace triqs::utility {

  /**
   * @addtogroup triqs-utility-runtime
   * @{
   */

  /**
   * @brief Current local time formatted as `HH:MM:SS`.
   * @return Local time string.
   */
  std::string inline timestamp() {
    std::ostringstream s;
    auto now          = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    s << std::put_time(std::localtime(&now_c), "%H:%M:%S");
    return s.str();
  }

  /**
   * @brief Format an absolute number of seconds as `HH:MM:SS`.
   * 
   * @param sec Duration in seconds.
   * @return Zero-padded `HH:MM:SS` string.
   */
  std::string inline hours_minutes_seconds_from_seconds(double sec) {
    auto s = std::chrono::seconds{int(sec)};
    auto h = std::chrono::duration_cast<std::chrono::hours>(s);
    auto m = std::chrono::duration_cast<std::chrono::minutes>(s -= h);
    s -= m;
    std::ostringstream os;
    os << std::setfill('0') << std::setw(2) << h.count() << ":" << std::setfill('0') << std::setw(2) << m.count() << ":" << std::setfill('0')
       << std::setw(2) << s.count();
    return os.str();
  }

  /**
   * @brief Linear extrapolation of the remaining time of a loop, formatted as `HH:MM:SS`.
   *
   * @details It uses the provided (running) @ref timer to estimate \f$ \eta = (N - 1 - n) * t / (n + 1) \f$, where \f$ 
   * N \f$ is the total number of iterations, \f$ n \f$ is the current iteration index (zero-based), and \f$ t \f$ is 
   * the total elapsed time so far.
   * 
   * @param N Total number of iterations the loop will perform.
   * @param n Index of the current iteration (zero-based).
   * @param t Running timer covering the iterations so far.
   * @return Estimated remaining time as `HH:MM:SS`.
   */
  std::string inline estimate_time_left(int N, int n, timer &t) {
    double eta = (N - 1 - n) * double(t) / (n + 1);
    return hours_minutes_seconds_from_seconds(eta);
  }

  /** @} */

} // namespace triqs::utility
