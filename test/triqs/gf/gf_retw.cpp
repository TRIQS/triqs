//#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK

#include <triqs/gf/refreq.hpp> 
#include <triqs/gf/retime.hpp> 

namespace tql= triqs::clef;
namespace tqa= triqs::arrays;
using tqa::range;
using triqs::arrays::make_shape;
using triqs::gf::scalar_valued;
using triqs::gf::Fermion;
using triqs::gf::refreq;
using triqs::gf::retime;
using triqs::gf::make_gf;
double precision=10e-12;

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
 
 std::cout << Gt(0.3352*tmax)(0,0) << std::endl; 
 if ( std::abs(Gt(0.3352*tmax)(0,0)-0.3352*tmax) > precision) TRIQS_RUNTIME_ERROR<< "evaluator error in Gt\n";  
 std::cout << Gt2(0.3352*tmax) << std::endl; 
 if ( std::abs(Gt2(0.3352*tmax)-0.3352*tmax) > precision) TRIQS_RUNTIME_ERROR<< "evaluator error in Gt2\n";  
 std::cout << Gw(0.3352*beta)(0,0) << std::endl; 
 if ( std::abs(Gw(0.3352*beta)(0,0)-0.3352*beta) > precision) TRIQS_RUNTIME_ERROR<< "evaluator error in Gw\n";  
 std::cout << Gw2(0.3352*beta) << std::endl; 
 if ( std::abs(Gw2(0.3352*beta)-0.3352*beta) > precision) TRIQS_RUNTIME_ERROR<< "evaluator error in Gw2\n";  
 std::cout << Gw2.on_mesh(N/3) << std::endl; 
 std::cout << Gw2[N/3] << std::endl; 
 if ( std::abs(Gw2.on_mesh(N/3)-Gw2[N/3]) > precision) TRIQS_RUNTIME_ERROR<< "error in on_mesh()\n";  
 if ( std::abs(Gt2.on_mesh(N/3)-Gt2[N/3]) > precision) TRIQS_RUNTIME_ERROR<< "error in on_mesh()\n";  
 
 // test hdf5 
 H5::H5File file("ess_gfre.h5", H5F_ACC_TRUNC );
 h5_write(file, "gt",  Gt);
 h5_write(file, "gw",  Gw);

}
