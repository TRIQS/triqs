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
#ifndef TRIQS_C14_FIX_H
#define TRIQS_C14_FIX_H
#include <memory>
#include <functional>

// a few that will be C++14, use in advance....

namespace std { 
 namespace c14 { 

  // use simply std::c14::plus<>() ...
  template<typename T = void> struct plus: std::plus<T>{};

  template<> struct plus<void> {
   template<typename T, typename U> 
    auto operator()( T&& t, U&& u) const DECL_AND_RETURN(std::forward<T>(t) + std::forward<U>(u));
  };

  template<typename T, typename... Args>
   std::unique_ptr<T> make_unique(Args&&... args) { return std::unique_ptr<T>(new T(std::forward<Args>(args)...)); }

  // a little helper class to wait for the correction that tuple construct is NOT explicit
  template<typename ... Args>
   class tuple : public std::tuple<Args...> { 
    public : 
     template<typename ... Args2>
     tuple(Args2 && ... args2) : std::tuple<Args...> (std::forward<Args2>(args2)...){}
   };
 }

 // minimal hack to get the metaprogramming work with this tuple too....
 template<int i, typename ... Args>
  auto get(c14::tuple<Args...> const & t) DECL_AND_RETURN( std::get<i>(static_cast<std::tuple<Args...>>(t)));

 template<typename ... Args> struct tuple_size<c14::tuple<Args...>>: tuple_size<std::tuple<Args...>>{};


}


#endif

