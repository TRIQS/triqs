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

#include "../../gfs.hpp"
#include "./fourier_common.hpp"
#include <itertools/itertools.hpp>

#define ASSERT_EQUAL(X, Y, MESS)                                                                                                                     \
  if (X != Y) TRIQS_RUNTIME_ERROR << MESS;

namespace triqs::gfs {

  // The implementation is almost the same in both cases...
  template <typename M1, typename M2> gf_vec_t<M1> __impl(int fftw_backward_forward, M1 const &out_mesh, gf_vec_cvt<M2> g_in) {

    //ASSERT_EQUAL(g_out.data().shape(), g_in.data().shape(), "Meshes are different");
    //ASSERT_EQUAL(g_out.data().indexmap().strides()[1], g_out.data().shape()[1], "Unexpected strides in fourier implementation");
    //ASSERT_EQUAL(g_out.data().indexmap().strides()[2], 1, "Unexpected strides in fourier implementation");
    //ASSERT_EQUAL(g_in.data().indexmap().strides()[1], g_in.data().shape()[1], "Unexpected strides in fourier implementation");
    //ASSERT_EQUAL(g_in.data().indexmap().strides()[2], 1, "Unexpected strides in fourier implementation");

    auto g_out    = gf_vec_t<M1>{out_mesh, std::array{g_in.target_shape()[0]}};
    long n_others = second_dim(g_in.data());

    auto dims     = g_in.mesh().dims();
    auto dims_int = stdutil::make_std_array<int>(dims);

    _fourier_base(g_in.data(), g_out.data(), dims.size(), dims_int.data(), n_others, fftw_backward_forward);

    return std::move(g_out);
  }

  // ------------------------ DIRECT TRANSFORM --------------------------------------------

  gf_vec_t<mesh::cyclat> _fourier_impl(mesh::cyclat const &r_mesh, gf_vec_cvt<mesh::brzone> gk) {
    auto gr = __impl(FFTW_FORWARD, r_mesh, gk);
    gr.data() /= gk.mesh().size();
    return std::move(gr);
  }

  // ------------------------ INVERSE TRANSFORM --------------------------------------------

  gf_vec_t<mesh::brzone> _fourier_impl(mesh::brzone const &k_mesh, gf_vec_cvt<mesh::cyclat> gr) { return __impl(FFTW_BACKWARD, k_mesh, gr); }

} // namespace triqs::gfs
