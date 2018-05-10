/*******************************************************************************
 * 
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011 by M. Ferrero, O. Parcollet
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
//#include "./fourier_base.hpp"
#include <fftw3.h>
#include <algorithm>

namespace triqs::gfs::details {

 void fourier_base(const vector<dcomplex> &in, vector<dcomplex> &out, size_t L, bool direct) {

  // !!!! L must always be the number of time bins !!!!
  //const size_t L( (direct ? in.size() : out.size()) );
  //const int L(max(in.size(),out.size()));  <-- bug
  
  fftw_complex *inFFT, *outFFT;
  fftw_plan p;
  inFFT = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * L);
  outFFT = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * L);
  
  const dcomplex * restrict in_ptr = in.data_start();
  dcomplex * restrict out_ptr = out.data_start();
  const size_t imax = std::min(L,in.size());
  for (size_t i =0; i<imax; ++i) { inFFT[i][0] = real(in_ptr[i]); inFFT[i][1] = imag(in_ptr[i]);}
  for (size_t i =imax; i<L; ++i) { inFFT[i][0] = 0; inFFT[i][1] = 0;}
//   for (size_t i =0; i<L; ++i) { inFFT[i][0] = real(in_ptr[i]); inFFT[i][1] = imag(in_ptr[i]);}
  p = fftw_plan_dft_1d(L, inFFT, outFFT, (direct ? FFTW_BACKWARD : FFTW_FORWARD), FFTW_ESTIMATE);
  fftw_execute(p); 
  const size_t jmax = std::min(L,out.size());
  for (size_t j =0; j<jmax; ++j) out_ptr[j] = dcomplex(outFFT[j][0] ,outFFT[j][1]);
  fftw_destroy_plan(p); 
  fftw_free(inFFT); fftw_free(outFFT);
 }

 template<int Rank> // FIXME replace mini_vector by std::array
 void fourier_base(array_const_view<dcomplex, 2> &in, array_view<dcomplex, 2> &out, triqs::utility::mini_vector<int, Rank> const & dims, int fftw_count, bool direct) {

  // !!!! L must always be the number of time bins !!!!
  //const size_t L( (direct ? in.size() : out.size()) );
  //const int L(max(in.size(),out.size()));  <-- bug

  auto in_fft = reinterpret_cast<fftw_complex*>(in.data().data_start());
  auto out_fft = reinterpret_cast<fftw_complex*>(g_out.data().data_start());

  auto p = fftw_plan_many_dft(rank,                                            // rank
                              dims.ptr(),                                         // the dimension
                              n_others, 				       // how many FFT : here 1
                              g_in_FFT,                                        // in data
                              NULL,                                            // embed : unused. Doc unclear ?
                              in.indexmap().strides()[0],             	       // stride of the in data
                              1,                                               // in : shift for multi fft.
                              g_out_fft,                                       // out data
                              NULL,                                            // embed : unused. Doc unclear ?
                              out.indexmap().strides()[0],            	       // stride of the out data
                              1,                                               // out : shift for multi fft.
                              direct, FFTW_ESTIMATE);

  fftw_execute(p);
  fftw_destroy_plan(p);
 }

}
