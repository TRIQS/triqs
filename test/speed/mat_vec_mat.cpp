#include <triqs/arrays.hpp>

using namespace triqs::arrays;
using namespace triqs;
const int N1= 25; //, N2 = 300;

struct mat_vec {
 void operator()() {
  matrix<double> a (N1,N1), V(N1, N1), f(N1, N1);
  for (int u =0; u<5000; ++u)
   for (int j=0; j<N1; ++j)
  triqs::arrays::blas::gemv(1.0, a, V(j,range()), 0.0,f(j,range()));
 }
};

struct mat_mat {
 void operator()() {
  matrix<double> a (N1,N1), V(N1, N1), f(N1, N1);
  for (int u =0; u<5000; ++u)
    triqs::arrays::blas::gemm(1.0, a, V, 0.0,f);
 }
};


#include "./speed_tester.hpp"
int main() {
 const int l = 200;

 speed_tester<mat_mat> (l);
 speed_tester<mat_vec> (l);

 return 0;
}

