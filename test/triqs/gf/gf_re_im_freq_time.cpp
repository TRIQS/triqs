#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK

#include <triqs/gf/re_im_freq.hpp> 
#include <triqs/gf/re_im_time.hpp> 
#include <triqs/gf/refreq_imtime.hpp> 

#include <triqs/gf/local/fourier_real.hpp> 
#include <triqs/arrays.hpp>

namespace tql= triqs::clef;
using triqs::gf::Fermion;
using triqs::gf::make_gf;
using triqs::gf::retime;
using triqs::gf::refreq;
using triqs::gf::refreq_imtime;
using triqs::gf::re_im_time;
using triqs::gf::re_im_freq;
using triqs::arrays::make_shape;
using triqs::gf::scalar_valued;

int main() {
  
 double beta =1.;
  
 double tmin=0.;
 double tmax=1.0;
 int n_re_time=100;
 int n_im_time=100;
 
 double wmin=0.;
 double wmax=1.0;
 int n_re_freq=100;
 int n_im_freq=100;
 
 auto G_t_tau= make_gf<re_im_time, scalar_valued>(   tmin, tmax, n_re_time, beta, Fermion, n_im_time);
 auto G_w_wn = make_gf<re_im_freq, scalar_valued>(   wmin, wmax, n_re_freq, beta, Fermion, n_im_freq);
 auto G_w_tau= make_gf<refreq_imtime, scalar_valued>(wmin, wmax, n_re_freq, beta, Fermion, n_im_time);
 
 triqs::clef::placeholder<0> w_;
 triqs::clef::placeholder<1> wn_;
 triqs::clef::placeholder<2> tau_;
 G_w_wn(w_,wn_)<<1/(wn_-1)/( pow(w_,3) );
 G_w_tau(w_,tau_)<< exp( -2*tau_ ) / (w_*w_ +1 );
 
 std::cout << G_t_tau(0.789,0.123) << std::endl; 
 std::cout << G_w_wn( 0.789,0.123) << std::endl; 
 std::cout << G_w_tau(0.789,0.123) << std::endl; 
 
 // test hdf5 
 H5::H5File file("gf_re_im_freq_time.h5", H5F_ACC_TRUNC );
 h5_write(file, "g_t_tau", G_t_tau);
 h5_write(file, "g_w_wn",  G_w_wn);
 h5_write(file, "g_w_tau", G_w_tau);

 // try to slice it
 auto gt = slice_mesh_imtime(G_t_tau, 1);
 std::cout  << gt.data()<< std::endl ;
 h5_write(file, "gt0", gt);
 auto gw = slice_mesh_imtime(G_w_tau, 1);
 std::cout  << gw.data()<< std::endl ;
 h5_write(file, "gw0", gw);


}
