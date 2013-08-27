/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2013 by O. Parcollet
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

#include "../common.hpp"
#include "../src/array.hpp"
#include <iostream>

//using std::cout; using std::endl;
using namespace triqs::arrays;
#include <type_traits>


template<typename U, ull_t opt, ull_t to, bool B> 
void f (array_const_view<U,2,opt,to,B> const & a) { 
 std::cout  << a << std::endl ;
}

int main(int argc, char **argv) {

 array<long,2 > A (2,3);
 A() =3;

 f(A());

}




