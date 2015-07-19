
#include <triqs/gfs.hpp>
#define TEST(X) std::cout << BOOST_PP_STRINGIZE((X)) << " ---> "<< (X) <<std::endl<<std::endl;

using namespace triqs::gfs;

int main(int argc, char* argv[]) {

 try { 

 double beta = 10.0;

 auto A = make_block_gf<imfreq> ( {"up","down"}, {gf<imfreq>{ {beta, Fermion}, {1,1} },gf<imfreq>{ {beta, Fermion}, {1,1} }}); 
 auto B = A;
 auto C = A;

 C = A + A * B;
 C() = A + A() * B();

 TEST( A[0](0) ) ;
 } 
 TRIQS_CATCH_AND_ABORT;

} 
