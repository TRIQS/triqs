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

#include "./src/array.hpp"
#include "./src/vector.hpp"
#include "./src/matrix.hpp"

using namespace triqs::arrays;

int main(int argc, char **argv) {

// only for icc 13.x
#ifndef TRIQS_WORKAROUND_INTEL_COMPILER_BUGS 
 array<double,1> A = {1,2,3,4};
 std::cout  << A<< A.shape()<<std::endl ;

 array<double,2> B = {{1,2},{3,4}, {5,6}};
 std::cout  << B<< B.shape()<<std::endl ;

 triqs::arrays::vector<double> V = {1.1,2.0,3.2,4.0};
 std::cout  << V<< V.shape()<<std::endl ;

 matrix<double> M = {{1,2},{3,4}, {5,6}};
 std::cout  << M<< M.shape()<<std::endl ;


#endif

}



