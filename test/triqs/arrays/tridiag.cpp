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
#include "./common.hpp"
#include "./src/linalg/eigenelements.hpp"
#include "./src/blas_lapack/stev.hpp"
using namespace triqs::arrays;
using namespace triqs;

int main(int argc, char **argv) {

 arrays::vector<double> D (5), DD(4);
 for (size_t i=0; i<5; ++i) {D(i) = 3.2*i+2.3;}
 for (size_t i=0; i<4; ++i) {DD(i) = 2.4*(i+1.82) + 0.78;} 

 auto R = range(0,5);

 blas::tridiag_worker w(10);
 w(D,DD);
 std::cerr<<" values = "<< w.values()<<std::endl;
 std::cerr<<" vectors = "<< w.vectors()(R,R)<<std::endl;

 //check 
 matrix<double> A(5,5); A()=0;
 for (size_t i=0; i<5; ++i) A(i,i) = D(i);
 for (size_t i=0; i<4; ++i) { A(i,i+1) = DD(i); A(i+1,i) = DD(i);} 

 std::cerr<<"A = "<<A<<std::endl;
 linalg::eigenelements_worker< matrix_view <double>, true> w2 (A());
 w2.invoke();
 std::cerr<<" check values = "<< w2.values()<<std::endl;
 std::cerr<<" check vectors = "<< w2.vectors()<<std::endl;

 assert_all_close(w.vectors()(R,R), w2.vectors(), 1.e-10);
 assert_all_close(array_view<double,1>(w.values()), w2.values(), 1.e-10);

 return 0;
}
