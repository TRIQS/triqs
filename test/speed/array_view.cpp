//#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include <triqs/arrays.hpp>
#include <triqs/arrays/matrix_view_proxy.hpp>

using namespace triqs::arrays;

#define TEST(X) std::cout << BOOST_PP_STRINGIZE((X)) << " ---> "<< (X) <<std::endl<<std::endl;

const int nl_interne = 100000;
const int N = 1000;

typedef double VALUE_TYPE;
//typedef int VALUE_TYPE;
inline VALUE_TYPE fnt(size_t i) { return i*(i+2.0)*(i-8.0);}
//inline VALUE_TYPE fnt(size_t i) { return i;} //*(i+2.0)*(i-8);}
//inline VALUE_TYPE fnt(size_t i) { return i*(i+2.0)*(i-8);}

struct with_view {
 void operator()() {

 array<double,3> A(N,2,2);
 A() =0;

 for (int u =0; u<nl_interne; ++u)
  for (int i =0; i<N-1; ++i) A(i,range(), range()) (0,0) = fnt(i);

 }
};

struct with_proxy {
 void operator()() {

 array<double,3> A(N,2,2);
 A() =0;

 for (int u =0; u<nl_interne; ++u)
  for (int i =0; i<N-1; ++i) matrix_view_proxy<array<double,3>, 0>(A,i) (0,0) = fnt(i);

 }
};

struct direct {
 void operator()() {

 array<double,3> A(N,2,2);
 A() =0;

 for (int u =0; u<nl_interne; ++u)
   for (int i =0; i<N-1; ++i) A(i,0,0) = fnt(i);

 }
};


struct direct2 {
 void operator()() {

 array<double,1> A(N);
 A() =0;

 for (int u =0; u<nl_interne; ++u)
   for (int i =0; i<N-1; ++i) A(i) = fnt(i);

 }
};


#include "./speed_tester.hpp"
int main() {
 speed_tester<direct> (50);
 speed_tester<with_proxy> (50);
 speed_tester<with_view> (50);
 speed_tester<direct2> (50);
 //speed_tester<with_slices> (5000);
 return 0;
}

