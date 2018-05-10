/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2014 by O. Parcollet
 *
 * TRIQS is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * TRIQS. If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/
#include "../../gfs.hpp"
//#include "./fourier_lattice.hpp"
#include <fftw3.h>

#define ASSERT_EQUAL(X,Y,MESS) if (X!=Y) TRIQS_RUNTIME_ERROR << MESS;

namespace triqs::gfs {

 // The implementation is almost the same in both cases...
 template <typename V1, typename V2>
 gf<V1, tensor_valued<1>> __impl(bool direct, gf_mesh<V1> const &out_mesh, gf<V2, tensor_valued<1>> g_in) {

  ASSERT_EQUAL(g_out.data().shape(), g_in.data().shape(), "Meshes are different");
  ASSERT_EQUAL(g_out.data().indexmap().strides()[1], g_out.data().shape()[1], "Unexpected strides in fourier implementation");
  ASSERT_EQUAL(g_out.data().indexmap().strides()[2], 1, "Unexpected strides in fourier implementation");
  ASSERT_EQUAL(g_in.data().indexmap().strides()[1], g_in.data().shape()[1], "Unexpected strides in fourier implementation");
  ASSERT_EQUAL(g_in.data().indexmap().strides()[2], 1, "Unexpected strides in fourier implementation");

  //check periodization_matrix is diagonal
  for(int i=0;i<g_in.mesh().periodization_matrix.shape()[0];i++)
   for(int j=0;j<g_in.mesh().periodization_matrix.shape()[1];j++)
    if(i!=j and g_in.mesh().periodization_matrix(i,j)!=0) TRIQS_RUNTIME_ERROR << "Periodization matrix must be diagonal for FFTW to work";

  auto g_out = gf<Var1, tensor_valued<1>>{out_mesh, {n_others}};
  long n_others = second_dim(g_in.data());

  auto dims = g_in.mesh().get_dimensions();
  details::fourier_base(g_in.data(), g_out.data(), dims, n_others, direct);

  return std::move(g_out);
 }

 // ------------------------ DIRECT TRANSFORM --------------------------------------------

 gf<brillouin_zone, tensor_valued<1>> _fourier_impl(gf_mesh<cyclic_lattice> const & r_mesh, gf<brillouin_zone, tensor_valued<1>>&& gk) {
  auto gr = __impl(FFTW_FORWARD, gk);
  gr.data() /= gk.mesh().size();
  return std::move(gr);
 }

  // ------------------------ INVERSE TRANSFORM --------------------------------------------

 gf<cyclic_lattice, tensor_valued<1>> _fourier_impl(gf_mesh<brillouin_zone> const & k_mesh, gf<cyclic_lattice, tensor_valued<1>>&& gr) {
  return __impl(FFTW_BACKWARD, gr);
 }

}
