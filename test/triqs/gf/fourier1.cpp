//#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK

#include <triqs/gfs/imfreq.hpp> 
#include <triqs/gfs/imtime.hpp> 
#include <triqs/gfs/local/fourier_matsubara.hpp> 

namespace tql= triqs::clef;
namespace tqa= triqs::arrays;
using tqa::range;
using triqs::arrays::make_shape;
using triqs::gfs::Fermion;
using triqs::gfs::imfreq;
using triqs::gfs::imtime;
using triqs::gfs::make_gf;

#define TEST(X) std::cout << BOOST_PP_STRINGIZE((X)) << " ---> "<< (X) <<std::endl<<std::endl;

int main() {

 triqs::gfs::freq_infty inf;

 double beta =1;
 auto G =  make_gf<imfreq> (beta, Fermion, make_shape(2,2));
 auto Gc = make_gf<imfreq> (beta, Fermion, make_shape(2,2));
 auto G3 = make_gf<imfreq> (beta, Fermion, make_shape(2,2));
 auto Gt = make_gf<imtime> (beta, Fermion, make_shape(2,2));

 auto gt = inverse_fourier(G);
 auto gw = fourier(gt);

 //gw() = lazy_fourier(gt);
 G() = lazy_fourier(Gt);
}


