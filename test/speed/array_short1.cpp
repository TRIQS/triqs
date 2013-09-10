/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011 by O. Parcollet
 *
 * TRIQS is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * TRIQS. If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/
#include <triqs/arrays.hpp>
#include <triqs/arrays/linalg/det_and_inverse.hpp>

using namespace triqs::arrays;
using namespace triqs;
const int N1= 10, N2 = 10;

struct plain_for_no_ptr_const {
 void operator()() {
  triqs::arrays::array<double,2> A (N1,N2,FORTRAN_LAYOUT);
  for (int u =0; u<2500000; ++u)
   for (int j=0; j<N2; ++j)
    for (int i =0; i<N1; ++i)
     A(i,j) = 1876;
 }
};

struct assign_to_const {
 void operator()() {
  triqs::arrays::array<double,2,TRAVERSAL_ORDER_FORTRAN> A (N1,N2,FORTRAN_LAYOUT);
  auto V = make_view(A);
  for (int u =0; u<2500000; ++u)
   //make_view(A) = 1867;
   V= 1867;
 }
};

struct plain_for_no_ptr {
 void operator()() {
  triqs::arrays::matrix<double > A (N1,N2, FORTRAN_LAYOUT);
  for (int u =0; u<2500000; ++u)
   for (int i =0; i<N1; ++i)
    for (int j=0; j<N2; ++j)
     A(i,j) = 10*i+ j;
 }
};

//typedef double value_type;
//typedef triqs::arrays::matrix<double>::indexmap_type::domain_type::index_value_type index_value_type;
struct F {
 triqs::arrays::matrix<double,TRAVERSAL_ORDER_FORTRAN > & a;
 F(triqs::arrays::matrix<double,TRAVERSAL_ORDER_FORTRAN > & a_): a(a_){}
 //void operator()(value_type & p, index_value_type const & key) const { p=key[0]*10 + key[1] ;}
 //void operator()(value_type & p, value_type const & x0, value_type const & x1) const { p=x0*10 + x1 ;}
 template<typename X0, typename X1> void operator()( X0 const & x0, X1 const & x1) const { a(x0,x1) =x0*10 + x1 ;}
};

struct with_foreach  {
 void operator()() {
  triqs::arrays::matrix<double,TRAVERSAL_ORDER_FORTRAN > A (N1,N2,FORTRAN_LAYOUT);
  // triqs::arrays::indexmaps::
  for (int u =0; u<2500000; ++u) foreach(A,F(A));
  // for (int u =0; u<5000; ++u)    make_view(A) = 1867;
 }
};

struct iterators {
 void operator()() {
  typedef triqs::arrays::matrix<double > MM;
  MM A (N1,N2,FORTRAN_LAYOUT);
  for (int u =0; u<3000; ++u)
  {
   for (auto it = A.begin(); it; ++it) {
    *it =10*it.indices()[0] + it.indices()[1] ;
   }
  }
 }
};

template<class int_type>
struct pointer_generic_form {
 void operator()() {
  triqs::arrays::matrix<double > A (N1,N2, FORTRAN_LAYOUT);
  const int_type s0 = A.indexmap().strides()[0];
  const int_type s1 = A.indexmap().strides()[1];

  const int_type l0 = A.indexmap().lengths()[0];
  const int_type l1 = A.indexmap().lengths()[1];
  for (int u =0; u<1000; ++u) {
   double * restrict p= &(A(0,0));
   //typedef int int_type;

   //assert(s0==1); assert(s1==N1);
   //assert(l0==N1); assert(l1==N2);
   for (int_type i0 =0; i0<l0; ++i0)
    for (int_type i1 =0; i1<l1; ++i1)
     p[i0*s0 + i1*s1] = 10*i0+ i1;

  }
 }
};



struct const_with_iterators {
 void operator()() {
  typedef triqs::arrays::matrix<double> MM;
  MM A (N1,N2,FORTRAN_LAYOUT);
  for (int u =0; u<5000; ++u)
  {
   for (MM::iterator it = A.begin(); it; ++it) {
    *it = 1876;
   }
  }
 }
};


struct foreach_manual_simple {
 void operator()() {
  triqs::arrays::array<double,2> A (N1,N2,FORTRAN_LAYOUT);
  mini_vector<int,2> t;
  const int u[2] = {1,0}; //=1, u1=0;
  //int u[2]; for (int i=0; i<2; ++i) u[i] = 1-i;
  for (int k =0; k<5000; ++k)
   for (t[u[0]]=0; t[u[0]]<A.shape(u[0]); ++t[u[0]])
    for (t[u[1]] =0; t[u[1]]<A.shape(u[1]); ++t[u[1]])
     A(t[u[1]],t[u[0]]) = 1876;
  //A[t] = 1876;
 }
};



#include "./speed_tester.hpp"
int main() {
 const int l = 100;

 std::cout  << "----------- Comparing const assign : A() =const to a manual loop --------------------"<< std::endl ;

 speed_tester<plain_for_no_ptr_const> (l);
 speed_tester<assign_to_const> (l);

 std::cout  << "----------- Comparing manual loop and foreach for A(i,j) = 10 *i + j--------------------"<< std::endl ;
 speed_tester<plain_for_no_ptr> (l);
 speed_tester<with_foreach> (l);
 //speed_tester<pointer_generic_form <std::ptrdiff_t> > (l);
 //speed_tester<pointer_generic_form < size_t> > (l);

 //speed_tester<foreach_manual_simple>(l);
 std::cout  << "----------- Comparing manual loop and foreach for A(i,j) = 10 *i + j--------------------"<< std::endl ;

 speed_tester<iterators> (l);
 speed_tester<const_with_iterators> (l);
 return 0;
}

