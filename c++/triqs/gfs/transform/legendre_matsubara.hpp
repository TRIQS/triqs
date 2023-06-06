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

#pragma once

#include <triqs/utility/legendre.hpp>
#include "../../gfs.hpp"

#include <cmath>

namespace triqs::gfs {

  // FIXME  REMOVE TAG AND gf_keeper
  namespace tags {
    struct legendre {};
  } // namespace tags

  // ----------------------------

  template <typename G1, typename G2>
    requires(is_gf_v<G1, imfreq>)
  void legendre_matsubara_direct(G1 &&gw, G2 const &gl) {

    static_assert(is_gf_v<G2, legendre>, "Second argument to legendre_matsubara_direct needs to be a Legendre Green function");
    static_assert(std::is_same_v<typename std::decay_t<G1>::target_t, typename std::decay_t<G2>::target_t>,
                  "Arguments to legendre_matsubara_direct require same target_t");

    gw() = 0.0;

    // Use the transformation matrix
    for (auto om : gw.mesh()) {
      for (auto l : gl.mesh()) { gw[om] += triqs::utility::legendre_T(om.index(), l.index()) * gl[l]; }
    }
  }

  // ----------------------------

  template <typename G1, typename G2>
    requires(is_gf_v<G1, imtime>)
  void legendre_matsubara_direct(G1 &&gt, G2 const &gl) {

    static_assert(is_gf_v<G2, legendre>, "Second argument to legendre_matsubara_direct needs to be a Legendre Green function");
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

  template <typename G1, typename G2>
    requires(is_gf_v<G2, imfreq>)
  void legendre_matsubara_inverse(G1 &&gl, G2 const &gw) {

    static_assert(is_gf_v<G1, legendre>, "First argument to legendre_matsubara_inverse needs to be a Legendre Green function");
    static_assert(std::is_same_v<typename std::decay_t<G1>::target_t, typename std::decay_t<G2>::target_t>,
                  "Arguments to legendre_matsubara_inverse require same target_t");

    gl() = 0.0;

    // Construct a temporary imaginary-time Green's function gt
    // I set Nt time bins. This is ugly, one day we must code the direct
    // transformation without going through imaginary time
    long Nt = 50000;
    auto gt = gf<imtime, typename std::decay_t<G1>::target_t>{{gw.mesh().beta(), gw.mesh().statistic(), Nt}, stdutil::front_pop(gw.data().shape())};

    // We first transform to imaginary time because it's been coded with the knowledge of the tails
    gt() = fourier(gw);
    legendre_matsubara_inverse(gl, gt());
  }

  // ----------------------------

  template <typename G1, typename G2>
    requires(is_gf_v<G2, imtime>)
  void legendre_matsubara_inverse(G1 &&gl, G2 const &gt) {

    static_assert(is_gf_v<G1, legendre>, "First argument to legendre_matsubara_inverse needs to be a Legendre Green function");
    static_assert(std::is_same_v<typename std::decay_t<G1>::target_t, typename std::decay_t<G2>::target_t>,
                  "Arguments to legendre_matsubara_inverse require same target_t");

    gl() = 0.0;
    utility::legendre_generator L;
    auto N = gt.mesh().size() - 1;
    double coef;

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

  template <typename Tag, typename D, typename Target> struct gf_keeper {
    gf_const_view<D, Target> g;
  };

  template <typename G> inline gf_keeper<tags::legendre, legendre, typename G::target_t> legendre_to_imfreq(G const &gl) {
    static_assert(is_gf_v<G, legendre>, "legendre_to_imfreq takes a Legendre Green function");
    return {make_const_view(gl)};
  }

  template <typename G> inline gf_keeper<tags::legendre, legendre, typename G::target_t> legendre_to_imtime(G const &gl) {
    static_assert(is_gf_v<G, legendre>, "legendre_to_imtime takes a Legendre Green function");
    return {make_const_view(gl)};
  }

  template <typename G> inline gf_keeper<tags::legendre, imfreq, typename G::target_t> imfreq_to_legendre(G const &gw) {
    static_assert(is_gf_v<G, imfreq>, "imfreq_to_legendre takes a Matsubara Green function");
    return {make_const_view(gw)};
  }

  template <typename G> inline gf_keeper<tags::legendre, imtime, typename G::target_t> imtime_to_legendre(G const &gt) {
    static_assert(is_gf_v<G, imtime>, "imtime_to_legendre takes an imaginary time Green function");
    return {make_const_view(gt)};
  }

  // ----------------------------

  template <typename T> void triqs_gf_view_assign_delegation(gf_view<imfreq, T> gw, gf_keeper<tags::legendre, legendre, T> const &L) {
    legendre_matsubara_direct(gw, L.g);
  }

  template <typename T> void triqs_gf_view_assign_delegation(gf_view<imtime, T> gt, gf_keeper<tags::legendre, legendre, T> const &L) {
    legendre_matsubara_direct(gt, L.g);
  }

  template <typename T> void triqs_gf_view_assign_delegation(gf_view<legendre, T> gl, gf_keeper<tags::legendre, imfreq, T> const &L) {
    legendre_matsubara_inverse(gl, L.g);
  }

  template <typename T> void triqs_gf_view_assign_delegation(gf_view<legendre, T> gl, gf_keeper<tags::legendre, imtime, T> const &L) {
    legendre_matsubara_inverse(gl, L.g);
  }

} // namespace triqs::gfs
