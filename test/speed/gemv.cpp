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

using namespace std;
using namespace triqs;
using namespace triqs::arrays;

struct gemv_via_binder { 

 triqs::arrays::matrix<double> A;
 typedef triqs::arrays::vector<double> vector_type;
 vector_type  MC, MB;
 static const unsigned int N =200;

 gemv_via_binder(): A(N,N,FORTRAN_LAYOUT), MC(N), MB(N) {
 
  for (int i =0; i<N; ++i)
    for (int j=0; j<N; ++j) 
     A(i,j) = 0.1*(10*i+ j);

   make_view(MC) = 1;

 }

 void operator()() { 
  blas::gemv(1,A, MC, 0, MB);
 }
};

struct expressif : gemv_via_binder  { 
 void operator()() { 
  MB = A * MC;
 }
};


#include "./speed_tester.hpp"
int main() {
 const int l= 1000*1000;
 speed_tester<gemv_via_binder> (l);
 speed_tester<expressif> (l);
 return 0;
}

