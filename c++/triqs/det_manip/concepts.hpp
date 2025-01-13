// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2023 Simons Foundation
// Copyright (c) 2016 Igor Krivenko
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
// Authors: Michel Ferrero, JaksaVucicevic, Igor Krivenko, Henri Menke, Laura Messio, Olivier Parcollet, Priyanka Seth, Hugo U. R. Strand, Nils Wentzell

/**
 * @file
 * @brief Provides various concepts for the triqs::det_manip::det_manip class.
 */

#pragma once

#include "./utils.hpp"

#include <ranges>

namespace triqs::det_manip {

  /**
   * @addtogroup detmanip
   * @{
   */

  /**
   * @brief Matrix builder concept.
   *
   * @details A matrix builder is a callable object that takes two arguments and returns either a `std::floating_point`
   * or `std::complex` type.
   *
   * It can be used to build a matrix \f$ M \f$ with the elements \f$ M_{ij} = f(x_i, y_j) \f$, where \f$ f \f$ is the
   * matrix builder and \f$ x_i \f$ and \f$ y_j \f$ are the arguments that determine the element in row \f$ i \f$ and
   * column \f$ j \f$.
   *
   * @tparam F Type to check.
   */
  template <typename F>
  concept MatrixBuilder = requires(F f, detail::get_xarg_t<F> const &x, detail::get_yarg_t<F> const &y) {
    { f(x, y) } -> detail::RealOrComplex;
  };

  /**
   * @brief Concept that checks if the elements of a given range can be used as the first argument to a given
   * triqs::det_manip::MatrixBuilder object.
   *
   * @tparam R Range to check.
   * @tparam F triqs::det_manip::MatrixBuilder.
   */
  template <typename R, typename F>
  concept MatrixBuilderXRange = MatrixBuilder<F> && std::ranges::input_range<R> && std::ranges::sized_range<R>
     && requires(F f, R &&rg, detail::get_yarg_t<F> const &y) { f(*std::ranges::begin(rg), y); };

  /**
   * @brief Concept that checks if the elements of a given range can be used as the second argument to a given
   * triqs::det_manip::MatrixBuilder object.
   *
   * @tparam R Range to check.
   * @tparam F triqs::det_manip::MatrixBuilder.
   */
  template <typename R, typename F>
  concept MatrixBuilderYRange = MatrixBuilder<F> && std::ranges::input_range<R> && std::ranges::sized_range<R>
     && requires(F f, R &&rg, detail::get_xarg_t<F> const &x) { f(x, *std::ranges::begin(rg)); };

  /** @} */

} // namespace triqs::det_manip
