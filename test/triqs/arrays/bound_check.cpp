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
#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK

#include "./common.hpp"
#include <triqs/arrays/array.hpp>
#include <triqs/arrays/matrix.hpp>
#include <iostream>

using namespace triqs::arrays;

int main(int argc, char **argv) {

 

 array<long,2, BoundCheck > A (2,3);
 array<long,2 > B (2,3);
 array<long,1 > C(2);
 array<long,2, BoundCheck > Af (2,3, FORTRAN_LAYOUT);

 try { 

  for (int i =0; i<2; ++i)
   for (int j=0; j<4; ++j) 
    B(i,j) = 10*i+ j;
 }

 catch ( triqs::arrays::key_error & e) { 
  std::cout<< " catching key error in B "<< std::endl ;
  std::cerr << e.what() <<std::endl ;
 }

 try { 

  for (int i =0; i<2; ++i)
   for (int j=0; j<4; ++j) 
    A(i,j) = 10*i+ j;
 }

 catch ( triqs::arrays::key_error & e) { 
  std::cout<< " catching key error in A "<< std::endl ;
  std::cerr << e.what() <<std::endl ;
 }

 try { 
  std::cout  << A( range(0,4),2) << std::endl ;
 }
 catch ( triqs::arrays::key_error & e) { 
  std::cout<< " catching key error in slice "<< std::endl ;
  std::cerr << e.what() <<std::endl ;
 }

 try { 
  std::cout  << A( range(10,14),2) << std::endl ;
 }
 catch ( triqs::arrays::key_error & e) { 
  std::cout<< " catching key error in slice "<< std::endl ;
  std::cerr << e.what() <<std::endl ;
 }

 try { 
  std::cout  << A( range(),5) << std::endl ;
 }
 catch ( triqs::arrays::key_error & e) { 
  std::cout<< " catching key error in slice "<< std::endl ;
  std::cerr << e.what() <<std::endl ;
 }

 return 0;
}

