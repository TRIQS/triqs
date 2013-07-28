//#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK

#include <triqs/gf/imfreq.hpp> 
#include <triqs/gf/imtime.hpp> 
#include <triqs/gf/local/fourier_matsubara.hpp> 

namespace tql= triqs::clef;
// namespace tqa= triqs::arrays;
// using tqa::range;
using triqs::arrays::make_shape;
using triqs::gfs::Fermion;
using triqs::gfs::imfreq;
using triqs::gfs::imtime;
using triqs::gfs::make_gf;
using triqs::arrays::range;
#define TEST(X) std::cout << BOOST_PP_STRINGIZE((X)) << " ---> "<< (X) <<std::endl<<std::endl;

int main() {
 
 double precision=10e-9;
 H5::H5File file("test_fourier_matsubara.h5",H5F_ACC_TRUNC);
 triqs::clef::placeholder<0> om_;
 double beta =1;
 int N=10000;
 double E=1;
 
 auto Gw1 = make_gf<imfreq> (beta, Fermion, make_shape(1,1), N);
 Gw1(om_) << 1/(om_-E);
//  for(auto const& w:Gw1.mesh()){
//   std::cout<<"w="<<std::complex<double>(w)<<", Gw1=" << Gw1(w)(0,0)<<std::endl;
//  }
 h5_write(file, "Gw1", Gw1);   // the original lorentzian
 
 auto Gt1 = make_gf<imtime> (beta, Fermion, make_shape(1,1), N);
 inverse_fourier_impl( Gt1, Gw1, triqs::gfs::matrix_valued() );
//  for(auto const& t:Gt1.mesh()){
//   std::cout<<"t="<<t<<",  expected="<<exp(-E*t) * ( (t>0?-1:0)+1/(1+exp(E*beta)) )<<std::endl;
//  }
 h5_write(file, "Gt1", Gt1);   // the lorentzian TF : lorentzian_inverse
 
 ///verification that TF(TF^-1)=Id
 auto Gw1b = make_gf<imfreq> (beta, Fermion, make_shape(1,1), N);
 fourier_impl(Gw1b, Gt1, triqs::gfs::matrix_valued());
 for(auto const& w:Gw1.mesh()){
//   std::cout<<"w="<<std::complex<double>(w)<<",Gw1b=" << Gw1b(w)(0,0)<<std::endl;
//   std::cout<<"w="<<std::complex<double>(w)<<",Delta Gw1b=" << Gw1b(w)(0,0)-Gw1(w)(0,0)<<std::endl;
  if ( std::abs(Gw1b(w)(0,0)-Gw1(w)(0,0)) > precision) TRIQS_RUNTIME_ERROR<<" fourier_matsubara error : w="<<std::complex<double>(w)<<" ,Gw1b="<<std::abs(Gw1b(w)(0,0))<<"\n";
 }
 h5_write(file,"Gw1b",Gw1b); // must be 0
 
 ///verification that the TF is OK
 for(auto const & t:Gt1.mesh()){
  Gt1(t)-= exp(-E*t) * ( (t>0?-1:0)+1/(1+exp(E*beta)) );
  if ( std::abs(Gt1(t)(0,0)) > precision) TRIQS_RUNTIME_ERROR<<" fourier_matsubara error : t="<<t<<" ,G1="<<std::abs(Gt1(t)(0,0))<<"\n";
 }
 h5_write(file,"Gt1b",Gt1); // must be 0
 
 ///to verify that lazy_fourier computes
 auto Gw2 = make_gf<imfreq> (beta, Fermion, make_shape(1,1));
 Gw2() = lazy_fourier(Gt1);
 
}


