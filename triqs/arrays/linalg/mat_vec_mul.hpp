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
#ifndef TRIQS_ARRAYS_EXPRESSION_MAT_VEC_MUL_H
#define TRIQS_ARRAYS_EXPRESSION_MAT_VEC_MUL_H
#include <boost/type_traits/is_same.hpp>
#include <boost/typeof/typeof.hpp>
#include "../matrix.hpp"
#include "../vector.hpp"
#include "../blas_lapack/gemv.hpp"

namespace triqs { namespace arrays { 

 ///
 template<typename MT, typename VT> class mat_vec_mul_lazy;
 
 ///
 template<typename MT, typename VT> mat_vec_mul_lazy<MT,VT> mat_vec_mul (MT const & a, VT const & b) { return mat_vec_mul_lazy<MT,VT>(a,b); }
 
 // ----------------- implementation -----------------------------------------
 
 template<typename MT, typename VT> 
  class mat_vec_mul_lazy :  TRIQS_MODEL_CONCEPT(ImmutableVector) { 

   typedef typename MT::value_type V1;
   typedef typename VT::value_type V2;
   //static_assert((boost::is_same<V1,V2>::value),"Different values : not implemented");

   public:
   typedef BOOST_TYPEOF_TPL( V1() * V2()) value_type;
   typedef typename VT::domain_type  domain_type;
   typedef typename const_view_type_if_exists_else_type<MT>::type M_type;
   typedef typename const_view_type_if_exists_else_type<VT>::type V_type;
   const M_type M; const V_type V;

   private: 
   typedef vector<value_type> vector_type;

   struct internal_data {
    vector_type R;
    internal_data(mat_vec_mul_lazy const & P): R(P.M.dim0()) { blas::gemv(1,P.M,P.V,0,R); }
   };
   friend struct internal_data;
   mutable boost::shared_ptr<internal_data> _id;
   void activate() const { if (!_id) _id= boost::make_shared<internal_data>(*this);}

   public:
   mat_vec_mul_lazy( MT const & M_, VT const & V_):M(M_),V(V_){
    if (M.dim1() != V.size()) TRIQS_RUNTIME_ERROR<< "Matrix product : dimension mismatch in Matrix*Vector "<< M<<" "<< V; 
   }

   domain_type domain() const { return mini_vector<size_t,1>(size());}
   //domain_type domain() const { return indexmaps::cuboid::domain_t<1>(mini_vector<size_t,1>(size()));}
   size_t size() const { return M.dim0();} 

   template<typename KeyType> value_type operator() (KeyType const & key) const { activate(); return _id->R (key); }

   template<typename LHS> // Optimized implementation of =
    friend void triqs_arrays_assign_delegation (LHS & lhs, mat_vec_mul_lazy const & rhs)  {
     static_assert((is_vector_or_view<LHS>::value), "LHS is not a vector or a vector_view"); 
     resize_or_check_if_view(lhs,make_shape(rhs.size()));
     blas::gemv(1,rhs.M,rhs.V,0,lhs);
    }

   template<typename LHS> 
    friend void triqs_arrays_compound_assign_delegation (LHS & lhs, mat_vec_mul_lazy const & rhs, mpl::char_<'A'>)  { rhs.assign_comp_impl(lhs,1.0);}
   template<typename LHS> 
    friend void triqs_arrays_compound_assign_delegation (LHS & lhs, mat_vec_mul_lazy const & rhs, mpl::char_<'S'>)  { rhs.assign_comp_impl(lhs,-1.0);}

   private:   
   template<typename LHS> void assign_comp_impl (LHS & lhs, double S) const { 
    static_assert((is_vector_or_view<LHS>::value), "LHS is not a vector or a vector_view"); 
    if (lhs.size() != size()) TRIQS_RUNTIME_ERROR<< "mat_vec_mul : -=/-= operator : size mismatch in M*V "<< lhs.size()<<" vs "<< size(); 
     blas::gemv(1,M,V,S,lhs);    
   }
   friend std::ostream & operator<<(std::ostream & out, mat_vec_mul_lazy const & x){ return out<<"mat_vec_mul("<<x.M<<","<<x.V<<")";}
  };
}}//namespace triqs::arrays
#endif

