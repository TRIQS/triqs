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

#include <mpi/communicator.hpp>

#include <complex>
#include <concepts>
#include <utility>

namespace triqs::mc_tools {

  /**
   * @brief Check if a type is either a double or a std::complex<double>.
   * @tparam T Type to check.
   */
  template <typename T>
  concept DoubleOrComplex = std::same_as<T, double> || std::same_as<T, std::complex<double>>;

  /**
   * @brief Check if a type can be used as a MC move.
   * @details It checks if the given type has `attempt()` and `accept()` methods that return a value which is
   * convertible to `MCSignType`.
   * @tparam T Type to check.
   */
  template <typename T, typename MCSignType>
  concept MCMove = requires(T x) {
    { x.attempt() } -> std::convertible_to<MCSignType>;
    { x.accept() } -> std::convertible_to<MCSignType>;
  };

  /**
   * @brief Check if a type can be used as a MC measure.
   * @details It checks if the given type has `accumulate(MCSignType)` and `collect_results(mpi::communicator const &)`
   * methods.
   * @tparam T Type to check.
   */
  template <typename T, typename MCSignType>
  concept MCMeasure = requires(T x, mpi::communicator comm) {
    { x.accumulate(std::declval<MCSignType>()) };
    { x.collect_results(comm) };
  };

} // namespace triqs::mc_tools
