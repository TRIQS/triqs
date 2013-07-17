.. highlight:: c

Linear Algebra with matrices and vectors
===============================================

 * Matrices and vectors are interfaced with the blas/lapack library
 * Moreover, for some current operations, simple operations can be programmed efficiently 
   and simply with expression.

Use of special expression
-----------------------------

Code::

 #include <iostream>
 #include "expressions/matmul.hpp"
 using namespace triqs_arrays;
 using Expression::matmul;

 int main(int argc, char **argv) {

  matrix<double> M1(2,2), M2(2,2), M3;
  for (int i =0; i<2; ++i) for (int j=0; j<2; ++j) { M1(i,j) = i+j; M2(i,j) = 1 + i -j ; }

  // The central instruction : note that matmul returns a lazy object 
  // that has ImmutableArray interface, and defines a specialized version assignment
  // As a result this is equivalent to matmul_with_lapack(M1,M2,M3) : there is NO intermediate copy.
  M3 = matmul(M1,M2);

  std::cout<<"M3 = "<<M3<<std::endl;
 }
 

General blas-Lapack interface for matrices and vectors
-------------------------------------------------------------------


* matrix, vector and their views are interfaced with blas/lapack, via boost::numerics::bindings.
* If needed (for a view), a temporary (and silent) copy is made to reorganize the 
  data before calling blas/lapack. Of course, performance is altered, but code is simple...
* TO DO : add a compile warning for such performance hits...

Examples::

 namespace blas   = boost::numeric::bindings::blas;
 namespace lapack = boost::numeric::bindings::lapack;

 triqs_arrays::vector<std::complex<double> >  V(5),V2(5);
 triqs_arrays::vector <double> V3(2);
 triqs_arrays::matrix<double,'F' > M1(2,2), M2(2,2), M3(2,2);

 blas::axpy(2.0,V,V2);
 blas::gemm(1.0,M1, M2, 1.0, M3);
 blas::ger(1.0,V3,V3,M2);

 // invert
 triqs_arrays::vector <int> ipiv(2);
 lapack::getrf(M1, ipiv);
 lapack::getri(M1, ipiv);

