/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012 by M. Ferrero, O. Parcollet
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
#ifndef TRIQS_STD_VECTOR_EXPR_TEMPL_H
#define TRIQS_STD_VECTOR_EXPR_TEMPL_H
#include <triqs/utility/expression_template_tools.hpp>
#include <triqs/utility/concept_tools.hpp>
#include <triqs/utility/exceptions.hpp>
#include <vector>
#include <iostream>

namespace triqs { namespace utility {  

 // a trait to identify the std::vector ... 
 TRIQS_DEFINE_CONCEPT_AND_ASSOCIATED_TRAIT(ImmutableStdVector);
 template<typename T> struct ImmutableStdVector<std::vector<T>> : std::true_type{};

 namespace expr_temp_vec_tools { 
  template<typename S> struct scalar_wrap {// S can be a T or a T &
   typedef typename std::remove_reference<S>::type value_type; 
   S s; 
   template<typename T> scalar_wrap(T && x):s(std::forward<T>(x)){}  
   S operator[](size_t i) const {return s;}
   friend std::ostream &operator <<(std::ostream &sout, scalar_wrap const &expr){return sout << expr.s;}
  };

  // Combine the two sizes of LHS and RHS : need to specialize where there is a scalar
  struct combine_size {
   template<typename L, typename R> 
    size_t operator() (L const & l, R const & r) const { 
     if (!(l.size() == r.size())) TRIQS_RUNTIME_ERROR << "size mismatch : ";//<< l.size()<<" vs" <<r.size();
     return l.size();
    }
   template<typename S, typename R> size_t operator() (scalar_wrap<S> const &, R const & r) const {return r.size();}
   template<typename S, typename L> size_t operator() (L const & l, scalar_wrap<S> const &) const {return l.size();}
  };

  template<typename T> struct node_t : std::conditional<utility::is_in_ZRC<T>::value, scalar_wrap<T>, typename remove_rvalue_ref<T>::type> {};

  template<typename V>
   auto make_vector(V const & v) -> std::vector<typename std::remove_reference<decltype(v[0])>::type> { 
   //auto make_vector(V const & v) -> std::vector<typename std::remove_reference<decltype(static_cast<V const &>(v)[0])>::type> { 
   std::vector<typename std::remove_reference<decltype(v[0])>::type> res;
   std::cout  << "makeVector"<< std::endl; 
   res.reserve(v.size());
   for (size_t i =0; i<v.size(); ++i) res.push_back(v[i]);
   return res;
  }

 }// std_vec_expr_tools
}}

namespace std { // no choice since the operators are found by ADL..

 template<typename Tag, typename L, typename R>  struct std_vec_expr : triqs::utility::ImmutableStdVector__concept_tag {
  L l; R r;
  template<typename LL, typename RR> std_vec_expr(LL && l_, RR && r_) : l(std::forward<LL>(l_)), r(std::forward<RR>(r_)) {}
  std_vec_expr(std_vec_expr const &) = delete;
  std_vec_expr(std_vec_expr &&)      = default;
  size_t size() const  { return triqs::utility::expr_temp_vec_tools::combine_size()(l,r); } 
  auto operator[](size_t i) const DECL_AND_RETURN(triqs::utility::operation<Tag>()(l[i] , r[i]));
  friend std::ostream &operator <<(std::ostream &sout, std_vec_expr const &expr){return sout << "("<<expr.l <<" "<<triqs::utility::operation<Tag>::name <<" "<<expr.r<<")";}
 };

 template<typename Tag, typename L, typename R> 
 auto make_vector(std_vec_expr<Tag,L,R> const & v) DECL_AND_RETURN(triqs::utility::expr_temp_vec_tools::make_vector(v));
 
 // -------------------------------------------------------------------
 //a special case : the unary operator !
 template<typename L> struct std_vec_expr_unary : triqs::utility::ImmutableStdVector__concept_tag {
  L l; 
  template<typename LL> std_vec_expr_unary(LL && l_) : l(std::forward<LL>(l_)) {}
  std_vec_expr_unary(std_vec_expr_unary const &) = delete;
  std_vec_expr_unary(std_vec_expr_unary &&)      = default;
  size_t size() const  { return l.size(); } 
  auto operator[](size_t i) const DECL_AND_RETURN( -l[i]);
  friend std::ostream &operator <<(std::ostream &sout, std_vec_expr_unary const &expr){return sout << '-'<<expr.l; }
 };
 
 template<typename L> 
  auto make_vector(std_vec_expr_unary<L> const & v) DECL_AND_RETURN(triqs::utility::expr_temp_vec_tools::make_vector(v));

 // -------------------------------------------------------------------
 // Now we can define all the C++ operators ...
#define DEFINE_OPERATOR(TAG, OP, TRAIT1, TRAIT2) \
 template<typename A1, typename A2>\
 typename std::enable_if<triqs::utility::TRAIT1<A1>::value && triqs::utility::TRAIT2 <A2>::value, \
 std_vec_expr<triqs::utility::tags::TAG, typename triqs::utility::expr_temp_vec_tools::node_t<A1>::type, typename triqs::utility::expr_temp_vec_tools::node_t<A2>::type>>::type\
 operator OP (A1 && a1, A2 && a2) { return {std::forward<A1>(a1),std::forward<A2>(a2)};} 

 DEFINE_OPERATOR(plus,       +, ImmutableStdVector,ImmutableStdVector);
 DEFINE_OPERATOR(minus,      -, ImmutableStdVector,ImmutableStdVector);
 DEFINE_OPERATOR(multiplies, *, ImmutableStdVector,ImmutableStdVector);
 DEFINE_OPERATOR(multiplies, *, is_in_ZRC,ImmutableStdVector);
 DEFINE_OPERATOR(multiplies, *, ImmutableStdVector,is_in_ZRC);
 DEFINE_OPERATOR(divides,    /, ImmutableStdVector,ImmutableStdVector);
 DEFINE_OPERATOR(divides,    /, is_in_ZRC,ImmutableStdVector);
 DEFINE_OPERATOR(divides,    /, ImmutableStdVector,is_in_ZRC);
#undef DEFINE_OPERATOR

 // the unary is special
 template<typename A1> typename std::enable_if<triqs::utility::ImmutableStdVector<A1>::value, std_vec_expr_unary<A1>>::type
  operator - (A1 const & a1) { return  std_vec_expr_unary<A1>(a1);} 
}

/*
namespace triqs { namespace utility {  namespace expr_temp_vec_tools { 

 //make sure expression are never taken by const &
 template<typename Tag, typename L, typename R>  struct expr_node_storage_t <std::std_vec_expr<Tag,L,R> &>      ;// { typedef std::std_vec_expr<Tag,L,R> type;};
 template<typename Tag, typename L, typename R>  struct expr_node_storage_t <std::std_vec_expr<Tag,L,R> const &> ;//{ typedef std::std_vec_expr<Tag,L,R> type;};

 template<typename L>  struct expr_node_storage_t <std::std_vec_expr_unary<L> &>      ;// { typedef std::std_vec_expr_unary<L> type;};
 template<typename L>  struct expr_node_storage_t <std::std_vec_expr_unary<L> const &>;// { typedef std::std_vec_expr_unary<L> type;};

 }// std_vec_expr_tools
 }}
*/

#endif



