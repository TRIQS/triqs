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
#ifndef TRIQS_UTILITY_EXPRESSION_TEMPLATE_TOOLS_H
#define TRIQS_UTILITY_EXPRESSION_TEMPLATE_TOOLS_H
#include <type_traits>
#include <complex>
#include "./macros.hpp"

namespace triqs { namespace utility {

 template<class T> struct remove_rvalue_ref      {typedef T type;};
 template<class T> struct remove_rvalue_ref<T&>  {typedef T const & type;};
 template<class T> struct remove_rvalue_ref<T&&> {typedef T type;};
 template<class T> using remove_rvalue_ref_t = typename remove_rvalue_ref<T>::type;

 namespace tags { struct plus{}; struct minus{}; struct multiplies{}; struct divides{}; } 

 // The basic operations put in a template.... 
 template<typename Tag> struct operation;
 template<> struct operation<tags::plus> { 
  template<typename L, typename R> auto operator()(L && l, R && r) const DECL_AND_RETURN(std::forward<L>(l) + std::forward<R>(r));
  static const char name = '+';
 };
 template<> struct operation<tags::minus> { 
  template<typename L, typename R> auto operator()(L && l, R && r) const DECL_AND_RETURN(std::forward<L>(l) - std::forward<R>(r));
  static const char name = '-';
 };
 template<> struct operation<tags::multiplies> { 
  template<typename L, typename R> auto operator()(L && l, R && r) const DECL_AND_RETURN(std::forward<L>(l) * std::forward<R>(r));
  static const char name = '*';
 };
 template<> struct operation<tags::divides> { 
  template<typename L, typename R> auto operator()(L && l, R && r) const DECL_AND_RETURN(std::forward<L>(l) / std::forward<R>(r));
  static const char name = '/';
 };

 // The scalar ...
 template<typename T> struct is_in_ZRC : std::is_arithmetic<T>  {};
 template<> struct is_in_ZRC<bool> : std::true_type {};
 template<typename T> struct is_in_ZRC<std::complex<T> > : std::true_type {};
 template<typename T> struct is_in_ZRC<T&> : is_in_ZRC<T>{};
 template<typename T> struct is_in_ZRC<T&&> : is_in_ZRC<T>{};
 template<typename T> struct is_in_ZRC<const T> : is_in_ZRC<T>{};

 template<typename A,  typename B> 
  struct type_of_mult{
   typedef decltype ( std::declval<typename std::remove_reference<A>::type>() * std::declval<typename std::remove_reference<B>::type>() ) type;
  };


}}//namespace triqs::utility
#endif
