//#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK

#include <triqs/gf/refreq.hpp> 
#include <triqs/gf/retime.hpp> 

namespace tql= triqs::clef;
namespace tqa= triqs::arrays;
using tqa::range;
using triqs::arrays::make_shape;
using triqs::gf::Fermion;
using triqs::gf::refreq;
using triqs::gf::retime;
using triqs::gf::make_gf;

#define TEST(X) std::cout << BOOST_PP_STRINGIZE((X)) << " ---> "<< (X) <<std::endl<<std::endl;

int main() {

 double beta =1;
 auto G =  make_gf<refreq> (-10, 10, 1000, make_shape(2,2));
 auto Gt = make_gf<retime> (0, 10, 1000, make_shape(2,2));
 auto Gt2 = make_gf<retime> (0, 10, 1000, make_shape(2,2));

 int i =0;
 for (auto & t : Gt.mesh()) Gt(t) = 1.0*t*t;
 
 triqs::clef::placeholder<0> t_;
 Gt2( t_)  << 2* t_; // * t_;

 std::cout << Gt(3.255) << std::endl; 
 //std::cout << Gt(3.2) << std::endl; 
 // test hdf5 
 H5::H5File file("ess_gfre.h5", H5F_ACC_TRUNC );
 h5_write(file, "gt", Gt);
 h5_write(file, "gt2", Gt2);

}
