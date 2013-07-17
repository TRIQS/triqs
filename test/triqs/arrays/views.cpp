
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
#include <iostream>

using std::cout; using std::endl;
using namespace triqs::arrays;

int main(int argc, char **argv) {

 

 {
  std::cout<<"test doc eg"<<std::endl;
  array<int,2> *p = new array<int,2> (2,3); // create an array p
  for (int i =0; i<2; ++i)
   for (int j=0; j<3; ++j)
    (*p)(i,j) = 10*i+ j;
  array_view<int,2> B(*p); // making a view
  delete p; // p is gone...
  B(0,0) = 314;
  std::cout<<B<<std::endl<<"Done"<<std::endl; // ok, but B (and the data) is still alive
 }

 array<long,2> A (2,3);
 for (int i =0; i<2; ++i)
  for (int j=0; j<3; ++j) 
   A(i,j) = 10*i+ j;

 array_view<long const ,2 > AA (A);

 std::cout<<"A is "<<A<<std::endl;
 std::cout<<"another view : "<<AA<<std::endl;

 {
  array_view<long,1>  SL( A(0,range(0,3)));
  array_view<long,1>  SL2( A(1,range(0,2)));
  array_view<long,1>  SL2b( A(1,range(1,3)));

  std::cout<<"SLICE : A(0,range(0,3))  "<<SL<<std::endl;
  std::cout<<"SLICE : A(1,range(0,2))  "<<SL2<<std::endl;
  std::cout<<"SLICE : A(1,range(1,3))  "<<SL2b<<std::endl;
 }
 {
  array_view<long,1>  SL( A(range(0,2),0));
  array_view<long,1>  SL2( A(range(0,2),1));

  std::cout<<"SLICE : A(range(0,2),0))  "<<SL<<std::endl;
  std::cout<<"SLICE : A(range(0,2),1))  "<<SL2<<std::endl;
 }

 array_view<long,2> V(A);
 std::cout<< V(0,0)<<std::endl;
 std::cout<< V<<std::endl;

 return 0;
}

