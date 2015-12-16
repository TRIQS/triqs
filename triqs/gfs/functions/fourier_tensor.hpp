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
  * @warning tail not used
  */
 gf<imfreq, tensor_valued<3>, nothing> fourier(gf_const_view<imtime, tensor_valued<3>, nothing> g_in, array_const_view<tail, 3> tail, int n_freq=1025);

 ///Inverse Fourier transform of tensor-valued functions
 gf<imtime, tensor_valued<3>, nothing> inverse_fourier(gf_const_view<imfreq, tensor_valued<3>, nothing> g_in, array_const_view<tail, 3> tail, int n_tau=1025);
}}
