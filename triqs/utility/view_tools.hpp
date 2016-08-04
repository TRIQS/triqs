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


 template<typename T, typename Enable=void> struct has_regular : std::false_type {};
 template <typename T> struct has_regular<T, decltype(nop(std::declval<typename T::regular_type>()))> : std::true_type {};

 // DEPRECATED
 template<typename T, bool HasRegular = has_regular<T>::value> struct regular_type_if_exists_else_type;
 template<typename T> struct regular_type_if_exists_else_type<T,false> {using type=T;};
 template<typename T> struct regular_type_if_exists_else_type<T,true > {using type=typename T::regular_type;};

 /// view_type_t<T> it T::view_type if T not const, T::const_view_type otherwise
 // decays the &, &&
 template <typename T> struct view_type_impl { using type = typename T::view_type; };
 template <typename T> struct view_type_impl<T &> : view_type_impl<T> {};
 template <typename T> struct view_type_impl<T &&> : view_type_impl<T> {};
 template <typename T> struct view_type_impl<T const> { using type = typename T::const_view_type; };
 template <typename T> using view_type_t = typename view_type_impl<T>::type;
 template <typename T> using const_view_type_t = typename view_type_t<T>::const_view_type;

 template<typename T, bool HasView = has_view<T>::value> struct view_type_if_exists_else_type;
 template<typename T> struct view_type_if_exists_else_type<T,false> {using type=T;};
 template<typename T> struct view_type_if_exists_else_type<T,true > {using type=typename T::view_type;};
 template<typename T> struct view_type_if_exists_else_type<T const,true > {using type=typename T::const_view_type;};

 template<typename T, bool HasView = has_view<T>::value> struct const_view_type_if_exists_else_type;
 template<typename T> struct const_view_type_if_exists_else_type<T,false> {using type=T;};
 template<typename T> struct const_view_type_if_exists_else_type<T,true>  {using type=typename T::const_view_type;};
 
 template<typename T> using view_or_type_t = typename view_type_if_exists_else_type<T>::type;
 template<typename T> using const_view_or_type_t = typename const_view_type_if_exists_else_type<T>::type;

 // CURRENT
 template<typename A> using Regular = typename regular_type_if_exists_else_type<A>::type;
 template<typename A> using get_regular_t = typename A::regular_type;

 /// Transform to the regular type
 template <typename A> Regular<A> make_regular(A &&x) { return std::forward<A>(x); }

}//namespace triqs

