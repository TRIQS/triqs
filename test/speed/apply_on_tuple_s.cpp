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
#include <triqs/utility/tuple_tools.hpp>
#include <functional>
#include <iostream>

struct fun { double operator()(int i, double x, double y, int k) {return k + i+ x + 2*y;} };

const int nl_interne = 1000000;

struct with_unpack {
  double s;
 ~with_unpack() { std::cout  << s<< std::endl ;}
 void operator()() {
  //std::function<double(int,double,double,int)> F(f);
  fun F;
  auto t = std::make_tuple(1,2.3,4.3,8);
  for (int u =0; u<nl_interne; ++u) { s += triqs::tuple::apply(F,t);}
 }
};

struct manual_code {
  double s;
 ~manual_code() { std::cout  << s<< std::endl ;}
 void operator()() {
 // std::function<double(int,double,double,int)> F(f);
  fun F;
  for (int u =0; u<nl_interne; ++u) { s += F(1,2.3,4.3,8); }
 }
};

#include "./speed_tester.hpp"
int main() {
 speed_tester<with_unpack> (50000);
 speed_tester<manual_code> (50000);
 return 0;
}



