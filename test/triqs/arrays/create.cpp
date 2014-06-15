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
#include <triqs/arrays/array.hpp>
#include <triqs/arrays/matrix.hpp>
#include <iostream>


using namespace triqs::arrays;

int main(int argc, char **argv) {
 
 

 array<long,2> A ;
 A.resize(make_shape(3,3));

 std::cout << A.shape() <<std::endl;
   
 matrix<double> M;
 M.resize( 3,3); 
 triqs::arrays::vector<double> V;
 V.resize(10);


 return 0;
}

