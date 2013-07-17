
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

//using std::cout; using std::endl;
//using namespace triqs::arrays;
#include <type_traits>

static_assert( ! std::is_pod< triqs::arrays::array<long,2 > >::value, "POD pb");

int main(int argc, char **argv) {

 
 triqs::arrays::array<long,2 > A (2,3);

 for (int i =0; i<2; ++i)
  for (int j=0; j<3; ++j) 
  { A(i,j) = 10*i+ j; }

 for (int i =0; i<2; ++i)
  for (int j=0; j<3; ++j) 
   std::cout<<"A "<<A(i,j)<<std::endl;

 std::cout<<"A = "<<A<<std::endl;
 
 //empty array
 triqs::arrays::array<long,2> B;
 std::cout<<"B= "<<B<<std::endl;

 return 0;
 std::cout<<A.indexmap().lengths()<<std::endl;
 std::stringstream fs; fs <<A.indexmap().lengths();
 std::cout<<fs.str();
 return 0;
}




