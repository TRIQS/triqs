/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2015 by O. Parcollet
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
#include <triqs/utility/static_if.hpp>
#include <triqs/test_tools/arrays.hpp>
#include <iostream>

using namespace triqs::utility;

template<typename T> auto f(T const &y) {

 auto r = static_if(bool_<std::is_integral<T>::value>{})
              .then([](auto &&x) { return std::make_pair(x, x); })
              .else_([](auto &&x) { return x.zozo(); })(y);
 return r;
}


TEST(StaticIF, Return) {

 static_assert(std::is_same<std::pair<int,int>, decltype(f(1))>::value, "oops");
 //static_assert(std::is_same<double, decltype(f(1.0))>::value, "oops");
}

MAKE_MAIN;

