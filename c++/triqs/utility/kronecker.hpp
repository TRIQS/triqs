// Copyright (c) 2014 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2014 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2020-2021 Simons Foundation
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
 * @brief Kronecker delta \f$ \delta_{ij} \f$ for arithmetic types, with a CLEF lazy-expression hook.
 */

#pragma once

#include <nda/clef.hpp>

#include <type_traits>

namespace triqs::utility {

  /**
   * @ingroup triqs-utility-math
   * @brief Kronecker delta \f$ \delta_{ij} \f$.
   * 
   * @tparam T1 Arithmetic type of the first index.
   * @tparam T2 Arithmetic type of the second index.
   * @param i Index \f$ i \f$.
   * @param j Index \f$ j \f$.
   * @return True if the given indices are equal, false otherwise.
   */
  template <typename T1, typename T2>
    requires(std::is_arithmetic_v<T1> and std::is_arithmetic_v<T2>)
  inline bool kronecker(T1 i, T2 j) {
    return i == j;
  }

} // namespace triqs::utility

namespace nda::clef {

  using triqs::utility::kronecker;

  // Make `kronecker` available as a lazy CLEF expression node.
  CLEF_MAKE_FNT_LAZY(kronecker);

} // namespace nda::clef
