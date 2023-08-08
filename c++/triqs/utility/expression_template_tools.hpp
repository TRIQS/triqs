// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2023 Simons Foundation
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

#ifndef TRIQS_UTILITY_EXPRESSION_TEMPLATE_TOOLS_H
#define TRIQS_UTILITY_EXPRESSION_TEMPLATE_TOOLS_H
#include <type_traits>
#include <complex>
#include "./macros.hpp"

namespace triqs {
  namespace utility {

    template <class T> struct remove_rvalue_ref {
      typedef T type;
    };
    template <class T> struct remove_rvalue_ref<T &> {
      typedef T const &type;
    };
    template <class T> struct remove_rvalue_ref<T &&> {
      typedef T type;
    };
    template <class T> using remove_rvalue_ref_t = typename remove_rvalue_ref<T>::type;

    namespace tags {
      struct plus {};
      struct minus {};
      struct multiplies {};
      struct divides {};
    } // namespace tags

    // The basic operations put in a template....
    template <typename Tag> struct operation;
    template <> struct operation<tags::plus> {
      template <typename L, typename R> auto operator()(L &&l, R &&r) const DECL_AND_RETURN(std::forward<L>(l) + std::forward<R>(r));
      static const char name = '+';
    };
    template <> struct operation<tags::minus> {
      template <typename L, typename R> auto operator()(L &&l, R &&r) const DECL_AND_RETURN(std::forward<L>(l) - std::forward<R>(r));
      static const char name = '-';
    };
    template <> struct operation<tags::multiplies> {
      template <typename L, typename R> auto operator()(L &&l, R &&r) const DECL_AND_RETURN(std::forward<L>(l) * std::forward<R>(r));
      static const char name = '*';
    };
    template <> struct operation<tags::divides> {
      template <typename L, typename R> auto operator()(L &&l, R &&r) const DECL_AND_RETURN(std::forward<L>(l) / std::forward<R>(r));
      static const char name = '/';
    };

    // The scalar ...
    template <typename T> struct is_in_ZRC : std::is_arithmetic<T> {};
    template <> struct is_in_ZRC<bool> : std::true_type {};
    template <typename T> struct is_in_ZRC<std::complex<T>> : std::true_type {};
    template <typename T> struct is_in_ZRC<T &> : is_in_ZRC<T> {};
    template <typename T> struct is_in_ZRC<T &&> : is_in_ZRC<T> {};
    template <typename T> struct is_in_ZRC<const T> : is_in_ZRC<T> {};

    template <typename A, typename B> struct type_of_mult {
      typedef decltype(std::declval<typename std::remove_reference<A>::type>() * std::declval<typename std::remove_reference<B>::type>()) type;
    };

  } // namespace utility
} // namespace triqs
#endif
