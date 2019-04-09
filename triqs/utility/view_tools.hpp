/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011 by O. Parcollet
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

namespace triqs {

  namespace details {
    template <typename T, typename Enable = void> struct _regular { using type = T; };
    template <typename T> struct _regular<T, std::void_t<typename T::regular_type>> { using type = typename T::regular_type; };

    template <typename T, typename Enable = void> struct _view_or_type { using type = T; };
    template <typename T> struct _view_or_type<T, std::void_t<typename T::view_type>> { using type = typename T::view_type; };
    template <typename T> struct _view_or_type<T const, std::void_t<typename T::view_type>> { using type = typename T::const_view_type; };

    template <typename T, typename Enable = void> struct _const_view_or_type { using type = T; };
    template <typename T> struct _const_view_or_type<T, std::void_t<typename T::const_view_type>> { using type = typename T::const_view_type; };
  } // namespace details

  template <typename T> using regular_t            = typename details::_regular<std::decay_t<T>>::type;
  template <typename T> using view_or_type_t       = typename details::_view_or_type<T>::type;
  template <typename T> using const_view_or_type_t = typename details::_const_view_or_type<T>::type;

  /// Transform to the regular type
  template <typename A> regular_t<A> make_regular(A &&x) { return std::forward<A>(x); }

} //namespace triqs
