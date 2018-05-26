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
#include <iostream>
#include <triqs/arrays/h5/simple_read_write.hpp>
#include <triqs/arrays/asserts.hpp>
#include <triqs/h5.hpp>

using std::cout;
using std::endl;
namespace tqa = triqs::arrays;
namespace h5  = triqs::h5;
using tqa::range;
using namespace tqa;

int main(int argc, char **argv) {

  // making some data
  tqa::array<std::string, 1> A(2), B;
  A(0) = "Nice String";
  A(1) = "another";
  std::vector<std::string> V1, V2;
  V1.push_back("abcd");
  V1.push_back("de");

  // writing
  h5::file file("test_array_string.h5", H5F_ACC_TRUNC);
  h5::group top(file);

  h5_write(top, "A", A);
  h5_write(top, "V", V1);

  // rereading
  h5_read(top, "A", B);
  h5_read(top, "V", V2);

  //comparing
  for (auto &x : V2) std::cerr << x << std::endl;
  assert_is_true(V1 == V2);
  assert_is_true(A == B);

  std::cerr << A << std::endl;
  std::cerr << B << std::endl;

  return 0;
}
