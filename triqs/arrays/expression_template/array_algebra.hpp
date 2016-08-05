/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011-2013 by O. Parcollet
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

 // a trait to compute the return type of the operator()(0,...0) const for anything with the ImmutableCuboidArray concept
 template<typename A, int R = get_rank<A>::value> struct get_call_const_return_type;
 template<typename A> struct get_call_const_return_type<A,0> { typedef decltype(std::declval<A const>()()) type; };
 template<typename A> struct get_call_const_return_type<A,1> { typedef decltype(std::declval<A const>()(0)) type; };
 template<typename A> struct get_call_const_return_type<A,2> { typedef decltype(std::declval<A const>()(0,0)) type; };
 template<typename A> struct get_call_const_return_type<A,3> { typedef decltype(std::declval<A const>()(0,0,0)) type; };
 template<typename A> struct get_call_const_return_type<A,4> { typedef decltype(std::declval<A const>()(0,0,0,0)) type; };
 template<typename A> struct get_call_const_return_type<A,5> { typedef decltype(std::declval<A const>()(0,0,0,0,0)) type; };
 template<typename A> struct get_call_const_return_type<A,6> { typedef decltype(std::declval<A const>()(0,0,0,0,0,0)) type; };
 template<typename A> struct get_call_const_return_type<A,7> { typedef decltype(std::declval<A const>()(0,0,0,0,0,0,0)) type; };
 template<typename A> struct get_call_const_return_type<A,8> { typedef decltype(std::declval<A const>()(0,0,0,0,0,0,0,0)) type; };
 template<typename A> struct get_call_const_return_type<A,9> { typedef decltype(std::declval<A const>()(0,0,0,0,0,0,0,0,0)) type; };

 template<typename Tag, typename L, typename R> 
  struct array_expr : TRIQS_CONCEPT_TAG_NAME(ImmutableArray) { 
   typedef typename std::remove_reference<L>::type L_t;
   typedef typename std::remove_reference<R>::type R_t;
   static_assert( get_rank<R_t>::value==0 || get_rank<L_t>::value==0 || get_rank<L_t>::value == get_rank<R_t>::value, "rank mismatch in array operations");
   typedef typename std::result_of<utility::operation<Tag>(typename get_call_const_return_type<L_t>::type,typename get_call_const_return_type<R_t>::type)>::type  value_type;
   //typedef typename std::result_of<utility::operation<Tag>(typename L_t::value_type,typename R_t::value_type)>::type  value_type;
   typedef typename std::remove_reference<typename std::result_of<combine_domain(L_t,R_t)>::type>::type domain_type;

   L l; R r;
   template<typename LL, typename RR> array_expr(LL && l_, RR && r_) : l(std::forward<LL>(l_)), r(std::forward<RR>(r_)) {}

   domain_type domain() const  { return combine_domain()(l,r); } 
   //template<typename ... Args> auto operator()(Args && ... args) const DECL_AND_RETURN( utility::operation<Tag>()(l(std::forward<Args>(args)...) , r(std::forward<Args>(args)...)));

 template <typename... Args>
   //require(!clef::is_any_lazy<Args...>)
   std14::enable_if_t<!clef::is_any_lazy<Args...>::value, value_type> operator()(Args &&... args) const {
    return utility::operation<Tag>()(l(std::forward<Args>(args)...), r(std::forward<Args>(args)...));
   }

   TRIQS_CLEF_IMPLEMENT_LAZY_CALL();

   friend std::ostream &operator<<(std::ostream &sout, array_expr const &expr) {
    return sout << "(" << expr.l << " " << utility::operation<Tag>::name << " " << expr.r << ")";
   }

   friend array<value_type, domain_type::rank> make_array(array_expr const &e) { return e; }
   friend array<value_type, domain_type::rank> make_regular(array_expr const &x) { return make_array(x); }
   friend array_const_view<value_type, domain_type::rank> make_const_view(array_expr const &x) { return make_array(x); }

   // just for better error messages
   template <typename T> void operator=(T &&x) = delete; // can not assign to an expression template !
   template <typename T> void operator+=(T &&x) = delete;// can not += into an expression template !
   template <typename T> void operator-=(T &&x) = delete;// can not -= into an expression template !
   };

 // a special case : the unary operator !
 template<typename L>   
  struct array_unary_m_expr : TRIQS_CONCEPT_TAG_NAME(ImmutableArray) { 
  typedef typename std::remove_reference<L>::type L_t;
  typedef typename L_t::value_type value_type;
  typedef typename L_t::domain_type domain_type;

  L l; 
  template<typename LL> array_unary_m_expr(LL && l_) : l(std::forward<LL>(l_)) {}

  domain_type domain() const  { return l.domain(); } 
  template<typename ... Args> value_type operator()(Args && ... args) const { return -l(std::forward<Args>(args)...);}

  friend std::ostream &operator <<(std::ostream &sout, array_unary_m_expr const &expr){return sout << '-'<<expr.l; }
  friend array<value_type, domain_type::rank> make_array(array_unary_m_expr const & e) { return e;}
  // just for better error messages
  template <typename T> void operator=(T &&x) = delete;  // can not assign to an expression template !
  template <typename T> void operator+=(T &&x) = delete; // can not += into an expression template !
  template <typename T> void operator-=(T &&x) = delete; // can not -= into an expression template !
  };

 // Now we can define all the C++ operators ...
#define DEFINE_OPERATOR(TAG, OP, TRAIT1, TRAIT2) \
 template<typename A1, typename A2>\
 typename std::enable_if<TRAIT1<A1>::value && TRAIT2 <A2>::value, \
 array_expr<utility::tags::TAG, typename node_t<A1,false>::type, typename node_t<A2,false>::type>>::type\
 operator OP (A1 && a1, A2 && a2) { return {std::forward<A1>(a1),std::forward<A2>(a2)};} 

 DEFINE_OPERATOR(plus,       +, ImmutableArray,ImmutableArray);
 DEFINE_OPERATOR(minus,      -, ImmutableArray,ImmutableArray);
 DEFINE_OPERATOR(multiplies, *, ImmutableArray,ImmutableArray);
 DEFINE_OPERATOR(multiplies, *, is_in_ZRC,ImmutableArray);
 DEFINE_OPERATOR(multiplies, *, ImmutableArray,is_in_ZRC);
 DEFINE_OPERATOR(divides,    /, ImmutableArray,ImmutableArray);
 DEFINE_OPERATOR(divides,    /, is_in_ZRC,ImmutableArray);
 DEFINE_OPERATOR(divides,    /, ImmutableArray,is_in_ZRC);

 // with scalar
 DEFINE_OPERATOR(plus,       +, ImmutableArray,is_in_ZRC);
 DEFINE_OPERATOR(plus,       +, is_in_ZRC,ImmutableArray);
 DEFINE_OPERATOR(minus,      -, ImmutableArray,is_in_ZRC);
 DEFINE_OPERATOR(minus,      -, is_in_ZRC,ImmutableArray);


#undef DEFINE_OPERATOR

 // the unary is special
 template<typename A1> 
  typename std::enable_if< 
  ImmutableArray<A1>::value, 
  array_unary_m_expr<typename node_t<A1,false>::type >
   >::type
   operator - (A1 && a1) { return {std::forward<A1>(a1)};}


 // inverse of an array
 //template <class A> struct __inv_array_rtype { using type = decltype(1 / std::declval<A>()); };
 //template <class A> struct __inv_array_rtype { using type = array_expr<utility::tags::divides, typename node_t<int, false>::type, typename node_t<A, false>::type>;};

 template <class A>
     std14::enable_if_t < ImmutableArray<std14::decay_t<A>>::value,
     array_expr<utility::tags::divides, _scalar_wrap<int, false>, utility::remove_rvalue_ref_t<A>>> inverse(A &&a) {
  return {1 , std::forward<A>(a)};
 }
}}//namespace triqs::arrays
#endif
