//#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK

#include <triqs/gf/refreq.hpp> 
#include <triqs/gf/retime.hpp> 

namespace tql= triqs::clef;
namespace tqa= triqs::arrays;
using tqa::range;
using triqs::arrays::make_shape;
using triqs::gfs::scalar_valued;
using triqs::gfs::Fermion;
using triqs::gfs::refreq;
using triqs::gfs::retime;
using triqs::gfs::make_gf;

#define TEST(X) std::cout << BOOST_PP_STRINGIZE((X)) << " ---> "<< (X) <<std::endl<<std::endl;

int main() {

 double beta =1;
 double tmax=10;
 int N=1000;
 auto Gw = make_gf<refreq> (-10, 10, N, make_shape(2,2));
 auto Gt = make_gf<retime> (0, tmax, N, make_shape(2,2));
 auto Gw2 = make_gf<refreq,scalar_valued> (-10, 10, N);
 auto Gt2 = make_gf<retime,scalar_valued> (0, tmax, N);

 int i =0;
 for (auto & t : Gt.mesh()) Gt(t) = 1.0*t;
 for (auto & w : Gw.mesh()) Gw(w) = 1.0*w;
 
 triqs::clef::placeholder<0> t_;
 triqs::clef::placeholder<1> w_;
 Gt2( t_) << 1.0*t_;
 Gw2( w_) << 1.0*w_;
 
 std::cout << Gt(3.255)(0,0) << std::endl; 
 std::cout << Gt2(3.255) << std::endl; 
 std::cout << Gw(3.255)(0,0) << std::endl; 
 std::cout << Gw2(3.255) << std::endl; 
 
 // test hdf5 
 H5::H5File file("ess_gfre.h5", H5F_ACC_TRUNC );
 h5_write(file, "gt",  Gt);
 h5_write(file, "gw",  Gw);

}
