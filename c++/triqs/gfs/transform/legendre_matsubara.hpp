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
 * @brief Provides transformations between Legendre-basis and Matsubara (imaginary-time / imaginary-frequency) Green's functions.
 */

#pragma once

#include "../gf/gf.hpp"
#include "../gf/gf_const_view.hpp"
#include "../gf/gf_view.hpp"
#include "../functions/functions2.hpp"
#include "../transform/fourier.hpp"
#include "../../mesh/imfreq.hpp"
#include "../../mesh/imtime.hpp"
#include "../../mesh/legendre.hpp"
#include "../../utility/legendre.hpp"

#include <cmath>

namespace triqs::gfs {

  // FIXME  REMOVE TAG AND gf_keeper
  // Tag identifying a deferred Legendre <-> Matsubara transformation carried by a gf_keeper.
  namespace tags {
    struct legendre {};
  } // namespace tags

  /**
   * @addtogroup triqs-gfs-legendre-matsubara
   * @{
   */

  // ----------------------------

  /**
   * @brief Fill a Matsubara Green's function from a Legendre Green's function.
   *
   * @details Performs the direct Legendre-to-Matsubara transform in imaginary frequency,
   * \f$ G(i\omega_n) = \sum_l T_{nl}\, G_l \f$, where \f$ T_{nl} \f$ is the Legendre-to-Matsubara
   * transformation matrix and \f$ G_l \f$ are the Legendre coefficients. The result is written into `gw`.
   *
   * @tparam G1 The type of the output Matsubara Green's function (must have an imfreq mesh).
   * @tparam G2 The type of the input Legendre Green's function (must have a legendre mesh).
   * @param gw The imaginary-frequency Green's function to fill (modified in place).
   * @param gl The input Legendre Green's function.
   */
  template <typename G1, typename G2>
    requires(is_gf_v<G1, mesh::imfreq>)
  void legendre_matsubara_direct(G1 &&gw, G2 const &gl) {

    static_assert(is_gf_v<G2, mesh::legendre>, "Second argument to legendre_matsubara_direct needs to be a Legendre Green function");
    static_assert(std::is_same_v<typename std::decay_t<G1>::target_t, typename std::decay_t<G2>::target_t>,
                  "Arguments to legendre_matsubara_direct require same target_t");

    gw() = 0.0;

    // Use the transformation matrix
    for (auto om : gw.mesh()) {
      for (auto l : gl.mesh()) { gw[om] += triqs::utility::legendre_T(om.index(), l.index()) * gl[l]; }
    }
  }

  // ----------------------------

  /// Fill an imaginary-time Green's function from a Legendre Green's function.
  template <typename G1, typename G2>
    requires(is_gf_v<G1, mesh::imtime>)
  void legendre_matsubara_direct(G1 &&gt, G2 const &gl) {

    static_assert(is_gf_v<G2, mesh::legendre>, "Second argument to legendre_matsubara_direct needs to be a Legendre Green function");
    static_assert(std::is_same_v<typename std::decay_t<G1>::target_t, typename std::decay_t<G2>::target_t>,
                  "Arguments to legendre_matsubara_direct require same target_t");

    gt() = 0.0;
    utility::legendre_generator L;

    for (auto t : gt.mesh()) {
      L.reset(2 * t / gt.mesh().beta() - 1);
      for (auto l : gl.mesh()) { gt[t] += std::sqrt(2 * l.index() + 1) / gt.mesh().beta() * gl[l] * L.next(); }
    }
  }

  // ----------------------------

  /**
   * @brief Fill a Legendre Green's function from a Matsubara Green's function.
   *
   * @details Performs the inverse Matsubara-to-Legendre transform. Starting from an imaginary-frequency
   * Green's function, the data is first transformed to imaginary time via a Fourier transform (which uses
   * the high-frequency tail) and the imaginary-time overload is then applied to obtain the Legendre
   * coefficients \f$ G_l \f$. The result is written into `gl`.
   *
   * @tparam G1 The type of the output Legendre Green's function (must have a legendre mesh).
   * @tparam G2 The type of the input Matsubara Green's function (must have an imfreq mesh).
   * @param gl The Legendre Green's function to fill (modified in place).
   * @param gw The input imaginary-frequency Green's function.
   */
  template <typename G1, typename G2>
    requires(is_gf_v<G2, mesh::imfreq>)
  void legendre_matsubara_inverse(G1 &&gl, G2 const &gw) {

    static_assert(is_gf_v<G1, mesh::legendre>, "First argument to legendre_matsubara_inverse needs to be a Legendre Green function");
    static_assert(std::is_same_v<typename std::decay_t<G1>::target_t, typename std::decay_t<G2>::target_t>,
                  "Arguments to legendre_matsubara_inverse require same target_t");

    gl() = 0.0;

    // Construct a temporary imaginary-time Green's function gt
    // I set Nt time bins. This is ugly, one day we must code the direct
    // transformation without going through imaginary time
    long Nt = 50000;
    auto gt =
       gf<mesh::imtime, typename std::decay_t<G1>::target_t>{{gw.mesh().beta(), gw.mesh().statistic(), Nt}, stdutil::front_pop(gw.data().shape())};

    // We first transform to imaginary time because it's been coded with the knowledge of the tails
    gt() = fourier(gw);
    legendre_matsubara_inverse(gl, gt());
  }

  // ----------------------------

  /// Fill a Legendre Green's function from an imaginary-time Green's function.
  template <typename G1, typename G2>
    requires(is_gf_v<G2, mesh::imtime>)
  void legendre_matsubara_inverse(G1 &&gl, G2 const &gt) {

    static_assert(is_gf_v<G1, mesh::legendre>, "First argument to legendre_matsubara_inverse needs to be a Legendre Green function");
    static_assert(std::is_same_v<typename std::decay_t<G1>::target_t, typename std::decay_t<G2>::target_t>,
                  "Arguments to legendre_matsubara_inverse require same target_t");

    gl() = 0.0;
    utility::legendre_generator L;
    auto N = gt.mesh().size() - 1;
    double coef{};

    // Do the integral over imaginary time
    for (auto t : gt.mesh()) {
      if (t.index() == 0 || t.index() == N)
        coef = 0.5;
      else
        coef = 1.0;
      L.reset(2 * t / gt.mesh().beta() - 1);
      for (auto l : gl.mesh()) { gl[l] += coef * std::sqrt(2 * l.index() + 1) * L.next() * gt[t]; }
    }
    gl.data() *= gt.mesh().delta();
  }

  // ----------------------------

  // Lightweight handle pairing a const view of a source Green's function with a transformation tag,
  // so that `g_dst() = legendre_to_imfreq(g_src)` can dispatch through triqs_gf_view_assign_delegation.
  template <typename Tag, typename D, typename Target> struct gf_keeper {
    gf_const_view<D, Target> g;
  };

  /**
   * @brief Tag a Legendre Green's function for assignment to a Matsubara Green's function.
   *
   * @details Returns a lightweight handle that, when assigned to an imaginary-frequency Green's function,
   * triggers the direct Legendre-to-Matsubara transform, e.g. `gw() = legendre_to_imfreq(gl)`.
   *
   * @tparam G The type of the input Legendre Green's function.
   * @param gl The input Legendre Green's function.
   * @return A handle holding a const view of `gl` tagged for the Legendre-to-imfreq transform.
   */
  template <typename G> inline gf_keeper<tags::legendre, mesh::legendre, typename G::target_t> legendre_to_imfreq(G const &gl) {
    static_assert(is_gf_v<G, mesh::legendre>, "legendre_to_imfreq takes a Legendre Green function");
    return {make_const_view(gl)};
  }

  /**
   * @brief Tag a Legendre Green's function for assignment to an imaginary-time Green's function.
   *
   * @details Returns a lightweight handle that, when assigned to an imaginary-time Green's function,
   * triggers the direct Legendre-to-imaginary-time transform, e.g. `gt() = legendre_to_imtime(gl)`.
   *
   * @tparam G The type of the input Legendre Green's function.
   * @param gl The input Legendre Green's function.
   * @return A handle holding a const view of `gl` tagged for the Legendre-to-imtime transform.
   */
  template <typename G> inline gf_keeper<tags::legendre, mesh::legendre, typename G::target_t> legendre_to_imtime(G const &gl) {
    static_assert(is_gf_v<G, mesh::legendre>, "legendre_to_imtime takes a Legendre Green function");
    return {make_const_view(gl)};
  }

  /**
   * @brief Tag a Matsubara Green's function for assignment to a Legendre Green's function.
   *
   * @details Returns a lightweight handle that, when assigned to a Legendre Green's function, triggers the
   * inverse Matsubara-to-Legendre transform, e.g. `gl() = imfreq_to_legendre(gw)`.
   *
   * @tparam G The type of the input Matsubara Green's function.
   * @param gw The input imaginary-frequency Green's function.
   * @return A handle holding a const view of `gw` tagged for the imfreq-to-Legendre transform.
   */
  template <typename G> inline gf_keeper<tags::legendre, mesh::imfreq, typename G::target_t> imfreq_to_legendre(G const &gw) {
    static_assert(is_gf_v<G, mesh::imfreq>, "imfreq_to_legendre takes a Matsubara Green function");
    return {make_const_view(gw)};
  }

  /**
   * @brief Tag an imaginary-time Green's function for assignment to a Legendre Green's function.
   *
   * @details Returns a lightweight handle that, when assigned to a Legendre Green's function, triggers the
   * inverse imaginary-time-to-Legendre transform, e.g. `gl() = imtime_to_legendre(gt)`.
   *
   * @tparam G The type of the input imaginary-time Green's function.
   * @param gt The input imaginary-time Green's function.
   * @return A handle holding a const view of `gt` tagged for the imtime-to-Legendre transform.
   */
  template <typename G> inline gf_keeper<tags::legendre, mesh::imtime, typename G::target_t> imtime_to_legendre(G const &gt) {
    static_assert(is_gf_v<G, mesh::imtime>, "imtime_to_legendre takes an imaginary time Green function");
    return {make_const_view(gt)};
  }

  /** @} */

  // ----------------------------

  // Assignment delegation: realise a Legendre-to-imfreq transform when a gf_keeper is assigned to a gf_view.
  template <typename T> void triqs_gf_view_assign_delegation(gf_view<mesh::imfreq, T> gw, gf_keeper<tags::legendre, mesh::legendre, T> const &L) {
    legendre_matsubara_direct(gw, L.g);
  }

  // Assignment delegation: realise a Legendre-to-imtime transform when a gf_keeper is assigned to a gf_view.
  template <typename T> void triqs_gf_view_assign_delegation(gf_view<mesh::imtime, T> gt, gf_keeper<tags::legendre, mesh::legendre, T> const &L) {
    legendre_matsubara_direct(gt, L.g);
  }

  // Assignment delegation: realise an imfreq-to-Legendre transform when a gf_keeper is assigned to a gf_view.
  template <typename T> void triqs_gf_view_assign_delegation(gf_view<mesh::legendre, T> gl, gf_keeper<tags::legendre, mesh::imfreq, T> const &L) {
    legendre_matsubara_inverse(gl, L.g);
  }

  // Assignment delegation: realise an imtime-to-Legendre transform when a gf_keeper is assigned to a gf_view.
  template <typename T> void triqs_gf_view_assign_delegation(gf_view<mesh::legendre, T> gl, gf_keeper<tags::legendre, mesh::imtime, T> const &L) {
    legendre_matsubara_inverse(gl, L.g);
  }

} // namespace triqs::gfs
