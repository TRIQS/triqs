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

#ifndef TRIQS_UTILITY_TRAITS_FUNCTION_H
#define TRIQS_UTILITY_TRAITS_FUNCTION_H
#include <iostream>
#include <tuple>
#include <type_traits>

namespace triqs {
  namespace utility {

    namespace details {

      template <typename Fun> struct function_arg_ret_type1;

      template <typename ClassType, typename ReturnType, typename... Args> struct function_arg_ret_type1<ReturnType (ClassType::*)(Args...) const> {
        static constexpr int arity = sizeof...(Args);
        using result_type          = ReturnType;
        template <size_t i> struct arg : std::tuple_element<i, std::tuple<Args...>> {};
        template <size_t i> struct decay_arg : std::tuple_element<i, std::tuple<typename std::decay<Args>::type...>> {};
      };

      template <typename ClassType, typename ReturnType, typename... Args>
      struct function_arg_ret_type1<ReturnType (ClassType::*)(Args...)> : function_arg_ret_type1<ReturnType (ClassType::*)(Args...) const> {};
    } // namespace details

    /*
  * Detect argument and return type of a callable object,
  * as long as its operator () is not overloaded
  *
  * \tparam Fun The callable object (object, lambda, etc...)
  *
  * \return
  *   * arg<i>::type is the type of the i th argument
  *   * decay_arg<i>::type is the std::decay<> of the type of the i-th argument
  *   * result_type is the return type of the function
  *   * constexpr int arity is the arity
  *
  *   Example :
  *   Object with such a call  :
  *
  *   R operator()( int const &, double )
  *
  *    result_type = R
  *    arg<0> = int const &
  *    arg<1> = double
  *    decay_arg<0> = int
  *    decay_arg<1> = double
  */
    template <typename Fun> struct function_arg_ret_type : public details::function_arg_ret_type1<decltype(&Fun::operator())> {};

  } // namespace utility
} // namespace triqs
#endif
