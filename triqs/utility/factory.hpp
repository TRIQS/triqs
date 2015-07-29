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
#pragma once
#include <type_traits>
#include <vector>
#include "./macros.hpp"

namespace triqs {
namespace utility {

 template <typename T> struct factories {
  template <typename U> static T invoke(U&& x) { return T(std::forward<U>(x)); }
 };

 template <typename T> struct factories<std::vector<T>> {
  using R = std::vector<T>;

  static R invoke(R&& x) { return R(std::move(x)); }
  static R invoke(R const& x) { return R(x); }
  static R invoke(R& x) { return R(x); }

  template <typename U> static R invoke(std::vector<U>&& v) {
   R r;
   r.reserve(v.size());
   for (auto& x : v) r.push_back(factories<T>::invoke(std::move(x)));
   return r;
  }

  template <typename U> static R invoke(std::vector<U>& v) {
   R r;
   r.reserve(v.size());
   for (auto& x : v) r.push_back(factories<T>::invoke(x));
   return r;
  }

  template <typename U> static R invoke(std::vector<U> const& v) {
   R r;
   r.reserve(v.size());
   for (auto& x : v) r.push_back(factories<T>::invoke(x));
   return r;
  }
 };

 template <typename T, typename... U> T factory(U&&... x) { return factories<T>::invoke(std::forward<U>(x)...); }
}
} // namespace triqs
