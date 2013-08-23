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
#ifndef TRIQS_ARRAYS_EXPRESSION_MATRIX_ALGEBRA_H
#define TRIQS_ARRAYS_EXPRESSION_MATRIX_ALGEBRA_H
#include "./vector_algebra.hpp"
#include "../matrix.hpp"
#include "../linalg/matmul.hpp"
#include "../linalg/mat_vec_mul.hpp"
#include "../linalg/det_and_inverse.hpp"
namespace triqs { namespace arrays {

 template<typename Tag, typename L, typename R, bool scalar_are_diagonal_matrices= false> 
  struct matrix_expr : TRIQS_CONCEPT_TAG_NAME(ImmutableMatrix) { 
   typedef typename keeper_type<L,scalar_are_diagonal_matrices>::type L_t;
   typedef typename keeper_type<R,scalar_are_diagonal_matrices>::type R_t;
   static_assert( get_rank<R_t>::value==0 || get_rank<L_t>::value==0 || get_rank<L_t>::value == get_rank<R_t>::value, "rank mismatch in matrix operations");
   typedef typename std::result_of<operation<Tag>(typename L_t::value_type,typename R_t::value_type)>::type  value_type;
   typedef typename std::remove_cv< typename std::remove_reference<typename std::result_of<combine_domain(L_t,R_t)>::type>::type>::type domain_type;
   
   L_t l; R_t r;
   template<typename LL, typename RR> matrix_expr(LL && l_, RR && r_) : l(std::forward<LL>(l_)), r(std::forward<RR>(r_)) {}

   domain_type domain() const  { return combine_domain()(l,r); } 

   //template<typename KeyType> value_type operator[](KeyType && key) const { return operation<Tag>()(l[std::forward<KeyType>(key)] , r[std::forward<KeyType>(key)]);}
   template<typename ... Args> value_type operator()(Args && ... args) const { return operation<Tag>()(l(std::forward<Args>(args)...) , r(std::forward<Args>(args)...));}
   friend std::ostream &operator <<(std::ostream &sout, matrix_expr const &expr){return sout << "("<<expr.l << " "<<operation<Tag>::name << " "<<expr.r<<")" ; }
  };

 template<typename L>  // a special case : the unary operator !
  struct matrix_unary_m_expr : TRIQS_CONCEPT_TAG_NAME(ImmutableMatrix) { 
   typedef typename keeper_type<L>::type L_t;
   typedef typename L_t::value_type value_type;
   typedef typename L_t::domain_type domain_type;
   L_t l; 
   template<typename LL> matrix_unary_m_expr(LL && l_) : l(std::forward<LL>(l_)) {}

   domain_type domain() const  { return l.domain(); } 

   //template<typename KeyType> value_type operator[](KeyType&& key) const {return -l[key];} 
   template<typename ... Args> value_type operator()(Args && ... args) const { return -l(std::forward<Args>(args)...);}
   friend std::ostream &operator <<(std::ostream &sout, matrix_unary_m_expr const &expr){return sout << '-'<<expr.l; }
  };

 // Now we can define all the C++ operators ...
#define DEFINE_OPERATOR(TAG, OP, TRAIT1, TRAIT2) \
 template<typename A1, typename A2>\
 typename std::enable_if<TRAIT1<A1>::value && TRAIT2 <A2>::value, matrix_expr<tags::TAG, A1,A2>>::type\
 operator OP (A1 const & a1, A2 const & a2) { return matrix_expr<tags::TAG, A1,A2>(a1,a2);} 

 DEFINE_OPERATOR(plus,       +, ImmutableMatrix,ImmutableMatrix);
 DEFINE_OPERATOR(minus,      -, ImmutableMatrix,ImmutableMatrix);
 DEFINE_OPERATOR(minus,      -, ImmutableMatrix,is_in_ZRC);
 DEFINE_OPERATOR(minus,      -, is_in_ZRC,ImmutableMatrix);
 DEFINE_OPERATOR(multiplies, *, is_in_ZRC,ImmutableMatrix);
 DEFINE_OPERATOR(multiplies, *, ImmutableMatrix,is_in_ZRC);
 DEFINE_OPERATOR(divides,    /, ImmutableMatrix,is_in_ZRC);
#undef DEFINE_OPERATOR

 // the addition/substraction of diagonal matrix is special : all scalar are diagonal matrices here... 
#define DEFINE_OPERATOR(TAG, OP, TRAIT1, TRAIT2) \
 template<typename A1, typename A2>\
 typename std::enable_if<TRAIT1<A1>::value && TRAIT2 <A2>::value, matrix_expr<tags::TAG, A1,A2,true>>::type\
 operator OP (A1 const & a1, A2 const & a2) { return matrix_expr<tags::TAG, A1,A2,true>(a1,a2);} 

 DEFINE_OPERATOR(plus,       +, ImmutableMatrix,is_in_ZRC);
 DEFINE_OPERATOR(plus,       +, is_in_ZRC,ImmutableMatrix);
 DEFINE_OPERATOR(minus,      -, ImmutableMatrix,is_in_ZRC);
 DEFINE_OPERATOR(minus,      -, is_in_ZRC,ImmutableMatrix);
#undef DEFINE_OPERATOR

 // the unary is special
 template<typename A1> typename std::enable_if<ImmutableMatrix<A1>::value, matrix_unary_m_expr<A1>>::type
  operator - (A1 const & a1) { return {a1};} 

 template<typename Expr > matrix <typename Expr::value_type>
  make_matrix( Expr const & e) { return matrix<typename Expr::value_type>(e);}

 template<typename M1, typename M2> // matrix * matrix
  typename boost::enable_if< mpl::and_<ImmutableMatrix<M1>, ImmutableMatrix<M2> >, matmul_lazy<M1,M2> >::type
  operator* (M1 const & a, M2 const & b) { return matmul_lazy<M1,M2>(a,b); }

 template<typename M, typename V> // matrix * vector
  typename boost::enable_if< mpl::and_<ImmutableMatrix<M>, ImmutableVector<V> >, mat_vec_mul_lazy<M,V> >::type
  operator* (M const & m, V const & v) { return mat_vec_mul_lazy<M,V>(m,v); }

 template<typename A, typename M> // anything / matrix ---> anything * inverse(matrix)
  typename boost::lazy_enable_if< ImmutableMatrix<M>, type_of_mult<A, inverse_lazy <M> > >::type
  operator/ (A const & a, M const & m) { return a * inverse(m);}

}}//namespace triqs::arrays
#endif

