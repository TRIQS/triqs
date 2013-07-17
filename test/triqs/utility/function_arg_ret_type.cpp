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
#include <iostream>
#include <type_traits>
#include <triqs/utility/function_arg_ret_type.hpp>
using namespace triqs;

template<typename T> struct F { T operator()(T const &x) { return 2*x;} };

int main(int argc, char **argv) {

 auto lambda = [](int const & i) { return double(i*10); };

 typedef utility::function_arg_ret_type<decltype(lambda)> traits;

 static_assert(std::is_same<double, traits::result_type>::value, "err");
 static_assert(std::is_same<int const &, traits::arg<0>::type>::value, "err");
 static_assert(std::is_same<int, traits::decay_arg<0>::type>::value, "err");

 typedef utility::function_arg_ret_type<F<int>> traits2;
 static_assert(std::is_same<int, traits2::decay_arg<0>::type>::value, "err");

 return 0;
}
