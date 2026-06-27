// Copyright (c) 2014-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2014-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2021 Simons Foundation
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
 * @brief Provides functions specific to Legendre-basis Green's functions.
 */

#pragma once

#include "../gf/defs.hpp"
#include "../gf/gf_const_view.hpp"
#include "../gf/gf_view.hpp"
#include "../../mesh/legendre.hpp"

namespace triqs::gfs {

  //-------------------------------------------------------
  // For Legendre functions
  // ------------------------------------------------------

  /**
   * @ingroup triqs-gfs-tailfitting
   * @brief Extract the leading high-frequency moments of a Legendre Green's function.
   *
   * @details Returns the array of moments \f$ c_n \f$ such that the corresponding Matsubara Green's function behaves 
   * as \f$ G(i\omega) \sim \sum_n c_n / (i\omega)^n \f$ at large frequency.
   *
   * @param gl The Legendre Green's function.
   * @return An array of high-frequency moments.
   */
  array<dcomplex, 3> get_tail(gf_const_view<mesh::legendre> gl);

  /**
   * @ingroup triqs-gfs-basis
   * @brief Enforce a prescribed jump at \f$ \tau = 0 \f$ for a Legendre Green's function.
   *
   * @details The Legendre coefficients are adjusted in place so that the corresponding imaginary-time Green's 
   * function has the specified discontinuity \f$ G(0^+) - G(0^-) \f$ at \f$ \tau = 0 \f$ (which equals \f$ -1 \f$ for 
   * a fermionic propagator). Coefficients above the constrained subspace are left unchanged.
   *
   * @param gl Legendre Green's function modified in place.
   * @param disc Target discontinuity at \f$ \tau = 0 \f$.
   */
  void enforce_discontinuity(gf_view<mesh::legendre> gl, nda::array_const_view<double, 2> disc);

} // namespace triqs::gfs
