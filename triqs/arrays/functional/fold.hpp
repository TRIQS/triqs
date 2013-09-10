/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012 by O. Parcollet
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
#ifndef TRIQS_ARRAYS_EXPRESSION_FOLD_H
#define TRIQS_ARRAYS_EXPRESSION_FOLD_H 
#include <boost/type_traits/remove_const.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/function.hpp>
#include "../array.hpp"

namespace triqs { namespace arrays {

 template<class F> 
  struct fold_worker  { 
   F f;
   
   template<class A, class R> struct fold_func_adaptor { 
     F const & f; A const & a; R & r;
     template<typename ... Args> void operator()(Args const & ... args) { r = f(r,a(args...)); }
    };

   template<class A, class R>   
    R operator() (A const & a, R init)  const { 
     foreach(a, fold_func_adaptor<A,R> {f,a,init});
     return init;
    }

   template<class A> typename A::value_type operator() (A const & a)  const { return (*this)(a, typename A::value_type{});}
  };

 template<class F> fold_worker<F> fold (F f) { return {std::move(f)};}

}}//namespace triqs::arrays

#endif

