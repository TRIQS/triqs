//#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK

#include <triqs/arrays.hpp>

using namespace triqs::arrays;

#define TEST(X) std::cout << BOOST_PP_STRINGIZE((X)) << " ---> "<< (X) <<std::endl<<std::endl;

const int L = 20;
const int nl_interne = 2000;
const int N = 1000;

struct matmul1 {
 void operator()() {

  matrix<double> A(L,L), B(L,L),C(L,L);
  A() = 0.01; B() = 1.2; C() = 0;

  for (int u =0; u<nl_interne; ++u){
   C = A*B;
  }
 }
};


struct matmul2 {
 void operator()() {

  matrix<double> A(L,L), B(L,L),C(L,L);
  A() = 0.01; B() = 1.2; C() = 0;

  for (int u =0; u<nl_interne; ++u){
   A = make_clone(A)*B;
  }
 }
};

struct gemm_direct {
 void operator()() {

  matrix<double> A(L,L), B(L,L),C(L,L);
  A() = 0.01; B() = 1.2; C() = 0;

  for (int u =0; u<nl_interne; ++u){
   blas::gemm(1.0, A, B, 0.0, C);
  }
 }
};

struct matmul_2AB {
 void operator()() {
  matrix<double> A(L,L), B(L,L),C(L,L);
  A() = 0.01; B() = 1.2; C() = 0;
  for (int u =0; u<nl_interne; ++u){
   C = 2*A*B;
  }
 }
};

struct gemm_direct_2AB {
 void operator()() {
  matrix<double> A(L,L), B(L,L),C(L,L);
  A() = 0.01; B() = 1.2; C() = 0;
  for (int u =0; u<nl_interne; ++u){
   blas::gemm(2.0, A, B, 0.0, C);
  }
 }
};

#include "./speed_tester.hpp"
int main() {
 speed_tester<matmul1> (5000);
 speed_tester<matmul2> (5000);
 speed_tester<gemm_direct> (5000);
 speed_tester<matmul_2AB> (5000);
 speed_tester<gemm_direct_2AB> (5000);
 return 0;
}

