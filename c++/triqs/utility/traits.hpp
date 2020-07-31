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

#pragma once

namespace triqs {

  // MPL traits
  template <typename... T> struct _or;
  template <typename T0, typename... T> struct _or<T0, T...> : std::integral_constant<bool, T0::value || _or<T...>::value> {};
  template <> struct _or<> : std::false_type {};

  template <typename... T> struct _and;
  template <typename T0, typename... T> struct _and<T0, T...> : std::integral_constant<bool, T0::value && _and<T...>::value> {};
  template <> struct _and<> : std::true_type {};

  // helpers
  template <bool B> using bool_constant = std::integral_constant<bool, B>;

} // namespace triqs
