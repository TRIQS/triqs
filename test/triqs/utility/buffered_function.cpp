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
#include <triqs/utility/buffered_function.hpp>
#include <iostream>

int main(int argc, char **argv) {

 // a function that generates all the square ....
 int x = 0;
 auto f = [x]() mutable { auto res=x*x; x++; return res; };

 // C++14 : init-capture 
 // auto f = [x=0]() mutable { return x*(x++);};

 auto gen = triqs::utility::buffered_function<double>(f, 5);

 for (int u = 0; u < 22; ++u)
  if (gen() != u * u) throw "Error";
}
