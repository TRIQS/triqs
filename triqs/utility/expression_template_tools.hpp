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
namespace triqs { namespace utility {

 namespace tags { struct plus{}; struct minus{}; struct multiplies{}; struct divides{}; } 

// The basic operations put in a template.... 
 template<typename Tag> struct operation;
 template<> struct operation<tags::plus> { 
  template<typename L, typename R> auto operator()(L const & l, R const & r) const -> decltype(l+r) { return l+r;} 
  static const char name = '+';
 };
 template<> struct operation<tags::minus> { 
  template<typename L, typename R> auto operator()(L const & l, R const & r) const -> decltype(l-r) { return l-r;} 
  static const char name = '-';
 };
 template<> struct operation<tags::multiplies> { 
  template<typename L, typename R> auto operator()(L const & l, R const & r) const -> decltype(l*r) { return l*r;} 
  static const char name = '*';
 };
 template<> struct operation<tags::divides> { 
  template<typename L, typename R> auto operator()(L const & l, R const & r) const -> decltype(l/r) { return l/r;} 
  static const char name = '/';
 };

 // The scalar ...
 template<typename T> struct is_in_ZRC : std::is_arithmetic<T>  {};
 template<> struct is_in_ZRC<bool> : std::true_type {};
 template<typename T> struct is_in_ZRC<std::complex<T> > : std::true_type {};

 template<typename A,  typename B> 
  struct type_of_mult{
   typedef decltype ( std::declval<typename std::remove_reference<A>::type>() * std::declval<typename std::remove_reference<B>::type>() ) type;
  };

}}//namespace triqs::utility
#endif
