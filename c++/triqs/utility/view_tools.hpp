// Copyright (c) 2013-2015 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2015 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2019-2023 Simons Foundation
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
#include <type_traits>
#include <nda/nda.hpp>

namespace triqs {

  namespace details {
    template <typename T, typename Enable = void> struct _regular {
      using type = T;
    };
    template <typename T> struct _regular<T, std::void_t<typename T::regular_type>> {
      using type = typename T::regular_type;
    };

    template <typename T, typename Enable = void> struct _view_or_type {
      using type = T;
    };
    template <typename T> struct _view_or_type<T, std::void_t<typename T::view_type>> {
      using type = typename T::view_type;
    };
    template <typename T> struct _view_or_type<T const, std::void_t<typename T::view_type>> {
      using type = typename T::const_view_type;
    };

    template <typename T, typename Enable = void> struct _const_view_or_type {
      using type = T;
    };
    template <typename T> struct _const_view_or_type<T, std::void_t<typename T::const_view_type>> {
      using type = typename T::const_view_type;
    };
  } // namespace details

  template <typename T> using regular_t            = typename details::_regular<std::decay_t<T>>::type;
  template <typename T> using view_or_type_t       = typename details::_view_or_type<T>::type;
  template <typename T> using const_view_or_type_t = typename details::_const_view_or_type<T>::type;

  using nda::make_regular;
} //namespace triqs
