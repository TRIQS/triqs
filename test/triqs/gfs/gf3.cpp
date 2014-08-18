#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include <triqs/gfs.hpp> 
using namespace triqs::gfs;
using namespace triqs::arrays;
namespace h5 = triqs::h5;
#define TEST(X) std::cout << BOOST_PP_STRINGIZE((X)) << " ---> "<< (X) <<std::endl<<std::endl;
#include <triqs/gfs/local/functions.hpp> 

 // example 
 //template<typename T> using block_gf = gf<block_index, gf<T>>;
 // block_gf<imtime> ... 
 // but not on gcc 4.6 !
 //

int main() {
 try { 

  double beta =1;

  auto G =  gf<imfreq>{ {beta, Fermion}, {2,2} };
  auto Gc = gf<imfreq>{ {beta, Fermion}, {2,2} };
  auto G3 = gf<imfreq>{ {beta, Fermion}, {2,2} };

  triqs::clef::placeholder<0> om_;
  
  G(om_) << om_ + 2 - G3(om_);
 
 G = -G3;

 }
 TRIQS_CATCH_AND_ABORT;

}
