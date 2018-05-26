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
#ifndef TRIQS_ARRAYS_EXPRESSION_MAP_H
#define TRIQS_ARRAYS_EXPRESSION_MAP_H
#include "../impl/common.hpp"
#include <functional>
//#include "../../utility/function_arg_ret_type.hpp"

namespace triqs { namespace arrays { 
 
 template<class F, int arity=F::arity> class map_impl;

 /** 
  * Given a function f : arg_type -> result_type, map(f) is the function promoted to arrays
  * map(f) : array<arg_type, N, Opt> --> array<result_type, N, Opt> 
  */
 //template<class F> map_impl<F,utility::function_arg_ret_type<F>::arity> map  (F f) { return {std::move(f),true}; }

 template<class F> map_impl<F,1> map  (F f) { return {std::move(f),true}; }
 template<class F> map_impl<F,2> map2 (F f) { return {std::move(f),true}; }
 
 // ----------- implementation  -------------------------------------

 template<typename F, int arity, bool is_vec, typename ... A> struct map_impl_result;

 template<typename F, bool is_vec, typename A> struct map_impl_result<F,1,is_vec,A> { 
  typedef typename std::result_of<F(typename std::remove_reference<A>::type::value_type)>::type value_type;
  typedef typename std::remove_reference<A>::type::domain_type domain_type;
  F f; 
  typename std::add_const<A>::type a; // A is a T or a T& : add const to them.
  domain_type domain() const { return a.domain(); } 
  template<typename ... Args> value_type operator() (Args && ... args) const { return f(a(std::forward<Args>(args)...)); }
  friend std::ostream &operator<<(std::ostream &out, map_impl_result const &x) { return out << array<value_type, std14::decay_t<A>::domain_type::rank>(x);} 
  // rest is only for vector 
  template<bool vec = is_vec> 
   TYPE_ENABLE_IFC(size_t,vec) size() const { return a.size();}
  template<typename Args, bool vec=is_vec>  
   TYPE_ENABLE_IFC(value_type,vec) operator[] (Args && args) const { return f(a[std::forward<Args>(args)]);}
 };

 // possible to generalize to N order using tuple techniques ...
 template<typename F, bool is_vec, typename A, typename B> struct map_impl_result<F,2,is_vec,A,B> { 
  typedef typename std::result_of<F(typename remove_cv_ref<A>::type::value_type, typename remove_cv_ref<B>::type::value_type)>::type value_type;
  typedef typename remove_cv_ref<A>::type::domain_type domain_type;
  F f; 
  typename std::add_const<A>::type a;
  typename std::add_const<B>::type b;
  domain_type domain() const { return a.domain(); } 
  template<typename ... Args> value_type operator() (Args && ... args) const { return f(a(std::forward<Args>(args)...),b(std::forward<Args>(args)...)); }
  friend std::ostream & operator<<(std::ostream & out, map_impl_result const & x){  return out << array<value_type, std14::decay_t<A>::domain_type::rank>(x);} 
  // rest is only for vector 
  template<bool vec = is_vec> 
   TYPE_ENABLE_IFC(size_t,vec) size() const { return a.size();}
  template<typename Args, bool vec=is_vec>  
   TYPE_ENABLE_IFC(value_type,vec) operator[] (Args && args) const { return f(a[std::forward<Args>(args)],b[std::forward<Args>(args)]);}
 };

 /* already defined in traist
 template<typename ... T> struct _and; 
 template<typename T0, typename ... T> struct _and<T0, T...> : std::integral_constant<bool, T0::value && _and<T...>::value>{}; 
 template<typename T> struct _and<T> : T{};
*/
 template<typename F, int arity, bool b, typename ... A> struct ImmutableCuboidArray<map_impl_result<F,arity,b,A...>> : std::true_type{};
 
 template<typename F, int arity, bool b, typename ... A> struct ImmutableArray <map_impl_result<F,arity,b,A...>> : _and<typename ImmutableArray <typename std::remove_reference<A>::type>::type...>{};
 template<typename F, int arity, bool b, typename ... A> struct ImmutableMatrix<map_impl_result<F,arity,b,A...>> : _and<typename ImmutableMatrix<typename std::remove_reference<A>::type>::type...>{};
 template<typename F, int arity, bool b, typename ... A> struct ImmutableVector<map_impl_result<F,arity,b,A...>> : _and<typename ImmutableVector<typename std::remove_reference<A>::type>::type...>{};
 
 //template<typename F, int arity, bool b, typename A> struct ImmutableArray <map_impl_result<F,arity,b,A>> : ImmutableArray <A>{};
 //template<typename F, int arity, bool b, typename A> struct ImmutableMatrix<map_impl_result<F,arity,b,A>> : ImmutableMatrix<A>{};
 //template<typename F, int arity, bool b, typename A> struct ImmutableVector<map_impl_result<F,arity,b,A>> : ImmutableVector<A>{};

 // NB The bool is to make constructor not ambiguous
 // clang on os X with lib++ has a pb otherwise (not clear what the pb is)
 template<class F, int arity> class map_impl  { 
  F f;
  public :   
  map_impl(F f_, bool):f(std::move(f_)) {}
  map_impl(map_impl const &) = default;
  map_impl(map_impl &&)      = default;
  map_impl & operator = (map_impl const &) = default;
  map_impl & operator = (map_impl &&)      = default;

  template<typename ... A> map_impl_result<F,arity,_and<typename ImmutableVector<A>::type...>::value,A...> 
   operator()(A&&... a) const { return {f,std::forward<A>(a)...}; } 

  friend std::ostream & operator<<(std::ostream & out, map_impl const & x){ return out<<"map("<<"F"<<")";}
 };

}}//namespace triqs::arrays
#endif



