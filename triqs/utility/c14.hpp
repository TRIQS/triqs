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

// a few that will be C++14, use in advance....

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

 }
}

namespace std14 { 
 using namespace std::c14;
}
