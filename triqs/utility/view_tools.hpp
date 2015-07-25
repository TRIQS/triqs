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
#include <functional>

namespace triqs {

 template<typename T> void nop(T ...){};
 template<typename T, typename Enable=void> struct has_view : std::false_type {};
 template <typename T>
 struct has_view<
     T, decltype(nop(std::declval<typename T::view_type>(), std::declval<typename T::regular_type>()))> : std::true_type {};

 template<typename T, bool HasView = has_view<T>::value> struct regular_type_if_exists_else_type;
 template<typename T> struct regular_type_if_exists_else_type<T,false> {using type=T;};
 template<typename T> struct regular_type_if_exists_else_type<T,true > {using type=typename T::regular_type;};

 template<typename T, bool HasView = has_view<T>::value> struct view_type_if_exists_else_type;
 template<typename T> struct view_type_if_exists_else_type<T,false> {using type=T;};
 template<typename T> struct view_type_if_exists_else_type<T,true > {using type=typename T::view_type;};

 template<typename T, bool HasView = has_view<T>::value> struct const_view_type_if_exists_else_type;
 template<typename T> struct const_view_type_if_exists_else_type<T,false> {using type=T;};
 template<typename T> struct const_view_type_if_exists_else_type<T,true>  {using type=const typename T::const_view_type;};

  /// Transform to the regular type
 template <typename A> typename regular_type_if_exists_else_type<A>::type make_regular(A const &x) { return x; }

}//namespace triqs

