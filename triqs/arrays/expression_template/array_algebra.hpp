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
#ifndef TRIQS_ARRAYS_EXPRESSION_ARRAY_ALGEBRA_H
#define TRIQS_ARRAYS_EXPRESSION_ARRAY_ALGEBRA_H
#include "./tools.hpp"
namespace triqs { namespace arrays {

 template<typename Tag, typename L, typename R> 
  struct array_expr : TRIQS_CONCEPT_TAG_NAME(ImmutableCuboidArray) { 
   typedef typename keeper_type<L>::type L_t;
   typedef typename keeper_type<R>::type R_t;
   static_assert( get_rank<R_t>::value==0 || get_rank<L_t>::value==0 || get_rank<L_t>::value == get_rank<R_t>::value, "rank mismatch in array operations");
   typedef typename std::result_of<operation<Tag>(typename L_t::value_type,typename R_t::value_type)>::type  value_type;
   typedef typename std::remove_reference<typename std::result_of<combine_domain(L_t,R_t)>::type>::type domain_type;
   
   L_t l; R_t r;
   template<typename LL, typename RR> array_expr(LL && l_, RR && r_) : l(std::forward<LL>(l_)), r(std::forward<RR>(r_)) {}
   
   domain_type domain() const  { return combine_domain()(l,r); } 
   
   //template<typename KeyType> value_type operator[](KeyType && key) const { return operation<Tag>()(l[std::forward<KeyType>(key)] , r[std::forward<KeyType>(key)]);}
   template<typename ... Args> value_type operator()(Args && ... args) const { return operation<Tag>()(l(std::forward<Args>(args)...) , r(std::forward<Args>(args)...));}
   friend std::ostream &operator <<(std::ostream &sout, array_expr const &expr){return sout << "("<<expr.l << " "<<operation<Tag>::name << " "<<expr.r<<")" ; }
  };

 // a special case : the unary operator !

  template<typename L>   struct array_unary_m_expr : TRIQS_CONCEPT_TAG_NAME(ImmutableCuboidArray) { 
   typedef typename keeper_type<L>::type L_t;
   typedef typename L_t::value_type value_type;
   typedef typename L_t::domain_type domain_type;
   
   L_t l; 
   template<typename LL> array_unary_m_expr(LL && l_) : l(std::forward<LL>(l_)) {}
   
   domain_type domain() const  { return l.domain(); } 
   
   //template<typename KeyType> value_type operator[](KeyType&& key) const {return -l[key];} 
   template<typename ... Args> value_type operator()(Args && ... args) const { return -l(std::forward<Args>(args)...);}
   friend std::ostream &operator <<(std::ostream &sout, array_unary_m_expr const &expr){return sout << '-'<<expr.l; }
  };
 
 // Now we can define all the C++ operators ...
#define DEFINE_OPERATOR(TAG, OP, TRAIT1, TRAIT2) \
 template<typename A1, typename A2>\
 typename std::enable_if<TRAIT1<A1>::value && TRAIT2 <A2>::value, array_expr<tags::TAG, A1,A2>>::type\
 operator OP (A1 const & a1, A2 const & a2) { return array_expr<tags::TAG, A1,A2>(a1,a2);} 

DEFINE_OPERATOR(plus,       +, ImmutableCuboidArray,ImmutableCuboidArray);
DEFINE_OPERATOR(minus,      -, ImmutableCuboidArray,ImmutableCuboidArray);
DEFINE_OPERATOR(multiplies, *, ImmutableCuboidArray,ImmutableCuboidArray);
DEFINE_OPERATOR(multiplies, *, is_in_ZRC,ImmutableCuboidArray);
DEFINE_OPERATOR(multiplies, *, ImmutableCuboidArray,is_in_ZRC);
DEFINE_OPERATOR(divides,    /, ImmutableCuboidArray,ImmutableCuboidArray);
DEFINE_OPERATOR(divides,    /, is_in_ZRC,ImmutableCuboidArray);
DEFINE_OPERATOR(divides,    /, ImmutableCuboidArray,is_in_ZRC);
#undef DEFINE_OPERATOR

 // the unary is special
 template<typename A1> typename std::enable_if<ImmutableCuboidArray<A1>::value, array_unary_m_expr<A1>>::type
  operator - (A1 const & a1) { return {a1};} 

 template<typename Expr > array<typename Expr::value_type, Expr::domain_type::rank>
  make_array( Expr const & e) { return array<typename Expr::value_type, Expr::domain_type::rank>(e);}

}}//namespace triqs::arrays
#endif
