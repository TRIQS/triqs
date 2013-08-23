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
#ifndef TRIQS_ARRAYS_EXPRESSION_A_X_TY_H
#define TRIQS_ARRAYS_EXPRESSION_A_X_TY_H
#include <boost/type_traits/is_same.hpp>
#include <boost/typeof/typeof.hpp>
#include "../matrix.hpp"
#include "../vector.hpp"
#include "../blas_lapack/ger.hpp"

namespace triqs { namespace arrays { 

 ///
 template<typename ScalarType, typename VectorType1, typename VectorType2> class a_x_ty_lazy;

 ///
 template<typename ScalarType, typename VectorType1, typename VectorType2>
  a_x_ty_lazy<ScalarType,VectorType1,VectorType2> a_x_ty (ScalarType a, VectorType1 const & x, VectorType2 const & y) 
  { return a_x_ty_lazy<ScalarType,VectorType1,VectorType2>(a,x,y); }

 //------------- IMPLEMENTATION -----------------------------------

 template<typename ScalarType, typename VectorType1, typename VectorType2> 
  class a_x_ty_lazy : TRIQS_CONCEPT_TAG_NAME(ImmutableMatrix) { 
   typedef typename boost::remove_const<typename VectorType1::value_type>::type V1;
   typedef typename boost::remove_const<typename VectorType2::value_type>::type V2;
   static_assert((boost::is_same<V1,V2>::value),"Different values : not implemented");

   public:
   typedef BOOST_TYPEOF_TPL( V1() * V2() * ScalarType()) value_type;
   typedef indexmaps::cuboid::domain_t<2> domain_type;
   typedef typename const_view_type_if_exists_else_type<VectorType1>::type X_type; 
   typedef typename const_view_type_if_exists_else_type<VectorType2>::type Y_type;
   const ScalarType a; const X_type x; const Y_type y; 

   public:
   a_x_ty_lazy( ScalarType a_, VectorType1 const & x_, VectorType2 const & y_):a(a_),x(x_),y(y_){}

   domain_type domain() const { return domain_type(mini_vector<size_t,2>(x.size(), y.size()));}

   template<typename K0, typename K1> value_type operator() (K0 const & k0, K1 const & k1) const { return a * x(k0) * y(k1); }

   // Optimized implementation of =
   template<typename LHS> 
    friend void triqs_arrays_assign_delegation (LHS & lhs, a_x_ty_lazy const & rhs)  {
     resize_or_check_if_view(lhs,make_shape( first_dim(rhs),second_dim(rhs) ));
     lhs()=0;
     blas::ger(rhs.a,rhs.x, rhs.y, lhs);
    }

   //Optimized implementation of +=
   template<typename LHS> 
    friend void triqs_arrays_compound_assign_delegation (LHS & lhs, a_x_ty_lazy const & rhs, mpl::char_<'A'>) {
     static_assert((is_matrix_or_view<LHS>::value), "LHS is not a matrix or a matrix_view"); // check that the target is indeed a matrix.
     blas::ger(rhs.a, rhs.x, rhs.y, lhs);
    }

   //Optimized implementation of -=
   template<typename LHS> 
    friend void triqs_arrays_compound_assign_delegation (LHS & lhs, a_x_ty_lazy const & rhs, mpl::char_<'S'>) { 
     static_assert((is_matrix_or_view<LHS>::value), "LHS is not a matrix or a matrix_view"); // check that the target is indeed a matrix.
     blas::ger(- rhs.a, rhs.x, rhs.y, lhs);
    }

   friend std::ostream & operator<<(std::ostream & out, a_x_ty_lazy const & a){ return out<<"a_x_ty("<<a.a<<","<<a.x<<","<<a.y<<")";}
  };
}} // namespace triqs_arrays 
#endif
