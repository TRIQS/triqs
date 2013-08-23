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
#ifndef TRIQS_ARRAYS_MAKE_IMMUTABLE_ARRAY_H
#define TRIQS_ARRAYS_MAKE_IMMUTABLE_ARRAY_H
#include "./array.hpp"
namespace triqs { namespace arrays { 

 template<typename Expr, int ... ph>
  class immutable_array_impl : TRIQS_CONCEPT_TAG_NAME(ImmutableCuboidArray) { 
   template<int I> struct _si { typedef size_t type;};
   public : 
    immutable_array_impl(Expr e_, clef::pair<ph,range> ... p): 
     f(clef::make_function(e_, clef::placeholder<ph>()...)), dom_(make_shape(p.rhs().size()...)) {};
    typedef typename triqs::clef::result_of::make_function<Expr,clef::placeholder<ph>... >::type function_type;
    typedef typename std::result_of<function_type(typename _si<ph>::type...)>::type value_type;
    typedef indexmaps::cuboid::domain_t<sizeof...(ph)> domain_type;
    domain_type domain() const { return dom_;} 
    template<typename ... Args> value_type operator() (Args const &  ... args) const { return f(args...); } 
    friend std::ostream & operator<<(std::ostream & out, immutable_array_impl const & x){return out<<" immutable_array on domain : "<<x.domain();}
   protected:
    function_type f;
    domain_type dom_;
  };

 /** 
  * \brief Makes a lazy immutable (cuboid) array from a simple expression and a set of range...
  * \param expr The lazy expression
  * \param i_=R i_ is a placeholder, R a range. The i_=R produce a pair of i_ and R , which is the parameter.
  * \return A lazy object implementing the ImmutableCuboidArray concept with the domain built from the ranges. 
  */
 template<typename Expr, int ... ph>
  immutable_array_impl<Expr,ph...> make_immutable_array( Expr const & expr, clef::pair<ph,range> ... p) {
   return immutable_array_impl<Expr,ph...> (expr, p...);
  }

}}//namespace triqs::arrays
#endif

