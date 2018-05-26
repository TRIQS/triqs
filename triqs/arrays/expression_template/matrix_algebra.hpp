/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011-2016 by O. Parcollet
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
#include "../linalg/det_and_inverse.hpp"
#include "../blas_lapack/gemv.hpp"
#include "../blas_lapack/gemm.hpp"
namespace triqs { namespace arrays {

 // matrix * matrix 
 template<typename A, typename B, typename Enable = void>  struct _matmul_rvalue {};

 template<typename A, typename B>  struct _matmul_rvalue<A,B, ENABLE_IFC(ImmutableMatrix<A>::value && ImmutableMatrix<B>::value)> { 
  typedef typename std::remove_const<typename A::value_type>::type V1;
  typedef typename std::remove_const<typename B::value_type>::type V2;
  typedef matrix<typename std::decay<decltype( V1{} * V2{})>::type> type;
 };

 template<typename A, typename B> 
  typename _matmul_rvalue<A,B>::type
  operator * (A const & a, B const & b) { 
   if (second_dim(a) != first_dim(b)) TRIQS_RUNTIME_ERROR<< "Matrix product : dimension mismatch in A*B "<< a<<" "<< b; 
   auto R = typename _matmul_rvalue<A,B>::type( first_dim(a), second_dim(b));
   blas::gemm(1.0,a, b, 0.0, R);
   return R;
  }

 // matrix * vector
 template<typename M, typename V, typename Enable = void>  struct _mat_vec_mul_rvalue {};

 template<typename M, typename V>  struct _mat_vec_mul_rvalue<M,V, ENABLE_IFC(ImmutableMatrix<M>::value && ImmutableVector<V>::value)> { 
  typedef typename std::remove_const<typename M::value_type>::type V1;
  typedef typename std::remove_const<typename V::value_type>::type V2;
  typedef vector<typename std::decay<decltype(V1{} * V2{})>::type> type;
 };

 template<typename M, typename V> 
  typename _mat_vec_mul_rvalue<M,V>::type
  operator * (M const & m, V const & v) { 
   if (second_dim(m) != v.size()) TRIQS_RUNTIME_ERROR<< "Matrix product : dimension mismatch in Matrix*Vector "<< m<<" "<< v; 
   auto R = typename _mat_vec_mul_rvalue<M,V>::type(first_dim(m));
   blas::gemv(1.0,m,v,0.0,R);
   return R;
  }

 // expression template 
 template<typename Tag, typename L, typename R> 
  struct matrix_expr : TRIQS_CONCEPT_TAG_NAME(ImmutableMatrix) { 
   typedef typename std::remove_reference<L>::type L_t;
   typedef typename std::remove_reference<R>::type R_t;
   static_assert( get_rank<R_t>::value==0 || get_rank<L_t>::value==0 || get_rank<L_t>::value == get_rank<R_t>::value, "rank mismatch in matrix operations");
   //typedef typename std::result_of<utility::operation<Tag>(typename L_t::value_type,typename R_t::value_type )>::type  value_type;
   typedef typename std::remove_cv< typename std::remove_reference<typename std::result_of<combine_domain(L_t,R_t)>::type>::type>::type domain_type;

   L l; R r;
   using value_type = decltype(utility::operation<Tag>()(l(0, 0), r(0, 0)));

   template<typename LL, typename RR> matrix_expr(LL && l_, RR && r_) : l(std::forward<LL>(l_)), r(std::forward<RR>(r_)) {}

   domain_type domain() const { return combine_domain()(l, r); }

   template <typename... Args>
   // require(!clef::is_any_lazy<Args...>)
   std14::enable_if_t<!clef::is_any_lazy<Args...>::value, value_type> operator()(Args &&... args) const {
    return utility::operation<Tag>()(l(std::forward<Args>(args)...), r(std::forward<Args>(args)...));
   }

   TRIQS_CLEF_IMPLEMENT_LAZY_CALL();

   friend std::ostream &operator <<(std::ostream &sout, matrix_expr const &expr){return sout << "("<<expr.l << " "<<utility::operation<Tag>::name << " "<<expr.r<<")" ; }

   friend matrix<value_type> make_regular(matrix_expr const &x) { return make_matrix(x); }
   friend matrix_const_view<value_type> make_const_view(matrix_expr const &x) { return make_matrix(x); }

   // just for better error messages
   template <typename T> void operator=(T &&x) = delete; // can not assign to an expression template !
   template <typename T> void operator+=(T &&x) = delete;// can not += into an expression template !
   template <typename T> void operator-=(T &&x) = delete;// can not -= into an expression template !
   };

 template<typename L>  // a special case : the unary operator !
  struct matrix_unary_m_expr : TRIQS_CONCEPT_TAG_NAME(ImmutableMatrix) { 
   typedef typename std::remove_reference<L>::type L_t;
   typedef typename L_t::value_type value_type;
   typedef typename L_t::domain_type domain_type;

   L l; 
   template<typename LL> matrix_unary_m_expr(LL && l_) : l(std::forward<LL>(l_)) {}

   domain_type domain() const  { return l.domain(); } 
   template<typename ... Args> value_type operator()(Args && ... args) const { return -l(std::forward<Args>(args)...);}
   friend std::ostream &operator <<(std::ostream &sout, matrix_unary_m_expr const &expr){return sout << '-'<<expr.l; }

   friend matrix<value_type> make_regular(matrix_unary_m_expr const &x) { return make_matrix(x); }
   friend matrix_const_view<value_type> make_const_view(matrix_unary_m_expr const &x) { return make_matrix(x); }
 
   // just for better error messages
   template <typename T> void operator=(T &&x) = delete; // can not assign to an expression template !
   template <typename T> void operator+=(T &&x) = delete;// can not += into an expression template !
   template <typename T> void operator-=(T &&x) = delete;// can not -= into an expression template !
   };

 // Now we can define all the C++ operators ...
#define DEFINE_OPERATOR(TAG, OP, TRAIT1, TRAIT2) \
 template<typename A1, typename A2>\
 typename std::enable_if<TRAIT1<A1>::value && TRAIT2 <A2>::value, \
 matrix_expr<utility::tags::TAG, typename node_t<A1,false>::type, typename node_t<A2,false>::type>>::type\
 operator OP (A1 && a1, A2 && a2) { return {std::forward<A1>(a1),std::forward<A2>(a2)};} 

 DEFINE_OPERATOR(plus,       +, ImmutableMatrix,ImmutableMatrix);
 DEFINE_OPERATOR(minus,      -, ImmutableMatrix,ImmutableMatrix);
 DEFINE_OPERATOR(multiplies, *, is_in_ZRC,ImmutableMatrix);
 DEFINE_OPERATOR(multiplies, *, ImmutableMatrix,is_in_ZRC);
 DEFINE_OPERATOR(divides,    /, ImmutableMatrix,is_in_ZRC);
#undef DEFINE_OPERATOR

 // the addition/substraction of diagonal matrix is special : all scalar are diagonal matrices here... 
#define DEFINE_OPERATOR(TAG, OP, TRAIT1, TRAIT2) \
 template<typename A1, typename A2>\
 typename std::enable_if<TRAIT1<A1>::value && TRAIT2 <A2>::value, \
 matrix_expr<utility::tags::TAG, typename node_t<A1,true>::type, typename node_t<A2,true>::type>>::type\
 operator OP (A1 && a1, A2 && a2) { return {std::forward<A1>(a1),std::forward<A2>(a2)};} 

 DEFINE_OPERATOR(plus,       +, ImmutableMatrix,is_in_ZRC);
 DEFINE_OPERATOR(plus,       +, is_in_ZRC,ImmutableMatrix);
 DEFINE_OPERATOR(minus,      -, ImmutableMatrix,is_in_ZRC);
 DEFINE_OPERATOR(minus,      -, is_in_ZRC,ImmutableMatrix);
#undef DEFINE_OPERATOR

 // the unary is special
 template<typename A1> 
  typename std::enable_if< 
  ImmutableMatrix<A1>::value, 
  matrix_unary_m_expr<typename node_t<A1,true>::type >
   >::type
   operator - (A1 && a1) { return {std::forward<A1>(a1)};} 


 template<typename M1, typename Enable = typename std::enable_if<ImmutableMatrix<M1>::value>::type > 
  struct _a_div_matrix {
    template<typename A, typename M> auto operator() (A && a, M && m) DECL_AND_RETURN ( std::forward<A>(a) * inverse(std::forward<M>(m)));
  };

 // FIXME : like array inverse, give explicit name of return type with matrix_expr
 template<typename A, typename M> // anything / matrix ---> anything * inverse(matrix)
 auto operator/ (A && a, M && m) DECL_AND_RETURN( _a_div_matrix<M>()( std::forward<A>(a), std::forward<M>(m)));

}}//namespace triqs::arrays
#endif

