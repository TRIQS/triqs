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
#include <triqs/arrays/matrix.hpp>
#include <triqs/arrays/array.hpp>

using namespace triqs::arrays;
using namespace triqs;
const int N1= 200, N2 = 300;
namespace tqa=triqs::arrays; namespace tql=triqs::clef;

struct plain {
 void operator()() { 
  triqs::arrays::array<double,2> A (N1,N2,FORTRAN_LAYOUT);
  for (int u =0; u<5000; ++u)
  {
   for (int j=0; j<N2; ++j) 
    for (int i =0; i<N1; ++i)
     A(i,j) = i+ 2*j;
  }
 }
};

struct lazy {
 void operator()() {
  tql::placeholder<0> i_;   tql::placeholder<1> j_;  
  triqs::arrays::array<double,2> A (N1,N2,FORTRAN_LAYOUT);
  for (int u =0; u<5000; ++u)
   A(i_,j_) << i_+ 2*j_;
 }
};

struct foreach_lambda {
 void operator()() {
  triqs::arrays::array<double,2> A (N1,N2,FORTRAN_LAYOUT);
  for (int u =0; u<5000; ++u)
   //assign_foreach(A, [](size_t i, size_t j) { return i + 2.0*j;});
   assign_foreach(A, [](long i, long j) { return i + 2.0*j;});
 }
};

#include "./speed_tester.hpp"
int main() {
 const int l = 100;
 speed_tester<plain> (l);
 speed_tester<lazy> (l);
 speed_tester<foreach_lambda> (l);
 return 0;
}

