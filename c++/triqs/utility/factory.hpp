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
#include <type_traits>
#include <vector>
#include "./macros.hpp"

namespace triqs {
  namespace utility {

    template <typename T> struct factories {
      template <typename U> static T invoke(U &&x) { return T(std::forward<U>(x)); }
    };

    template <typename T> struct factories<std::vector<T>> {
      using R = std::vector<T>;

      static R invoke(R &&x) { return R(std::move(x)); }
      static R invoke(R const &x) { return R(x); }
      static R invoke(R &x) { return R(x); }

      template <typename U> static R invoke(std::vector<U> &&v) {
        R r;
        r.reserve(v.size());
        for (auto &x : v) r.push_back(factories<T>::invoke(std::move(x)));
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

    template <typename T, typename... U> T factory(U &&...x) { return factories<T>::invoke(std::forward<U>(x)...); }
  } // namespace utility
} // namespace triqs
