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

/**
 * @file
 * @brief Provides concepts for the @ref triqs-mc.
 */

#pragma once

#include <mpi/communicator.hpp>

#include <complex>
#include <concepts>
#include <utility>

namespace triqs::mc_tools {

  /**
   * @addtogroup triqs-mc-concepts
   * @{
   */

  /**
   * @brief Check if a type is either a `double` or a `std::complex<double>`.
   * @tparam T Type to check.
   */
  template <typename T>
  concept DoubleOrComplex = std::same_as<T, double> || std::same_as<T, std::complex<double>>;

  /**
   * @brief Check if a type can be used as a MC move.
   * 
   * @details A MC move type is required to define the following methods:
   * - `attempt() -> MCSignType`: Proposes a new MC configuration and returns the acceptance ratio of the proposed move.
   * - `accept() -> MCSignType`: Accepts the previously proposed move and returns a possible sign correction.
   * 
   * See also triqs::mc_tools::move and triqs::mc_tools::move_set for more information.
   * 
   * @tparam T Type to check.
   */
  template <typename T, typename MCSignType>
  concept MCMove = requires(T x) {
    { x.attempt() } -> std::convertible_to<MCSignType>;
    { x.accept() } -> std::convertible_to<MCSignType>;
  };

  /**
   * @brief Check if a type can be used as a MC measure.
   * 
   * @details A MC measure type is required to define the following methods:
   * - `accumulate(MCSignType)`: Performs a measurement on the current MC configuration given the sign of its weight. 
   * - `collect_results(mpi::communicator const &)`: Collects results from multiple MPI processes.
   * 
   * See also triqs::mc_tools::measure, triqs::mc_tools::measure_set and triqs::mc_tools::measure_aux for more 
   * information.
   * 
   * @tparam T Type to check.
   */
  template <typename T, typename MCSignType>
  concept MCMeasure = requires(T x, mpi::communicator comm) {
    { x.accumulate(std::declval<MCSignType>()) };
    { x.collect_results(comm) };
  };

  /** @} */

} // namespace triqs::mc_tools
