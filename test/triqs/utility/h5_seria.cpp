/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2014 by O. Parcollet
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
#include <triqs/arrays.hpp>
#include <iostream>

#if not H5_VERSION_GE(1,8,9)
int main() {}
#else

#include <triqs/h5/serialization.hpp>

using triqs::h5::serialize;
using triqs::h5::deserialize;

int main() {

 try{
 using triqs::arrays::array;

 auto a = array<double, 1>{1, 2, 3, 4, 5};

 {
  auto a2 = array<double, 1>(10000);
  a2() = 8.3;
  auto s2 = serialize(a2);
  std::cout << "len " << s2.size() << std::endl;
 }

 {
  auto a2 = array<double, 1>(20000);
  a2() = 8.3;
  auto s2 = serialize(a2);
  std::cout << "len " << s2.size() << std::endl;
 }

 auto s1 = serialize(a);
 auto b = deserialize<array<double, 1>>(s1);

 std::cout << "a = " << a << " == " << b << std::endl;
 }
 catch(std::exception const & e) { std::cout  << e.what()<<std::endl;}
}
#endif

