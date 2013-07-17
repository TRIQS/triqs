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
  class fold_worker  { 
   F f;
   typedef typename boost::remove_const<typename boost::remove_reference<typename F::result_type >::type>::type result_type;
   
   template<class A>
    struct fold_func_adaptor { 
     A const & b;
     F f; result_type r;
     fold_func_adaptor(F f_, A const & a_,typename A::value_type init ):b(a_), f(f_) { r= init;}
     template<typename ... Args> void operator()(Args const & ... args) { r = f(r,b(args...)); }
    };

   public:

   fold_worker ( F const & f_):f(f_) {} 

   template<class A>   
    result_type operator() (A const & a, typename A::value_type init = typename A::value_type() )  const { 
     fold_func_adaptor<A> func(f,a,init);
     //fold_func_adaptor<typename A::value_type> func(f,init);
     foreach(a,std::ref(func));
     //foreach(a,boost::ref(func));
     return func.r;
    }
  };

 template<class F> fold_worker<F> fold (F const & f) { return fold_worker<F>(f);}

 template<typename R, typename A1> 
  fold_worker< boost::function<R(A1,A1)> >
  fold (R (*f)(A1,A1)) { return fold( boost::function<R(A1,A1)>(f)); } 

}}//namespace triqs::arrays

#endif

