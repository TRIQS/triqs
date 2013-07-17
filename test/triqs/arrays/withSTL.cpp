
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

#include <vector>
#include <map>
#include <algorithm>

bool te(int x) { return (x<25);}

int main(int argc, char **argv) {
 

 array<long,2> A (2,3);
 for (int i =0; i<2; ++i)
  for (int j=0; j<3; ++j) 
   A(i,j) = 10*i+ j;

 std::vector<array<long,2> > VV;
 VV.push_back(A);

 std::map<std::string, array<long,2> > MAP;
 MAP["1"] = A;
 std::cout<<"printing map"<<std::endl;
 std::cout<<MAP["1"]<<std::endl;

 // Trying to put a vector in an array
 std::vector<int> V (10);
 array<int,1 > B(V.size()), C(V.size());
 
 //put something in V
 for (unsigned int i =0; i<10; ++i) V[i] = 10+i;

 // copy to B. Iterators on array are STL compliant so STL algorithms work.
 std::copy(V.begin(),V.end(),B.begin()); 
 std::cout<<" B = "<< B<<std::endl;

 // change B
 for (int i =0; i<10; ++i) B(i)*=2;
 
 // come back !
 std::copy(B.begin(),B.end(),V.begin()); 

 // print V
 std::copy(V.begin(),V.end(),std::ostream_iterator<int>(std::cout,","));

 // countif
 std::cout<<" B= "<< B<<std::endl;
 std::cout<<" Number of elements <25 : "<< std::count_if(B.begin(), B.end(),te)<<std::endl;
 //std::cout<<" Number of elements <25"<< std::count_if(B.begin(), B.end(),[](int x){ return x<25;});
 
 // max_element
 B(9) = 0; B(8) = 18;
 std::cout<<" B= "<< B<<std::endl;
 std::cout<<" max(B) "<< *std::max_element(B.begin(),B.end())<<std::endl;
 std::cout<<" max(B) position "<< std::max_element(B.begin(),B.end()).indices()[0]<<std::endl;

 // replace_if
 std::replace_if (B.begin(), B.end(), te, 0);
 std::cout<<" B= "<< B<<std::endl;
 
 //swap
 C()=0;
 std::cout<<" B= "<< B<<std::endl;
 std::cout<<" C= "<< C<<std::endl;
 std::swap(B,C);
 std::cout<<" B= "<< B<<std::endl;
 std::cout<<" C= "<< C<<std::endl;

 return 0;
}

