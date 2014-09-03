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
#include <triqs/arrays/linalg/eigenelements.hpp>
#include <triqs/arrays/blas_lapack/stev.hpp>
using namespace triqs::arrays;
using namespace triqs;

int main(int argc, char **argv) {

 std::cerr << "Real matrix" << std::endl;
 {
 arrays::vector<double> D (5), DD(4);
 for (size_t i=0; i<5; ++i) {D(i) = 3.2*i+2.3;}
 for (size_t i=0; i<4; ++i) {DD(i) = 2.4*(i+1.82) + 0.78;} 

 auto R = range(0,5);

 blas::tridiag_worker<false> w(10);
 w(D,DD);
 std::cerr<<" values = "<< w.values()<<std::endl;
 std::cerr<<" vectors = "<< w.vectors()(R,R)<<std::endl;

 //check
 matrix<double> A(5,5); A()=0;
 for (size_t i=0; i<5; ++i) A(i,i) = D(i);
 for (size_t i=0; i<4; ++i) { A(i,i+1) = DD(i); A(i+1,i) = DD(i);}
 auto AA = A; // A will be destroyed by linalg::eigenelements_worker

 std::cerr<<"A = "<<A<<std::endl;
 linalg::eigenelements_worker< matrix_view <double>, true> w2 (A());
 w2.invoke();
 std::cerr<<" check values = "<< w2.values()<<std::endl;
 std::cerr<<" check vectors = "<< w2.vectors()<<std::endl;

 assert_all_close(w.vectors()(R,R), w2.vectors(), 1.e-10);
 arrays::vector<double> zero(5); zero() = 0;
 for(int n=0; n<5; ++n){
    arrays::vector<double> diff = (AA - w.values()(n))*w.vectors()(n,R);
    assert_all_close(diff,zero,1.e-10);
 }
 }

 std::cerr << std::endl << "Complex matrix" << std::endl;
 {
 std::complex<double> I(0,1.0);
 arrays::vector<double> D (5);
 arrays::vector<std::complex<double>> DD(4);
 for (size_t i=0; i<5; ++i) {D(i) = 3.2*i+2.3;}
 for (size_t i=0; i<4; ++i) {DD(i) = 2.4*(i+1.82) + 0.78*I;}
 DD(1) = 0;

 auto R = range(0,5);

 blas::tridiag_worker<true> w(10);
 w(D,DD);
 std::cerr<<" values = "<< w.values()<<std::endl;
 std::cerr<<" vectors = "<< w.vectors()(R,R)<<std::endl;

 //check
 matrix<std::complex<double>> A(5,5); A()=0;
 for (size_t i=0; i<5; ++i) A(i,i) = D(i);
 for (size_t i=0; i<4; ++i) {A(i,i+1) = DD(i); A(i+1,i) = conj(DD(i));}
 auto AA = A;   // A will be destroyed by linalg::eigenelements_worker

 std::cerr<<"A = "<<A<<std::endl;
 linalg::eigenelements_worker<matrix_view<std::complex<double>>, true> w2 (A());
 w2.invoke();
 std::cerr<<" check values = "<< w2.values()<<std::endl;
 std::cerr<<" check vectors = "<< w2.vectors()<<std::endl;

 assert_all_close(array_view<double,1>(w.values()), w2.values(), 1.e-10);
 arrays::vector<std::complex<double>> zero(5); zero() = 0;
 for(int n=0; n<5; ++n){
    arrays::vector<std::complex<double>> diff = (AA - w.values()(n))*w.vectors()(n,R);
    assert_all_close(diff,zero,1.e-10);
 }
 }

 return 0;
}
