#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK

#include <triqs/gfs/re_im_freq.hpp> 
#include <triqs/gfs/re_im_time.hpp> 
#include <triqs/gfs/refreq_imtime.hpp> 

#include <triqs/gfs/local/fourier_real.hpp> 
#include <triqs/arrays.hpp>

namespace tql= triqs::clef;
using namespace triqs::gfs;

int main() {
 
 double precision=10e-9;
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
 auto G_w= make_gf<refreq, scalar_valued>(wmin, wmax, n_re_freq);
 
 triqs::clef::placeholder<0> w_;
 triqs::clef::placeholder<1> wn_;
 triqs::clef::placeholder<2> tau_;
 G_w_wn(w_,wn_)<<1/(wn_-1)/( pow(w_,3) );
 G_w_tau(w_,tau_)<< exp( -2*tau_ ) / (w_*w_ + 1 );
 
 int index = n_re_freq/3;
 double tau = std::get<1>(G_w_tau.mesh().components())[index];
 
 //identical functions
 G_w(w_) << exp( -2*tau ) / (w_*w_ + 1 );
 //the singularity must be removed as it is inexistent in re_im_time, to give the same TF. 
 G_w.singularity()(0)=triqs::arrays::matrix<double>{{0}};
 G_w.singularity()(1)=triqs::arrays::matrix<double>{{0}};
 G_w.singularity()(2)=triqs::arrays::matrix<double>{{0}};
 auto G_w2 = slice_mesh_imtime(G_w_tau, index);
 for(auto& w:G_w.mesh())
  if ( std::abs(G_w[w]-G_w2[w]) > precision) TRIQS_RUNTIME_ERROR<<" fourier_slice error : w="<< w <<" ,G_w="<< G_w[w]<<" ,G_w2="<< G_w2[w] <<"\n";
 
 //test of the interpolation
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
 h5_write(file, "gt0", gt);
 auto gw = slice_mesh_imtime(G_w_tau, 1);
 h5_write(file, "gw0", gw);

 //comparison of the TF of the one time and sliced two times GF's
 auto G_t = inverse_fourier(G_w);
 auto G_t2 = inverse_fourier(slice_mesh_imtime(G_w_tau, index) );
 for(auto& t:G_t.mesh())
  if ( std::abs(G_t[t]-G_t2[t]) > precision) TRIQS_RUNTIME_ERROR<<" fourier_slice_re_time error : t="<< t <<" ,G_t="<< G_t[t] <<" ,G_t2="<< G_t2[t] <<"\n";  
  
}
