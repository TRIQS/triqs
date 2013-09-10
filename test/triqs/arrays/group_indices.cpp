#include "./common.hpp"
#include <triqs/arrays/indexmaps/cuboid/group_indices.hpp>
#include <triqs/arrays/matrix.hpp>
#include <triqs/arrays/asserts.hpp>

namespace tqa=triqs::arrays;
namespace tql=triqs::clef;

using tqa::m_index;

template<triqs::ull_t FLAG> void test() { 

 tql::placeholder<0> i_;
 tql::placeholder<1> j_;
 tql::placeholder<2> k_;
 tql::placeholder<3> l_;

 { // a simple test
  tqa::array<int,4,FLAG> A(2,2,2,2); 
  A(i_,j_,k_,l_) << i_ + 10*j_ + 100*k_ + 1000*l_;
  TEST(A);
  TEST( group_indices(A,  m_index<0,1>(), m_index<2,3>())); 
 }

 { // more complex : inversing a tensor product of matrices...
  tqa::matrix<double,FLAG> B(2,2), C(3,3), Binv, Cinv;
  C(i_,j_) << 1.7 /( 3.4*i_ - 2.3*j_ + 1) ;
  B(i_,j_) << 2*i_ + j_;
  TEST(B); TEST(C);
  Binv = inverse(B);
  Cinv = inverse(C);
  TEST(Binv); TEST(Cinv);

  tqa::array<double,4,FLAG> A(2,3,2,3); 
  A(i_,j_,k_,l_) << B(i_,k_) * C(j_,l_);	

  TEST(A);

  tqa::matrix_view<double,FLAG> M = group_indices (A, m_index<0,1>() , m_index<2,3>() );
  M = inverse(M);

  // checking 
  tqa::array<double,4,FLAG> R(A.shape());
  R(i_,j_,k_,l_) << Binv(i_,k_) * Cinv(j_,l_);	

  TEST(R);
  TEST(A);
  assert_all_close(R,A,5.e-15);
  //TEST(make_array(R-A));
  //TEST( max(abs(R-A)));
 }

}

int main () {
 test<TRAVERSAL_ORDER_FORTRAN>();
 test<TRAVERSAL_ORDER_C>();

 // template dans le namespace clef pour ADL et enable sur le concept de A ...
 // an alternative syntax ? 
 //auto V = group_indices( A(i_,j_,k_,l_), var(i_,j_), var(k_,l_));
 // match A(...), deduce the int of hte ph, and make a finder for a ph in it
 // finder<Expr>::find<i>::value --> m_index<...>
}
