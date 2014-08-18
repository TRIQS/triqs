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
#include <iostream>

#include <triqs/arrays/array.hpp>
#include <triqs/arrays/vector.hpp>
#include <triqs/arrays/matrix.hpp>
#include <triqs/arrays/blas_lapack/dot.hpp>
#include <triqs/arrays/asserts.hpp>

using namespace triqs::arrays;

template<typename Vd, typename Vi> 
void test() {
 Vd a(2),aa(2),c(2) ;a()=2.0; c() = 1;
 Vi b(2);b()=3;
 std::cout << blas::dot<false>(a,b) << std::endl;

 aa = 2*a;

 assert_close( dot(a,b), 12);
 assert_close( dot(aa,a), 16);
 assert_close( dot(aa,b), 24);
 assert_close( dot(aa-a,b), 12);

 std::cerr  << dot(aa,a) << std::endl ;
 std::cerr  << dot(aa,b) << std::endl ;
 std::cerr  << dot(aa-a, b) << std::endl;
 std::cerr<< "---------------------"<<std::endl;
}

int main(int argc, char **argv) {

 test<vector<double> , vector<int> >();
 
 // does not work for array just because of .size() vs .shape(0)...
 //test<array<double,1> , array<int,1> >();
 //test<array<double,1> , vector<int> >();
 //test<vector<double> , array<int,1>  >();

}



