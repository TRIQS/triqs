/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2018, The Simons Foundation
 * Author: H. U.R. Strand
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
#include "start.hpp"

using namespace triqs::utility;

TEST(Array, empty_iteration) {

  /*
  std::vector<size_t> sizes = {1, 2, 3};
  array<double, 2> arr2(sizes);
  */

  int size = 0;
  std::cout << "size = " << size << "\n";
  array<double, 1> arr(size);
  std::cout << "lengths = " << arr.domain().lengths() << "\n"; 
  std::cout << "nel = " << arr.domain().number_of_elements() << "\n";

  auto it = arr.begin();
  auto it_end = arr.end();
  
  std::cout << "it = " << it << "\n";
  std::cout << "it_end = " << it_end << "\n";

  for( auto i : arr ) std::cout << i << "\n";
  
}

MAKE_MAIN;
  
