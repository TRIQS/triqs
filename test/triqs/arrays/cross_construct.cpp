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
#include "./src/array.hpp"
#include "./src/matrix.hpp"
#include "./src/vector.hpp"
#include <iostream>

using namespace triqs::arrays;

int main(int argc, char **argv) {
 
 

 array<long,2> A (2,3);

 for (int i =0; i<2; ++i)
  for (int j=0; j<3; ++j) 
   A(i,j) = 10*i+ j;

 array<double,2> B(A);
 std::cerr << "B = " << B <<std::endl ;

 vector<int> Vi(3); Vi() = 3;
 vector<double> Vd(Vi);
 std::cerr  << "V = "<< Vd << std::endl;

 std::vector<array<long,2> > V(3, A);

 std::vector<array_view<long,2> > W;
 for (auto & x : V) W.push_back(x);
 
 std::vector<array_view<long,2> > W2(W);
 long i=1; for (auto & x : V)  x *=i++;

 for (auto & x : V) { std::cout  <<x << std::endl ;}
 for (auto & x : W) { std::cout  <<x << std::endl ;}

 return 0;
}


