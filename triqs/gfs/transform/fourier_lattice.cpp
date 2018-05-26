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

namespace triqs {
namespace gfs {

 // We rewrite the scalar in term of the matrix 
 // We will change this for other FFT later when this FFT on lattice is checked.
 void _fourier_impl(gf_view<brillouin_zone, scalar_valued> gk, gf_const_view<cyclic_lattice, scalar_valued> gr) {
  _fourier_impl(reinterpret_scalar_valued_gf_as_matrix_valued(gk), reinterpret_scalar_valued_gf_as_matrix_valued(gr));
 }
 //--------------------------------------------------------------------------------------

 void _fourier_impl(gf_view<cyclic_lattice, scalar_valued> gr, gf_const_view<brillouin_zone, scalar_valued> gk) {
  _fourier_impl(reinterpret_scalar_valued_gf_as_matrix_valued(gr), reinterpret_scalar_valued_gf_as_matrix_valued(gk));
 }

 //--------------------------------------------------------------------------------------

 // The implementation is almost the same in both cases...
 template <typename V1, typename V2>
 void __impl(int sign, gf_view<V1, matrix_valued> g_out, gf_const_view<V2, matrix_valued> g_in) {

  ASSERT_EQUAL(g_out.data().shape(), g_in.data().shape(), "Meshes are different");
  ASSERT_EQUAL(g_out.data().indexmap().strides()[1], g_out.data().shape()[1], "Unexpected strides in fourier implementation");
  ASSERT_EQUAL(g_out.data().indexmap().strides()[2], 1, "Unexpected strides in fourier implementation");
  ASSERT_EQUAL(g_in.data().indexmap().strides()[1], g_in.data().shape()[1], "Unexpected strides in fourier implementation");
  ASSERT_EQUAL(g_in.data().indexmap().strides()[2], 1, "Unexpected strides in fourier implementation");

  //check periodization_matrix is diagonal
  for(int i=0;i<g_in.mesh().periodization_matrix.shape()[0];i++)
   for(int j=0;j<g_in.mesh().periodization_matrix.shape()[1];j++)
    if(i!=j and g_in.mesh().periodization_matrix(i,j)!=0) TRIQS_RUNTIME_ERROR << "Periodization matrix must be diagonal for FFTW to work";

  auto L = g_in.mesh().get_dimensions();
  auto rank = g_in.mesh().rank();
  auto in_FFT = reinterpret_cast<fftw_complex*>(g_in.data().data_start());
  auto outFFT = reinterpret_cast<fftw_complex*>(g_out.data().data_start());
  // auto p = fftw_plan_dft(rank, L.ptr(), in_FFT, outFFT, FFTW_BACKWARD, FFTW_ESTIMATE);

  // use the general routine that can do all the matrices at once.
  auto p = fftw_plan_many_dft(rank,                                            // rank
                              L.ptr(),                                         // the dimension
                              g_in.data().shape()[1] * g_in.data().shape()[2], // how many FFT : here 1
                              in_FFT,                                          // in data
                              NULL,                                            // embed : unused. Doc unclear ?
                              g_in.data().indexmap().strides()[0],             // stride of the in data
                              1,                                               // in : shift for multi fft.
                              outFFT,                                          // out data
                              NULL,                                            // embed : unused. Doc unclear ?
                              g_out.data().indexmap().strides()[0],            // stride of the out data
                              1,                                               // out : shift for multi fft.
                              sign, FFTW_ESTIMATE);


  fftw_execute(p);
  fftw_destroy_plan(p);
 }

 //--------------------------------------------------------------------------------------

 void _fourier_impl(gf_view<brillouin_zone, matrix_valued> gk, gf_const_view<cyclic_lattice, matrix_valued> gr) {
  __impl(FFTW_BACKWARD, gk, gr);
 }

 //--------------------------------------------------------------------------------------
 void _fourier_impl(gf_view<cyclic_lattice, matrix_valued> gr, gf_const_view<brillouin_zone, matrix_valued> gk) {
  __impl(FFTW_FORWARD, gr, gk);
  gr.data() /= gk.mesh().size();
 }
}
}
