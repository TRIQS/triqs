// Copyright (c) 2014-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2014-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2019 Simons Foundation
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
// Authors: Olivier Parcollet, Nils Wentzell, tayral

#include "../../gfs.hpp"
#include "./fourier_common.hpp"
#include <itertools/itertools.hpp>

#define ASSERT_EQUAL(X, Y, MESS)                                                                                                                     \
  if (X != Y) TRIQS_RUNTIME_ERROR << MESS;

namespace triqs::gfs {

  // The implementation is almost the same in both cases...
  template <typename V1, typename V2> gf_vec_t<V1> __impl(int fftw_backward_forward, gf_mesh<V1> const &out_mesh, gf_vec_cvt<V2> g_in) {

    //ASSERT_EQUAL(g_out.data().shape(), g_in.data().shape(), "Meshes are different");
    //ASSERT_EQUAL(g_out.data().indexmap().strides()[1], g_out.data().shape()[1], "Unexpected strides in fourier implementation");
    //ASSERT_EQUAL(g_out.data().indexmap().strides()[2], 1, "Unexpected strides in fourier implementation");
    //ASSERT_EQUAL(g_in.data().indexmap().strides()[1], g_in.data().shape()[1], "Unexpected strides in fourier implementation");
    //ASSERT_EQUAL(g_in.data().indexmap().strides()[2], 1, "Unexpected strides in fourier implementation");

    //check periodization_matrix is diagonal
    auto &period_mat = g_in.mesh().periodization_matrix;
    for (auto [i, j] : itertools::product_range(period_mat.shape()[0], period_mat.shape()[1]))
      if (i != j and period_mat(i, j) != 0) {
        std::cerr
           << "WARNING: Fourier Transform of k-mesh with non-diagonal periodization matrix. Please make sure that the order of real and reciprocal space vectors is compatible for FFTW to work. (Cf. discussion doi:10.3929/ethz-a-010657714, p.26)\n";
        break;
      }

    auto g_out    = gf_vec_t<V1>{out_mesh, g_in.target_shape()[0]};
    long n_others = second_dim(g_in.data());

    auto dims = g_in.mesh().get_dimensions();
    _fourier_base(g_in.data(), g_out.data(), dims.size(), dims.ptr(), n_others, fftw_backward_forward);

    return std::move(g_out);
  }

  // ------------------------ DIRECT TRANSFORM --------------------------------------------

  gf_vec_t<cyclic_lattice> _fourier_impl(gf_mesh<cyclic_lattice> const &r_mesh, gf_vec_cvt<brillouin_zone> gk) {
    auto gr = __impl(FFTW_FORWARD, r_mesh, gk);
    gr.data() /= gk.mesh().size();
    return std::move(gr);
  }

  // ------------------------ INVERSE TRANSFORM --------------------------------------------

  gf_vec_t<brillouin_zone> _fourier_impl(gf_mesh<brillouin_zone> const &k_mesh, gf_vec_cvt<cyclic_lattice> gr) {
    return __impl(FFTW_BACKWARD, k_mesh, gr);
  }

} // namespace triqs::gfs
