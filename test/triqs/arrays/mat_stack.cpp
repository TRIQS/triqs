#include "./common.hpp"
#include <triqs/arrays/matrix_stack_view.hpp>

using namespace triqs::arrays;
using namespace triqs;

int main() {

 clef::placeholder<0> i_;
 clef::placeholder<1> j_;
 clef::placeholder<2> k_;

 {
  std::cerr<< "------------- Testing inversion  ---------------"<< std::endl;
  array <double,3> A(4,2,2);

  A(k_,i_,j_) << clef::if_else(i_ == j_, i_ + k_+1, 0); // + 2.3* j_;

  matrix_stack_view<double> S (std::move(A));

  std::cout  << S(0) << S(1)<< S(2) << std::endl ;

  S.invert();
  std::cout  << S(0) << S(1)<< S(2)<< std::endl ;
 }

 {
  std::cerr<< "------------- Testing += matrix ---------------"<< std::endl;
  array <double,3> A(4,2,3);
  matrix<double> M(2,3); M()= 1;

  A(k_,i_,j_) << i_ + 2*j_ + k_/2.0;

  matrix_stack_view<double> S (std::move(A));

  std::cout  << S(0) << S(1)<< S(2) << std::endl ;

  S += M;
  std::cout  << S(0) << S(1)<< S(2)<< std::endl ;
 }

 {
  std::cerr<< "------------- Testing += me---------------"<< std::endl;
  array <double,3> A(4,2,3);
  matrix<double> M(2,3); M()= 1;

  A(k_,i_,j_) << i_ + 2*j_ + k_/2.0;

  matrix_stack_view<double> S (std::move(A));

  std::cout  << S(0) << S(1)<< S(2) << std::endl ;

  S += S;
  std::cout  << S(0) << S(1)<< S(2)<< std::endl ;
 }
{
  std::cerr<< "------------- Testing *= 2---------------"<< std::endl;
  array <double,3> A(4,2,3);
  matrix<double> M(2,3); M()= 1;

  A(k_,i_,j_) << i_ + 2*j_ + k_/2.0;

  matrix_stack_view<double> S (std::move(A));

  std::cout  << S(0) << S(1)<< S(2) << std::endl ;

  S *= 2;
  std::cout  << S(0) << S(1)<< S(2)<< std::endl ;
 }


{
  std::cerr<< "------------- Testing R, L mul  ---------------"<< std::endl;
  array <double,3> A(4,3,3);
  matrix<double> L(2,3); L()= 2;
  matrix<double> R(3,2); R()= 3;

  A(k_,i_,j_) << i_ + 2*j_ + k_/2.0;

  matrix_stack_view<double> S (std::move(A));

  std::cout  << S(0) << S(1)<< S(2) << std::endl ;

  auto Sb = matmul_L_R(L,S,R);
  std::cout  << Sb(0) << Sb(1)<< Sb(2)<< std::endl ;
  std::cout  << S(0) << std::endl ;
 }

}
