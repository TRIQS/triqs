#pragma once
#include <triqs/gfs.hpp>
#include <triqs/gfs/singularity/fit_tail.hpp>
#include "./fourier_tensor.hpp"
using triqs::gfs::curry;
namespace triqs { namespace gfs {
 using triqs::arrays::array;
 using triqs::arrays::array_const_view;

 ///FT from  $g_{uvw}(\tau,\tau')$ to $g_{uvw}(i\omega,i\Omega)$
 /**
   Fourier transform from  $g_{uvw}(\tau,\tau')$ to $g_{uvw}(i\omega,i\Omega)$ :
   $$g_{uvw}(i\omega,i\Omega)=\iint_{0}^{\beta}d\taud\tau'e^{i\omega\tau+i\Omega\tau'}g_{uvw}(\tau,\tau')$$
   In two steps:
#. Fourier transform from  $g_{uvw}(\tau,\tau')$ to  $g_{uvw}(i\omega,\tau')$
#. Fourier transform from  $g_{uvw}(i\omega,\tau')$ to  $g_{uvw}(i\omega,i\Omega)$
   @param g2t $g_{uvw}(\tau,\tau')$
   @param n_w_1 defined as n_w_1 = n_max + 1 (n_max: max Matsubara index)
   @param n_w_2 defined as n_w_2 = n_max + 1 (n_max: max Matsubara index
   @param positive_matsub_only_1 if true only positive fermionic Matsubara frequencies
   @param positive_matsub_only_2 if true only positive bosonic Matsubara frequencies
   @return $g_{uvw}(i\omega,i\Omega)$
  */
 gf<cartesian_product<imfreq,imfreq>, tensor_valued<3>> fourier(gf_const_view<cartesian_product<imtime, imtime>, tensor_valued<3>> g2t,  int n_w_1,  int n_w_2, bool positive_matsub_only_1=true, bool positive_matsub_only_2=true);


 ///FT from $g_{uvw}(i\omega,i\Omega)$ to $g_{uvw}(\tau,\tau')$
 /**
  * Fourier Transform from $g_{uvw}(i\omega,i\Omega)$ to $g_{uvw}(\tau,\tau')$ :
  $$g_{uvw}(\tau,\tau')=\frac{1}{\beta^2}\sum_{n=n_\mathrm{min}}^{n_\mathrm{n_max}}\sum_{m=n_\mathrm{min}}^{n_\mathrm{max}}e^{-i\omega_n\tau-i\Omega_m\tau'}g_{uvw}(i\omega_n,i\Omega_m)$$
   In two steps:
#. Fourier transform from  $g_{uvw}(i\omega,i\Omega)$ to  $g_{uvw}(\tau,i\Omega)$
#. Fourier transform from $g_{uvw}(\tau,i\Omega)$ to  $g_{uvw}(i\omega,i\Omega)$
   @param g2w $g_{uvw}(i\omega,i\Omega)$
   @param n_t_1 number of imaginary time points 
   @param n_t_2 number of imaginary time points 
   @return $g_{uvw}(i\tau,i\tau')$
@warning fitting the tails!
  */
 gf<cartesian_product<imtime, imtime>, tensor_valued<3>> inverse_fourier(gf_const_view<cartesian_product<imfreq, imfreq>, tensor_valued<3>> g2w,  int n_t_1,  int n_t_2, bool fit_tails=false);

}}
