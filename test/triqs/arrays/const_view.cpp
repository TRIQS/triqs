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

using namespace triqs::arrays;

int main(int argc, char **argv) {

 const array<long,1> A = {1,2,3,4};

 std::cerr  << A.opt_flags<<std::endl;
 std::cerr  << A().opt_flags<<std::endl;

 // None of this should compile
 //A(0) = 2;
 //A()(0) = 2;
 //A(range(0,2))(0) = 10;

 return 0;
}

