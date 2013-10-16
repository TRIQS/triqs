#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include <triqs/gfs.hpp> 
using namespace triqs::gfs;
using namespace triqs::arrays;
#define TEST(X) std::cout << BOOST_PP_STRINGIZE((X)) << " ---> "<< (X) <<std::endl<<std::endl;
#include <triqs/gfs/local/functions.hpp> 

const int nl_interne = 1000;
const int N = 1000;

typedef double VALUE_TYPE;
//typedef int VALUE_TYPE;
inline VALUE_TYPE fnt(size_t i) { return i*(i+2.0)*(i-8.0);}
//inline VALUE_TYPE fnt(size_t i) { return i;} //*(i+2.0)*(i-8);}
//inline VALUE_TYPE fnt(size_t i) { return i*(i+2.0)*(i-8);}

struct with_sliding_view {
 void operator()() {

 double beta =1;
 auto G =  gf<imfreq> { {beta, Fermion,N}, {2,2}};
 G() =0;

 //auto slv = G.data_getter.slv;

 for (int u =0; u<nl_interne; ++u)
   //for (int i =0; i<N-1; ++i) { G.data_getter.slv[i](0,0) = fnt(i);}
   //for (int i =0; i<N-1; ++i) { slv.set(i); slv(0,0) = fnt(i);}
   //for (int i =0; i<N-1; ++i) G.data_getter(i)(0,0) = fnt(i);
  //for (int i =0; i<N-1; ++i) G.on_mesh(i)(0,0) = fnt(i);
  for (int i =0; i<N-1; ++i) G[i](0,0) = fnt(i);

 }
};

struct array_code {
 void operator()() {

 double beta =1;
 auto G =  gf<imfreq> { {beta, Fermion,N}, {2,2}};
 G() =0;
 auto V = G.data();

 for (int u =0; u<nl_interne; ++u)
   for (int i =0; i<N-1; ++i) V(i,0,0) = fnt(i);

 }
};



#include "./speed_tester.hpp"
int main() {
 try { 
 speed_tester<array_code> (500);
 speed_tester<with_sliding_view> (500);
 //speed_tester<with_slices> (5000);
 }
 TRIQS_CATCH_AND_ABORT;
}

