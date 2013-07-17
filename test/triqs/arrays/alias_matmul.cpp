#include <triqs/arrays.hpp>

using namespace triqs::arrays;

int main() {

  typedef std::complex<double> dcomplex;

  array<dcomplex,3> A(10,2,2); A() = -1;

  A(4,range(),range()) = 1;
  A(5,range(),range()) = 2;

  matrix_view<dcomplex> M1 = A(4,range(),range());
  matrix_view<dcomplex> M2 = A(5,range(),range());

  std::cout << M1 << std::endl;
  std::cout << M2 << std::endl;

  M1 = M1*M2;

  // aie! M1 is zero!
  std::cout << M1 << std::endl;
  std::cout << M2 << std::endl;

  matrix<double> B1(2,2), B2(2,2);
  B1() = 2; B2() = 3;

  std::cout << "----------" << std::endl;
  std::cout << B1 << std::endl;
  std::cout << B2 << std::endl;
  
  //make_cache(B1).view() = B1 * B2;
  //B1 = matrix<double>(B1) * B2;
  B1 = make_clone(B1) * B2;

  std::cout << B1 << std::endl;

}

