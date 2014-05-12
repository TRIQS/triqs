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
#include <type_traits>

using namespace triqs::arrays;

int main() { 

 array<int, 1> A(3), B(3),C; 
 array<double,1> D;

 B() = 2; A() = 3;
 

 C = A+B; std::cout  << A + B<< " = "<< C << std::endl; 
 C = A*B; std::cout  << A * B<< " = "<< C << std::endl; 
 C = 2*B; std::cout  << 2 * B<< " = "<< C << std::endl; 
 C = B*2; std::cout  << 2 * B<< " = "<< C << std::endl; 
 D = 2.3*B; std::cout  << 2.3 * B<< " = "<< D << std::endl; 
 D = A + B/1.2; std::cout  << A +  B/1.2<< " = "<< D << std::endl; 


 auto x  = A + B + 2*A;
 std::cerr << x << std::endl;

  C =  A+2*A+3*A - 2*A+A -A+A+A*3+A+A+A+A+A+A+A+A+A+A+A+A+A; 
 std::cout  << C << std::endl ; 
 array<double,2> D2 (2,2);
// auto y = A+D/2 + (D2 + 2*D2);// should not and does not compile
}


