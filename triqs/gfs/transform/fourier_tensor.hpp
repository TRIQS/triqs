/****************************************
 *  Copyright (C) 2015 by Thomas Ayral  *
 ****************************************/
#pragma once
#include <triqs/gfs.hpp>
namespace triqs { namespace gfs {

 void assign_(double & t, dcomplex c);
 void assign_(dcomplex & t, dcomplex c);
 template<int R>
  void assign_(triqs::arrays::array_view<double,R> t, triqs::arrays::array_view<dcomplex,R> c){t=real(c); ;}
 template<int R>
  void assign_(triqs::arrays::array_view<dcomplex,R> t, triqs::arrays::array_view<dcomplex,R> c){t=c;}

 using triqs::arrays::array;
 using triqs::arrays::array_const_view;
 ///Fourier transform of tensor-valued functions
 /**
  * @param g_in input Green's function $g_{abc}(\tau)$
  * @param tail singularity of the input Gf.
  * @param n_pts defined as n_pts = n_max +1 where n_max is the maximal matsubara index
  * @return $g_{abc}(i\omega)$
  * @warning tail not used
  */
 gf<imfreq, tensor_valued<3>> fourier(gf_const_view<imtime, tensor_valued<3>> g_in, array_const_view<__tail<scalar_valued>, 3> tail, int n_pts=1025, bool positive_frequencies_only=false);

 ///Inverse Fourier transform of tensor-valued functions
 /**
  * @param g_in input Green's function $g_{abc}(i\omega)$
  * @param tail singularity of the input Gf.
  * @param n_tau number of tau points
  * @return $g_{abc}(i\tau)$
  * @warning tail not used
  */
 gf<imtime, tensor_valued<3>> inverse_fourier(gf_const_view<imfreq, tensor_valued<3>> g_in, array_const_view<__tail<scalar_valued>, 3> tail, int n_tau=1025);
}}
