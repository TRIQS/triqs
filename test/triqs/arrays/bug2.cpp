#include <triqs/arrays.hpp>

using namespace triqs::arrays;

int main() {

  typedef std::complex<double> dcomplex;

  array<double,3> A(10,2,2); A() = 0;

  A(4,range(),range()) = 1;
  A(5,range(),range()) = 2;

  matrix_view<double> M1 = A(4,range(),range());
  matrix_view<double> M2 = A(5,range(),range());

  std::cout << M1 << std::endl;
  std::cout << M2 << std::endl;
  
  std::cout << M1.data_start() << std::endl;
  std::cout << M2.data_start() << std::endl;


  M1 = M2;
  //M1 = M1*M2;

  // aie! M1 is zero!
  std::cout << M1 << std::endl;
  std::cout << M2 << std::endl;

}


