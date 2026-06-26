// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018 Simons Foundation
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
// Authors: Olivier Parcollet, Nils Wentzell

/**
 * @file
 * @brief Callbacks used by the Monte Carlo framework.
 */

#pragma once

#include "./first_include.hpp"

#include <functional>

namespace triqs::utility {

  /**
   * @ingroup triqs-utility-runtime
   * @brief Build a callback that returns `true` once a given time in seconds has elapsed since its creation.
   *
   * @details Typically used as a stop condition for long-running loops (Monte Carlo cycles, optimisers, ...). The first 
   * invocation starts the internal clock. Subsequent invocations check the elapsed wall-clock time.
   * 
   * @param time_in_seconds Wall-clock budget in seconds.
   * @return A `std::function<bool()>` that returns `true` once the budget is exhausted.
   */
  std::function<bool()> clock_callback(int time_in_seconds);

} // namespace triqs::utility
