//#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK

#include <triqs/gfs/imfreq.hpp> 
#include <triqs/gfs/imtime.hpp> 
#include <triqs/gfs/local/functions.hpp> 

namespace tql= triqs::clef;
namespace tqa= triqs::arrays;
using tqa::range;
using triqs::arrays::make_shape;
using triqs::gfs::Fermion;
using triqs::gfs::imfreq;
using triqs::gfs::imtime;
using triqs::gfs::make_gf;

#define TEST(X) std::cout << BOOST_PP_STRINGIZE((X)) << " ---> "<< (X) <<std::endl<<std::endl;

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
 auto G =  make_gf<imfreq> (beta, Fermion, make_shape(2,2),N);
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
 auto G =  make_gf<imfreq> (beta, Fermion, make_shape(2,2),N);
 G() =0;
 auto V = G.data();

 for (int u =0; u<nl_interne; ++u)
   for (int i =0; i<N-1; ++i) V(0,0,i) = fnt(i);

 }
};



#include "./speed_tester.hpp"
int main() {
 speed_tester<array_code> (5000);
 speed_tester<with_sliding_view> (5000);
 //speed_tester<with_slices> (5000);
 return 0;
}

