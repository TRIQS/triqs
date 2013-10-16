#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include <triqs/gfs.hpp> 
using namespace triqs::gfs;
using namespace triqs::arrays;
#define TEST(X) std::cout << BOOST_PP_STRINGIZE((X)) << " ---> "<< (X) <<std::endl<<std::endl;
#include <triqs/gfs/local/fourier_real.hpp> 

double lorentzian(double w, double a){
  return 2*a / (w*w + a*a) ; 
};
std::complex<double> lorentzian_inverse(double t, double a){
  return std::exp(-a*std::abs(t)) ; 
};
double theta(double x){
  return x>0 ? 1.0 : ( x<0 ? 0.0 : 0.5 ) ; 
};

int main() {
 
  double precision=10e-10;
  H5::H5File file("fourier_real_time.h5",H5F_ACC_TRUNC);

  std::complex<double> I(0,1);  
  
  //Test on the tail: GF in frequency that is a lorentzian, with its singularity, TF and TF^-1. 
  
  double wmax=10;
  int Nw=1001;
  
  auto Gw1 = gf<refreq> {{-wmax, wmax, Nw,full_bins}, {1,1}};
  double a = Gw1.mesh().delta() * sqrt( Gw1.mesh().size() );
  for(auto const & w:Gw1.mesh()) Gw1[w]=lorentzian(w,a);
  Gw1.singularity()(2)=triqs::arrays::matrix<double>{{2.0*a}};
  h5_write(file,"Gw1",Gw1);   // the original lorentzian
  
  auto Gt1 = inverse_fourier(Gw1);  
  h5_write(file,"Gt1",Gt1);   // the lorentzian TF : lorentzian_inverse
  
  // verification that TF(TF^-1)=Id
  auto Gw1b = fourier(Gt1);
  for(auto const & w:Gw1b.mesh()){
    Gw1b[w]-=Gw1[w];
    if ( std::abs(Gw1b[w](0,0)) > precision) TRIQS_RUNTIME_ERROR<<" fourier_real_time error : w="<<w<<" ,G1="<<std::abs(Gw1b[w](0,0))<<"\n";
  }
  h5_write(file,"Gw1b",Gw1b); // must be 0
  
  // verification that TF is the lorentzian_inverse function
  for(auto const & t:Gt1.mesh()){
    Gt1[t]-=lorentzian_inverse(t,a);
    if ( std::abs(Gt1[t](0,0)) > precision) TRIQS_RUNTIME_ERROR<<" fourier_real_time error : t="<<t<<" ,G1="<<std::abs(Gt1[t](0,0))<<"\n";
  }
  h5_write(file,"Gt1b",Gt1); // must be 0
  
  
  
  //Test on the tail: GF in time that is a decreasing exponential 
  
  double tmax=10.;
  int Nt=501;
  
  auto Gt2 = gf<retime> {{-tmax, tmax, Nt}, {1,1}};
  a = 2*acos(-1.) / ( Gt2.mesh().delta() *sqrt( Gt2.mesh().size() ) );
  for(auto const & t:Gt2.mesh()) Gt2[t] = 0.5 *I * ( lorentzian_inverse(-t,a)*theta(-t)-lorentzian_inverse(t,a)*theta(t) );
  //for(auto const & t:Gt2.mesh()) Gt2[t] = 0.5_j * ( lorentzian_inverse(-t,a)*theta(-t)-lorentzian_inverse(t,a)*theta(t) );
  Gt2.singularity()(1)=triqs::arrays::matrix<double>{{1.0}};
  h5_write(file,"Gt2",Gt2);
  
  auto Gw2 = fourier(Gt2);
  h5_write(file,"Gw2",Gw2);
  
  for(auto const & w:Gw2.mesh()){
    Gw2[w]-= 0.5/(w+a*I)+0.5/(w-a*I);
    //Gw2[w]-= 0.5/(w+a*1_j)+0.5/(w-a*1_j);
    if ( std::abs(Gw2[w](0,0)) > precision) TRIQS_RUNTIME_ERROR<<" fourier_real_time error : w="<<w<<" ,G2="<<std::abs(Gw2[w](0,0))<<"\n";
  }
  h5_write(file,"Gw2b",Gw2);

  
  
  //Test : GF in time is a simple trigonometric function, the result is a sum of Dirac functions 
  
  tmax=4*acos(-1.);
  
  auto Gt3 = gf<retime> {{-tmax, tmax, Nt}, {1,1}};
  for(auto const & t:Gt3.mesh()) Gt3[t] = 1.0 * std::cos(10*t) + 0.25*std::sin(4*t) + 0.5 * I*std::sin(8*t+0.3*acos(-1.)) ;
  //for(auto const & t:Gt3.mesh()) Gt3[t] = 1.0 * std::cos(10*t) + 0.25*std::sin(4*t) + 0.5_j*std::sin(8*t+0.3*acos(-1.)) ;
  h5_write(file,"Gt3",Gt3);
  
  auto Gw3 = fourier(Gt3);
  h5_write(file,"Gw3",Gw3);
  
}


