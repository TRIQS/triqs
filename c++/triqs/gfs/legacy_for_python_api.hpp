// Copyright (c) 2015-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2015-2018 Centre national de la recherche scientifique (CNRS)
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
namespace triqs {
  namespace gfs {

    template <typename A> nda::matrix<nda::get_value_t<A>> make_matrix(A const &a) { return a; }

    template <typename A> void _gf_invert_data_in_place(A &a) {
      auto mesh_lengths = stdutil::mpop<2>(a.indexmap().lengths());
      nda::for_each(mesh_lengths, [&a, _ = nda::range::all](auto &&...i) { nda::inverse_in_place(make_matrix_view(a(i..., _, _))); });
    }

    // FOR LEGACY PYTHON CODE ONLY
    // THIS MUST be kept for python operations
    // specific operations (for legacy python code).
    // +=, -= with a matrix
    inline void operator+=(gf_view<imfreq> g, nda::matrix<std::complex<double>> const &m) {
      for (int u = 0; u < int(first_dim(g.data())); ++u) g.data()(u, nda::ellipsis()) += m;
    }

    inline void operator-=(gf_view<imfreq> g, nda::matrix<std::complex<double>> const &m) {
      for (int u = 0; u < int(first_dim(g.data())); ++u) g.data()(u, nda::ellipsis()) -= m;
    }

    inline void operator+=(gf_view<imfreq> g, std::complex<double> a) { operator+=(g, make_matrix(a * nda::eye<double>(g.target_shape()[0]))); }
    inline void operator-=(gf_view<imfreq> g, std::complex<double> a) { operator-=(g, make_matrix(a * nda::eye<double>(g.target_shape()[0]))); }

    // Same for scalar valued
    // THIS MUST be kept for python operations
    // specific operations (for legacy python code).
    inline void operator+=(gf_view<imfreq, scalar_valued> g, std::complex<double> a) { g.data() += a; }

    inline void operator-=(gf_view<imfreq, scalar_valued> g, std::complex<double> a) { g.data() -= a; }

    // - same with refreq
    inline void operator+=(gf_view<refreq> g, nda::matrix<std::complex<double>> const &m) {
      for (int u = 0; u < int(first_dim(g.data())); ++u) g.data()(u, nda::ellipsis()) += m;
    }

    inline void operator-=(gf_view<refreq> g, nda::matrix<std::complex<double>> const &m) {
      for (int u = 0; u < int(first_dim(g.data())); ++u) g.data()(u, nda::ellipsis()) -= m;
    }

    inline void operator+=(gf_view<refreq> g, std::complex<double> a) { operator+=(g, make_matrix(a * nda::eye<double>(g.target_shape()[0]))); }
    inline void operator-=(gf_view<refreq> g, std::complex<double> a) { operator-=(g, make_matrix(a * nda::eye<double>(g.target_shape()[0]))); }

    // Same for scalar valued
    // THIS MUST be kept for python operations
    // specific operations (for legacy python code).
    inline void operator+=(gf_view<refreq, scalar_valued> g, std::complex<double> a) { g.data() += a; }

    inline void operator-=(gf_view<refreq, scalar_valued> g, std::complex<double> a) { g.data() -= a; }

    //inline gf<imfreq> operator+(std::complex<double> const &m, gf_view<imfreq> g) { return g + m; }
    //inline gf<imfreq> operator+(nda::matrix<std::complex<double>> const &m, gf_view<imfreq> g) { return g + m; }

    // inline gf<imfreq> operator-(std::complex<double> const &m, gf_view<imfreq> g) {
    //  gf_view<imfreq> r = g;
    //  r *= -1;
    //  r += m;
    //  return r;
    // }

    //inline gf<imfreq> operator-(nda::matrix<std::complex<double>> const &m, gf_view<imfreq> g) {
    //gf_view<imfreq> r = g;
    //r *= -1;
    //r += m;
    //return r;
    //}

  } // namespace gfs
} // namespace triqs
