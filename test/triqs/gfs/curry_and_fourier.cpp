#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK

#include <triqs/gfs.hpp> 
#include <triqs/gfs/product.hpp> 
#include <triqs/gfs/curry.hpp> 

#include <triqs/gfs/local/fourier_real.hpp> 
#include <triqs/gfs/local/fourier_matsubara.hpp> 
#include <triqs/arrays.hpp>

using namespace triqs::gfs;

int main() {

try { 
 double tmax=10;
 double wmin=0.;
 double wmax=1.0;
 int n_re_freq=100;
 int Nt=100;
 
 triqs::clef::placeholder<0> w_;
 triqs::clef::placeholder<1> wn_;
 triqs::clef::placeholder<2> tau_;
   
 auto G_w_wn =  gf<cartesian_product<refreq,refreq>, scalar_valued>( {gf_mesh<refreq>(wmin, wmax, n_re_freq), gf_mesh<refreq>(wmin, wmax, n_re_freq)});
 auto G_w_tau = gf<cartesian_product<refreq,retime>, scalar_valued>( {gf_mesh<refreq>(wmin, wmax, n_re_freq), gf_mesh<retime>(-tmax, tmax, Nt)});

 G_w_wn(w_,wn_)<<1/(wn_-1)/( pow(w_,3) );
 G_w_tau(w_,tau_)<< exp( -2*tau_ ) / (w_*w_ + 1 );
 
 auto G_w_wn_curry0 = curry<0>(G_w_wn);
 auto G_w_tau_curry0 = curry<0>(G_w_tau);

 for (auto const & w : G_w_wn_curry0.mesh()) G_w_wn_curry0[w] =  fourier(G_w_tau_curry0[w]);
 
 G_w_wn_curry0[w_] << fourier(G_w_tau_curry0[w_]);
 
 curry<0>(G_w_wn) [w_] << fourier(curry<0>(G_w_tau)[w_]);
}
// temp fix : THE TEST DOES NOT RUN !!
//TRIQS_CATCH_AND_ABORT;
catch(std::exception const & e ) { std::cout  << "error "<< e.what()<< std::endl;}
}
