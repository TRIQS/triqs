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
#include <memory>
#include <functional>
#include "./macros.hpp"

// backward compat. C++11 compilers.
// new stuff in 

// C++14 detection in gcc (4.9 like recent clang).
#if __cplusplus > 201103L

#ifndef __cpp_generic_lambdas
#define __cpp_generic_lambdas
#endif

#ifndef __cpp_decltype_auto
#define __cpp_decltype_auto
#endif

#ifndef __cpp_return_type_deduction 
#define __cpp_return_type_deduction 
#endif

#endif

// C++11 workaround
#if not defined(__cpp_return_type_deduction) or not defined(__cpp_decltype_auto)
#define TRIQS_C11
#define AUTO_DECL auto
#define RETURN(X,...) ->decltype(X) { __VA_ARGS__; return X;}
#else
#define AUTO_DECL decltype(auto)
#define RETURN(X,...) { __VA_ARGS__; return X;}
#endif 

namespace std { 
 namespace c14 {

  // helpers
  template <bool B, class T, class F> using conditional_t = typename conditional<B, T, F>::type;
  template <class T> using result_of_t = typename result_of<T>::type;
  template <class T> using remove_reference_t = typename remove_reference<T>::type;
  template <class T> using add_const_t = typename add_const<T>::type;
  template <class T> using remove_const_t = typename remove_const<T>::type;
  template <class T> using decay_t = typename decay<T>::type;
  template <bool B, class T = void> using enable_if_t = typename enable_if<B, T>::type;

  // use simply std::c14::plus<>() ...
  template<typename T = void> struct plus: std::plus<T>{};
  template<> struct plus<void> {
   template<typename T, typename U> 
    auto operator()( T&& t, U&& u) const DECL_AND_RETURN(std::forward<T>(t) + std::forward<U>(u));
  };

   // use simply std::c14::plus<>() ...
  template<typename T = void> struct greater: std::greater<T>{};
  template<> struct greater<void> {
   template<typename T, typename U> 
    auto operator()( T&& t, U&& u) const DECL_AND_RETURN(std::forward<T>(t) > std::forward<U>(u));
  };

  template<typename T = void> struct less: std::less<T>{};
  template<> struct less<void> {
   template<typename T, typename U> 
    auto operator()( T&& t, U&& u) const DECL_AND_RETURN(std::forward<T>(t) < std::forward<U>(u));
  };

  template<typename T, typename... Args>
   std::unique_ptr<T> make_unique(Args&&... args) { return std::unique_ptr<T>(new T(std::forward<Args>(args)...)); }

  //------------------------------------------------------------------------------------------
  //
  // sequence from C++14 : from gcc lib 4.9 
 /// Class template integer_sequence
  // Stores a tuple of indices.  Used by tuple and pair, and by bind() to
  // extract the elements in a tuple.
  template<size_t... _Indexes>
    struct _Index_tuple
    {
      typedef _Index_tuple<_Indexes..., sizeof...(_Indexes)> __next;
    };

  // Builds an _Index_tuple<0, 1, 2, ..., _Num-1>.
  template<size_t _Num>
    struct _Build_index_tuple
    {
      typedef typename _Build_index_tuple<_Num - 1>::__type::__next __type;
    };

  template<>
    struct _Build_index_tuple<0>
    {
      typedef _Index_tuple<> __type;
    };

  template<typename _Tp, _Tp... _Idx>
    struct integer_sequence
    {
      typedef _Tp value_type;
      static constexpr size_t size() { return sizeof...(_Idx); }
    };

  template<typename _Tp, _Tp _Num,
	   typename _ISeq = typename _Build_index_tuple<_Num>::__type>
    struct _Make_integer_sequence;

  template<typename _Tp, _Tp _Num,  size_t... _Idx>
    struct _Make_integer_sequence<_Tp, _Num, _Index_tuple<_Idx...>>
    {
      static_assert( _Num >= 0,
		     "Cannot make integer sequence of negative length" );

      typedef integer_sequence<_Tp, static_cast<_Tp>(_Idx)...> __type;
    };

  /// Alias template make_integer_sequence
  template<typename _Tp, _Tp _Num>
    using make_integer_sequence
      = typename _Make_integer_sequence<_Tp, _Num>::__type;

  /// Alias template index_sequence
  template<size_t... _Idx>
    using index_sequence = integer_sequence<size_t, _Idx...>;

  /// Alias template make_index_sequence
  template<size_t _Num>
    using make_index_sequence = make_integer_sequence<size_t, _Num>;

  /// Alias template index_sequence_for
  template<typename... _Types>
    using index_sequence_for = make_index_sequence<sizeof...(_Types)>;
//--------------------------------------------------

 }
}

namespace std14 { 
 using namespace std::c14;
}



