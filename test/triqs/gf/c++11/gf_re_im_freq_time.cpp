#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK

#include <triqs/gfs/re_im_freq.hpp> 
#include <triqs/gfs/re_im_time.hpp> 
#include <triqs/gfs/refreq_imtime.hpp> 
#include <triqs/gfs/product.hpp> 
#include <triqs/gfs/curry.hpp> 

#include <triqs/gfs/local/fourier_real.hpp> 
#include <triqs/arrays.hpp>

namespace tql= triqs::clef;
using namespace triqs::gfs;

int main() {

try { 

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
 
 //auto G_t_tau= make_gf<re_im_time, scalar_valued>(   tmin, tmax, n_re_time, beta, Fermion, n_im_time);
 auto G_w_wn = make_gf<re_im_freq, scalar_valued>(   wmin, wmax, n_re_freq, beta, Fermion, n_im_freq);
 auto G_w_tau= make_gf<refreq_imtime, scalar_valued>(wmin, wmax, n_re_freq, beta, Fermion, n_im_time);
 auto G_w= make_gf<refreq, scalar_valued>(wmin, wmax, n_re_freq);

 auto G_t_tau= make_gf<cartesian_product<retime,imtime>, scalar_valued>(gf_mesh<retime>(tmin, tmax, n_re_time), gf_mesh<imtime>(beta, Fermion, n_im_time));
 //auto G_t_tau_N= make_gf<cartesian_product<retime,imtime>, scalar_valued>(  {tmin, tmax, n_re_time}, {beta, Fermion, n_im_time});

 auto G_w_wn2 = make_gf<cartesian_product<refreq,imfreq>, scalar_valued>( gf_mesh<refreq>(wmin, wmax, n_re_freq), gf_mesh<imfreq>(beta, Fermion, n_im_freq));
 auto G_w_tau2 = make_gf<cartesian_product<refreq,imtime>, scalar_valued>( gf_mesh<refreq>(wmin, wmax, n_re_freq), gf_mesh<imtime>(beta, Fermion, n_im_time,full_bins));


 //auto g_tau = slice_mesh1(G_w_tau(),1);
 //auto g_wn = G_w_wn_curry0[1];

 //std::cout  << G_t_tau_N (0.1,0.2) << std::endl;

 auto G_w_wn2_view = G_w_wn2();
 auto G_w_wn_sl0_a = partial_eval<0>(G_w_wn2(), std::make_tuple(8));
 //auto G_w_wn_curry0_a = curry0(G_w_wn2);
 //auto G_w_wn_sl0_a = slice_mesh0(G_w_wn2(), 8);

 triqs::clef::placeholder<0> w_;
 triqs::clef::placeholder<1> wn_;
 triqs::clef::placeholder<2> tau_;
 G_w_wn(w_,wn_)<<1/(wn_-1)/( pow(w_,3) );
 G_w_wn2(w_,wn_)<<1/(wn_-1)/( pow(w_,3) );
 G_w_tau(w_,tau_)<< exp( -2*tau_ ) / (w_*w_ + 1 );
 G_w_tau2(w_,tau_)<< exp( -2*tau_ ) / (w_*w_ + 1 );
 
 int index = n_re_freq/3;
 double tau = std::get<1>(G_w_tau.mesh().components())[index];
 
 //identical functions
 G_w(w_) << exp( -2*tau ) / (w_*w_ + 1 );
 //the singularity must be removed as it is inexistent in re_im_time, to give the same TF. 
 G_w.singularity()(0)=triqs::arrays::matrix<double>{{0}};
 G_w.singularity()(1)=triqs::arrays::matrix<double>{{0}};
 G_w.singularity()(2)=triqs::arrays::matrix<double>{{0}};
 //auto G_w2 = slice_mesh1(G_w_tau(), index);
 auto G_w2 = slice_mesh_imtime(G_w_tau, index);
 for(auto& w:G_w.mesh())
  if ( std::abs(G_w[w]-G_w2[w]) > precision) TRIQS_RUNTIME_ERROR<<" fourier_slice error : w="<< w <<" ,G_w="<< G_w[w]<<" ,G_w2="<< G_w2[w] <<"\n";
 
 //test of the interpolation
 std::cout << G_t_tau(0.789,0.123) << std::endl; 
 std::cout << "G_w_wn( 0.789,0.123) "<< G_w_wn( 0.789,0.123) << std::endl; 
 std::cout << "G_w_wn( 0.789,0.123) "<<G_w_wn2( 0.789,0.123) << std::endl; 
 std::cout << "G_w_tau(0.789,0.123)" << G_w_tau(0.789,0.123) << std::endl; 
 std::cout << "G_w_tau(0.789,0.123)" << G_w_tau2(0.789,0.123) << std::endl; 

 // test curry
 std::cout  << "curry no"<< G_w_wn.on_mesh(8,3) << std::endl ;

 auto G_w_wn_curry0 = curry<0>(G_w_wn2);
 auto G_w_wn_curry1 = curry<1>(G_w_wn2);
 auto G_w_wn2_view2 = G_w_wn2();

 std::cout  << " curry "<<G_w_wn_curry0[8] << G_w_wn_curry0[8][3] << G_w_wn2_view2.on_mesh(8,3) << G_w_wn2_view2.on_mesh(8,3) <<std::endl ;
 std::cout  << " curry "<<G_w_wn_curry1[3][8] << std::endl; 
 std::cout  << "G_w_wn_sl0_a [3]"<<G_w_wn_sl0_a[3] << std::endl ;


 // test hdf5 
 H5::H5File file("gf_re_im_freq_time.h5", H5F_ACC_TRUNC );
 h5_write(file, "g_t_tau", G_t_tau);
 h5_write(file, "g_w_wn",  G_w_wn);
 h5_write(file, "g_w_wn2", G_w_wn2);
 h5_write(file, "g_w_tau", G_w_tau);

 /*
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
*/
}
catch(std::exception const & e ) { std::cout  << "error "<< e.what()<< std::endl;}
}
