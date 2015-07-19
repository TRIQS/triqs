#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include <triqs/gfs.hpp> 
using namespace triqs::gfs;
#define TEST(X) std::cout << BOOST_PP_STRINGIZE((X)) << " ---> "<< (X) <<std::endl<<std::endl;

int main() {

 double beta =1;
 auto G =  gf<imfreq> {{beta, Fermion}, {2,2}};
 auto Gc = G;
 auto G3 = G;
 auto Gt = gf<imtime> {{beta, Fermion, 3000}, {2,2}};

 auto gt = make_gf_from_inverse_fourier(G);
 auto gw = make_gf_from_fourier(gt);

 //gw() = fourier(gt);
 G() = fourier(Gt);
}


