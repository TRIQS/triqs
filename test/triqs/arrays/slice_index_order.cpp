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
#include <triqs/arrays/impl/common.hpp>
#include <triqs/arrays/indexmaps/range.hpp>
#include <triqs/arrays/indexmaps/permutation.hpp>
#include <triqs/arrays/indexmaps/cuboid/slice_traversal_order.hpp>

using namespace triqs::arrays;
using namespace triqs::arrays::permutations;

template<ull F> struct P { 
 static constexpr ull value = F;
 friend std::ostream & operator <<( std::ostream & out, P const &  s) { 
  //out << "Permutation of size " << permutations::size(s.value) << " : "<< std::hex;
  out << std::hex;
  s.print(out, std::integral_constant<ull,0>()); 
  return out << std::dec;
 } 

 template<ull c> void print( std::ostream & out, std::integral_constant<ull,c>) const { out << apply(this->value,c); print(out,  std::integral_constant<ull,c+1>());}
 void print( std::ostream & out, std::integral_constant<ull,size(F)>) const {}
};

template<ull R, ull p, typename... Args > void test() { 

 typedef indexmaps::cuboid::slicing_TO_order::sliced_memory_order<p,Args...> S1;
 constexpr auto s1 = S1::value; 
 //std::cout  << " sliced "<< std::hex<< S1::value << "  " <<std::endl ; 
 std::cout << P<p>() << std::endl;
 std::cout << P<s1>() << std::endl;
 //std::cout  << "mashk "<< std::hex<<S1::mask_t::value<< std::endl;
 if ( R != s1) {std::cout  << "error "<< std::hex<< R<< "  "<< s1<<std::endl ; exit(1);}//TRIQS_RUNTIME_ERROR<<" failed";
 std::cout  << "----------------------"<< std::endl ; 
}
int main(int argc, char **argv) {

 

 std::cout  << " F order " << std::endl ; 
 test< permutation(0,1)     ,permutation(0,1,2,3), int, range,int, range>();
 test< permutation(0,1,2)   ,permutation(0,1,2,3), range, range,int, range>();
 test< permutations::identity(4) ,permutation(0,1,2,3), range, range, range, range>();
 //test< permutation(0,1,2,3) ,permutation(0,1,2,3), range, range, range, range>();
 test< 0                     ,permutation(0,1,2,3), int, int, int, int> (); 

 std::cout  << " c order " << std::endl ; 
 test< permutation(1,0)     ,permutation(3,2,1,0), int, range,int, range>();
 test< permutation(2,1,0)   ,permutation(3,2,1,0), int, range, range, range>();
 test< permutation(2,1,0)   ,permutation(3,2,1,0), range,int, range, range>();
 test< permutation(2,1,0)   ,permutation(3,2,1,0), range, range,int, range>();
 test< permutation(2,1,0)   ,permutation(3,2,1,0), range, range, range, int>();
 test< permutation(3,2,1,0) ,permutation(3,2,1,0), range, range, range, range>();
 test< 0                     ,permutation(3,2,1,0), int, int, int, int> (); 

 test< permutation(0), permutation(0,1), int, range>();

 std::cout  << " custom order " << std::endl ; 
 test< permutation(1,0)     ,permutation(0,3,1,2), int, range,int, range>();
 test< permutation(2,0,1)   ,permutation(0,3,1,2), int, range, range, range>();
 test< permutation(0,2,1)   ,permutation(0,3,1,2), range,int, range, range>();
 test< permutation(0,2,1)   ,permutation(0,3,1,2), range, range,int, range>();
 test< permutation(0,1,2)   ,permutation(0,3,1,2), range, range, range, int>();
 test< permutation(0,3,1,2) ,permutation(0,3,1,2), range, range, range, range>();
 test< 0                     ,permutation(0,3,1,2), int, int, int, int> (); 

 std::cout  << " ----------- custom order ------------- " << std::endl ; 

 std::cout  << " ---- 0 int "<< std::endl ;
 test< permutation(2,0,3,1) ,permutation(2,0,3,1), range, range, range, range>();

 std::cout  << " ---- 1 int "<< std::endl ;
 test< permutation(1,2,0)   ,permutation(2,0,3,1), int, range, range, range>();
 test< permutation(1,0,2)   ,permutation(2,0,3,1), range,int, range, range>();
 test< permutation(0,2,1)   ,permutation(2,0,3,1), range, range,int, range>();
 test< permutation(2,0,1)   ,permutation(2,0,3,1), range, range, range, int>();

 std::cout  << " ---- 2 int "<< std::endl ;
 test< permutation(0,1)   ,permutation(2,0,3,1), int, int, range, range>();
 test< permutation(1,0)   ,permutation(2,0,3,1), int, range,int, range>();
 test< permutation(1,0)   ,permutation(2,0,3,1), int, range,range, int>();
 test< permutation(0,1)   ,permutation(2,0,3,1), range,int, int, range>();
 test< permutation(1,0)   ,permutation(2,0,3,1), range,int, range, int>();
 test< permutation(0,1)   ,permutation(2,0,3,1), range, range,int, int>();

 std::cout  << " ---- 3 int "<< std::endl ;
 test< permutation(0)   ,permutation(2,0,3,1), range, int, int, int>();
 test< permutation(0)   ,permutation(2,0,3,1), int,range, int, int>();
 test< permutation(0)   ,permutation(2,0,3,1), int, int,range, int>();
 test< permutation(0)   ,permutation(2,0,3,1), int, int, int, range>();

 std::cout  << " ---- 4 int "<< std::endl ;
 test< 0                     ,permutation(2,0,3,1), int, int, int, int> (); 
 std::cout  << " OK "<< std::endl ;

}
