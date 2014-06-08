#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include <triqs/gfs.hpp> 
using namespace triqs::gfs;
using namespace triqs::arrays;
namespace h5 = triqs::h5;
#define TEST(X) std::cout << BOOST_PP_STRINGIZE((X)) << " ---> "<< (X) <<std::endl<<std::endl;
#include <triqs/gfs/local/fourier_matsubara.hpp> 

int main() {
 
 double precision=10e-9;
 h5::file file("test_fourier_matsubara.h5",H5F_ACC_TRUNC);
 triqs::clef::placeholder<0> om_;
 double beta =1;
 int N=10000;
 double E=1;
 
 auto Gw1 = gf<imfreq> {{beta, Fermion, N}, {1,1}};
 Gw1(om_) << 1/(om_-E);
 h5_write(file, "Gw1", Gw1);   // the original lorentzian
 
 auto Gt1 = gf<imtime> {{beta, Fermion, N}, {1,1}};
 Gt1() = inverse_fourier(Gw1);
 h5_write(file, "Gt1", Gt1);   // the lorentzian TF : lorentzian_inverse
 
 ///verification that TF(TF^-1)=Id
 auto Gw1b = gf<imfreq> {{beta, Fermion, N}, {1,1}};
 Gw1b() = fourier(Gt1);
 for(auto const& w:Gw1.mesh()){
  if ( std::abs(Gw1b[w](0,0)-Gw1[w](0,0)) > precision) TRIQS_RUNTIME_ERROR<<" fourier_matsubara error : w="<<std::complex<double>(w)<<" ,Gw1b="<<std::abs(Gw1b[w](0,0))<<"\n";
 }
 h5_write(file,"Gw1b",Gw1b); // must be 0
 
 ///verification that the TF is OK
 for(auto const & t:Gt1.mesh()){
  Gt1[t]-= exp(-E*t) * ( (t>0?-1:0)+1/(1+exp(E*beta)) );
  if ( std::abs(Gt1[t](0,0)) > precision) TRIQS_RUNTIME_ERROR<<" fourier_matsubara error : t="<<t<<" ,G1="<<std::abs(Gt1[t](0,0))<<"\n";
 }
 h5_write(file,"Gt1b",Gt1); // must be 0
 
 ///to verify that fourier computes
 auto Gw2 = gf<imfreq> {{beta, Fermion}, {1,1}};
 Gw2() = fourier(Gt1);
 
}


