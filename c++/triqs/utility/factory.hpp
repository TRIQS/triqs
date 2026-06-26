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

/**
 * @file
 * @brief Generic factory for constructing objects of a given type.
 */

#pragma once

#include "./macros.hpp"

#include <utility>
#include <vector>

namespace triqs::utility {

  namespace detail {

    // Generic factory to construct a given type.
    template <typename T> struct factories {
      template <typename U> static T invoke(U &&x) { return T(std::forward<U>(x)); }
    };

    // Specialization of factories to construct a std::vector<T> from a std::vector<U>.
    template <typename T> struct factories<std::vector<T>> {
      using R = std::vector<T>;

      // Copy and move vectors of the same type.
      static R invoke(R &&x) { return R(std::move(x)); }
      static R invoke(R const &x) { return R(x); }
      static R invoke(R &x) { return R(x); }

      // Convert vectors of different types element-wise, moving the elements out of an rvalue source.
      template <typename U> static R invoke(std::vector<U> &&v) {
        auto tmp = std::move(v);
        R r;
        r.reserve(tmp.size());
        for (auto &x : tmp) r.push_back(factories<T>::invoke(std::move(x)));
        return r;
      }

      template <typename U> static R invoke(std::vector<U> &v) {
        R r;
        r.reserve(v.size());
        for (auto &x : v) r.push_back(factories<T>::invoke(x));
        return r;
      }

      template <typename U> static R invoke(std::vector<U> const &v) {
        R r;
        r.reserve(v.size());
        for (auto &x : v) r.push_back(factories<T>::invoke(x));
        return r;
      }
    };

  } // namespace detail

  /**
   * @ingroup triqs-utility-other
   * @brief Generic factory to construct an object of a given type from an arbitrary parameter pack of arguments.
   *
   * @details The actual construction is delegated to the helper class `detail::factories<T>`, whose static `invoke`
   * member is called with the forwarded arguments. By default `detail::factories<T>` simply forwards its single
   * argument to a constructor of `T`, so any type that is directly constructible from the given argumnets works out of 
   * the box.
   *
   * To support custom construction logic for a type `T` (e.g. element-wise conversion, allocation, or building from
   * a different representation), provide a partial or full specialization of `detail::factories<T>` exposing one or
   * more static `invoke` overloads that return a `T`. The library already specializes it for `std::vector<T>` to
   * allow constructing a vector from another vector with element-wise conversion.
   *
   * @tparam T Target type.
   * @tparam U Argument types.
   * @param x Constructor arguments.
   * @return A newly constructed `T` instance.
   */
  template <typename T, typename... U> T factory(U &&...x) { return detail::factories<T>::invoke(std::forward<U>(x)...); }

} // namespace triqs::utility
