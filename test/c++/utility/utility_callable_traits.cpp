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

#include <triqs/test_tools/arrays.hpp>
#include <triqs/utility/callable_traits.hpp>

#include <concepts>

struct bar {};

template <typename T> struct F {
  T operator()(T const &x, bar) { return x * 10; }
};

TEST(TRIQSUtility, CallableTraitsWithLambda) {
  auto lambda  = [](int const &i) { return i * 10.0; };
  using traits = triqs::utility::callable_traits<decltype(lambda)>;
  static_assert(traits::arity == 1);
  static_assert(std::same_as<double, traits::result_type>);
  static_assert(std::same_as<int const &, traits::arg_t<0>>);
  static_assert(std::same_as<int, traits::decay_arg_t<0>>);
}

TEST(TRIQSUtility, CallableTraitsWithCallableObject) {
  using traits = triqs::utility::callable_traits<F<long>>;
  static_assert(traits::arity == 2);
  static_assert(std::same_as<long, traits::result_type>);
  static_assert(std::same_as<long const &, traits::arg_t<0>>);
  static_assert(std::same_as<long, traits::decay_arg_t<0>>);
  static_assert(std::same_as<bar, traits::arg_t<1>>);
  static_assert(std::same_as<bar, traits::decay_arg_t<1>>);
}

TEST(TRIQSUtility, CallableTraitsWithDefaultArgument) {
  auto lambda  = [](int i = 10) { return i * 10; };
  using traits = triqs::utility::callable_traits<decltype(lambda)>;
  static_assert(traits::arity == 1);
  static_assert(std::same_as<int, traits::result_type>);
  static_assert(std::same_as<int, traits::arg_t<0>>);
  static_assert(std::same_as<int, traits::decay_arg_t<0>>);
}

MAKE_MAIN
