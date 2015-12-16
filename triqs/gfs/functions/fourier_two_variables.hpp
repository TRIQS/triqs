#pragma once
#include <triqs/gfs.hpp>
#include <triqs/gfs/singularity/fit_tail.hpp>
#include "./fourier_tensor.hpp"
using triqs::gfs::curry;
namespace triqs { namespace gfs {
 using triqs::arrays::array;
 using triqs::arrays::array_const_view;
 ///FT from  $g_{uvw}(\tau, \tau')$ to $g_{uvw}(i\omega, i\Omega)$
 /**
   In two steps:
#. Fourier transform from  $g_{uvw}(\tau, \tau')$ to  $g_{uvw}(i\omega, \tau')$
#. Fourier transform from  $g_{uvw}(i\omega, \tau')$ to  $g_{uvw}(\tau, \tau')$
  */
 gf<cartesian_product<imfreq, imfreq>, tensor_valued<3>> fourier(gf_const_view<cartesian_product<imtime, imtime>, tensor_valued<3>> g2t,  int n_w_1,  int n_w_2);


 array<triqs::gfs::tail, 3> fit_tail(gf_const_view<imfreq, tensor_valued<3>> g, array_const_view<triqs::gfs::tail,3> known_moments, int max_moment, int n_min, int n_max);

 ///FT from $g_{uvw}(i\omega, i\Omega)$ to $g_{uvw}(\tau, \tau')$
 /**
   In two steps:
#. Fourier transform from  $g_{uvw}(i\omega, i\Omega)$ to  $g_{uvw}(\tau, i\Omega)$
#. Fourier transform from $g_{uvw}(\tau, i\Omega)$ to  $g_{uvw}(i\omega, i\Omega)$

@warning fitting the tails!
  */
 gf<cartesian_product<imtime, imtime>, tensor_valued<3>> inverse_fourier(gf_const_view<cartesian_product<imfreq, imfreq>, tensor_valued<3>> g2w,  int n_t_1,  int n_t_2, bool fit_tails=false);

}}
