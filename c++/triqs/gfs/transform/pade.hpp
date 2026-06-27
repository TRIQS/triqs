// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2023 Simons Foundation
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
 * @brief Provides the Pade analytic continuation from Matsubara to real frequencies.
 */

#pragma once

#include "../gf/gf_const_view.hpp"
#include "../gf/gf_view.hpp"
#include "../gf/targets.hpp"
#include "../functions/functions2.hpp"
#include "../../mesh/imfreq.hpp"
#include "../../mesh/refreq.hpp"
#include "../../utility/macros.hpp"

#include <tuple>

namespace triqs::gfs {

  /**
   * @addtogroup triqs-gfs-pade
   * @{
   */

  /**
   * @brief Analytically continue a scalar Matsubara Green's function to the real-frequency axis via Pade.
   *
   * @details Builds a Pade approximant from the first `n_points` positive Matsubara frequencies of `gw` and
   * evaluates it on the real-frequency mesh of `gr`, shifted into the upper half-plane by `freq_offset`,
   * i.e. at \f$ \omega + i\,\eta \f$ with \f$ \eta = \f$ `freq_offset`. The result is written into `gr`.
   *
   * @param gr The real-frequency Green's function to fill (modified in place).
   * @param gw The input scalar Matsubara Green's function.
   * @param n_points Number of Matsubara frequencies used to construct the Pade approximant.
   * @param freq_offset Imaginary shift \f$ \eta \f$ added to the real-frequency axis during evaluation.
   */
  void pade(gf_view<mesh::refreq, scalar_valued> gr, gf_const_view<mesh::imfreq, scalar_valued> gw, int n_points, double freq_offset);

  /**
   * @brief Analytically continue a matrix-valued Matsubara Green's function to the real-frequency axis via Pade.
   *
   * @details Applies the scalar Pade continuation element-wise over the target indices of `gw` and `gr`,
   * which must share the same target shape.
   *
   * @tparam GR The type of the real-frequency Green's function (must have a refreq mesh).
   * @tparam GW The type of the Matsubara Green's function (must have an imfreq mesh).
   * @param gr The real-frequency Green's function to fill (modified in place).
   * @param gw The input Matsubara Green's function.
   * @param n_points Number of Matsubara frequencies used to construct each Pade approximant.
   * @param freq_offset Imaginary shift \f$ \eta \f$ added to the real-frequency axis during evaluation.
   */
  template <MemoryGf<mesh::refreq> GR, MemoryGf<mesh::imfreq> GW>
  void pade(GR &gr, GW const &gw, int n_points, double freq_offset)
    requires(GR::target_rank > 0 && GW::target_rank > 0)
  {
    EXPECTS(gr.target_shape() == gw.target_shape());
    for (auto argtpl : gr.target_indices()) {
      std::apply([&](auto &&...args) { pade(slice_target_to_scalar(gr, args...), slice_target_to_scalar(gw, args...), n_points, freq_offset); },
                 argtpl);
    }
  }

  /** @} */

} // namespace triqs::gfs
