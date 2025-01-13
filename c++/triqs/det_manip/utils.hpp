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
 * @brief Provides various utilities for the triqs::det_manip::det_manip class.
 */

#pragma once

#include <triqs/utility/callable_traits.hpp>

#include <nda/nda.hpp>

#include <concepts>
#include <cmath>

namespace triqs::det_manip::detail {

  // Get the first argument type of a callable object.
  template <typename F> using get_xarg_t = typename triqs::utility::callable_traits<F>::template decay_arg_t<0>;

  // Get the second argument type of a callable object.
  template <typename F> using get_yarg_t = typename triqs::utility::callable_traits<F>::template decay_arg_t<1>;

  // Get the result type of a callable object.
  template <typename F> using get_result_t = typename triqs::utility::callable_traits<F>::result_type;

  // Concept that checks if a type is either a `std::floating_point` or an `nda::is_complex_v`.
  template <typename T>
  concept RealOrComplex = std::floating_point<T> || nda::is_complex_v<T>;

  // Calculate the relative difference between two scalar values.
  double rel_diff(RealOrComplex auto a, RealOrComplex auto b) {
    auto const tmp = std::abs(a) + std::abs(b);
    return (tmp == 0.0 ? 0.0 : 2 * std::abs(a - b) / tmp);
  }

  // Calculate the relative difference between two matrices.
  double rel_diff(nda::Matrix auto const &A, nda::Matrix auto const &B) {
    if (A.size() == 0) return 0.0;
    auto const tmp = nda::max_element(nda::abs(A)) + nda::max_element(nda::abs(B));
    return (tmp == 0 ? 0.0 : 2 * nda::max_element(nda::abs(A - B)) / tmp);
  }

} // namespace triqs::det_manip::detail
