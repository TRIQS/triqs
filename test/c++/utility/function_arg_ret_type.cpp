// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018 Simons Foundation
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You may obtain a copy of the License at
//     https://www.gnu.org/licenses/gpl-3.0.txt
//
// Authors: Olivier Parcollet, Nils Wentzell

#include <iostream>
#include <type_traits>
#include <triqs/utility/function_arg_ret_type.hpp>
using namespace triqs;

template <typename T> struct F {
  T operator()(T const &x) { return 2 * x; }
};

int main(int, char **) {

  auto lambda = [](int const &i) { return double(i * 10); };

  typedef utility::function_arg_ret_type<decltype(lambda)> traits;

  static_assert(std::is_same<double, traits::result_type>::value, "err");
  static_assert(std::is_same<int const &, traits::arg<0>::type>::value, "err");
  static_assert(std::is_same<int, traits::decay_arg<0>::type>::value, "err");

  typedef utility::function_arg_ret_type<F<int>> traits2;
  static_assert(std::is_same<int, traits2::decay_arg<0>::type>::value, "err");

  return 0;
}
