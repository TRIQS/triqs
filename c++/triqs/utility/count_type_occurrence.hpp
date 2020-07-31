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

#ifndef TRIQS_UTILITY_COUNT_OCCUR_H
#define TRIQS_UTILITY_COUNT_OCCUR_H
namespace triqs {

  // count_type_occurrence<T, Args...>::value is the number of Args deriving or equal to T

  template <typename T, typename... A> struct count_type_occurrence;

  template <typename T, typename A0, typename... A>
  struct count_type_occurrence<T, A0, A...> : std::integral_constant<int, std::is_base_of<T, A0>::value + count_type_occurrence<T, A...>::value> {};

  template <typename T> struct count_type_occurrence<T> : std::integral_constant<int, 0> {};

  template <typename T, typename... A> struct count_type_occurrence_not;

  template <typename T, typename A0, typename... A>
  struct count_type_occurrence_not<T, A0, A...>
     : std::integral_constant<int, !(std::is_base_of<T, A0>::value) + count_type_occurrence_not<T, A...>::value> {};

  template <typename T> struct count_type_occurrence_not<T> : std::integral_constant<int, 0> {};

} // namespace triqs
#endif
