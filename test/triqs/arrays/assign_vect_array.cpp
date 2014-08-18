
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
#include <triqs/arrays/vector.hpp>
#include <iostream>

using namespace triqs::arrays;

int main(int argc, char **argv) {

 

 triqs::arrays::vector<double> V; array<double,1> Va(5);
 for (int i =0; i<5; ++i) Va(i) = i+2;
 V = Va/2.0;
 std::cout<<" V = VA/2 : "<< V<< Va<<std::endl;
 V = Va;
 std::cout<<" V = VA : "<< V<< Va<<std::endl;
 return 0;
}

