/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011-2012 by O. Parcollet
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
#ifndef TRIQS_ARRAYS_EXPRESSION_MATMUL_H
#define TRIQS_ARRAYS_EXPRESSION_MATMUL_H
#include <boost/type_traits/is_same.hpp>
#include <boost/typeof/typeof.hpp>
#include "../blas_lapack/gemm.hpp"
namespace triqs { namespace arrays {

 ///
 template<typename A, typename B> class matmul_lazy;
 ///
 template<typename A, typename B> matmul_lazy<A,B> matmul (A const & a, B const & b) { return matmul_lazy<A,B>(a,b); }
 
 // ----------------- implementation -----------------------------------------
 
 template<typename A, typename B> class matmul_lazy : TRIQS_MODEL_CONCEPT(ImmutableMatrix) {  
    typedef typename boost::remove_const<typename A::value_type>::type V1;
    typedef typename boost::remove_const<typename B::value_type>::type V2;
    //static_assert((boost::is_same<V1,V2>::value),"Different values : not implemented");

    public:
    typedef BOOST_TYPEOF_TPL( V1() * V2()) value_type; // what is the result of multiplying a V1 by a V2 ?
    typedef typename A::domain_type  domain_type;
    typedef typename const_view_type_if_exists_else_type<A>::type A_type; 
    typedef typename const_view_type_if_exists_else_type<B>::type B_type;
    const A_type a; const B_type b;

    private: 
    typedef matrix<value_type> matrix_type;

    struct internal_data { // implementing the pattern LazyPreCompute
     matrix_type R;
     internal_data(matmul_lazy const & P): R( P.a.dim0(), P.b.dim1()) { blas::gemm(1.0,P.a, P.b, 0.0, R); }
    };
    friend struct internal_data;
    mutable std::shared_ptr<internal_data> _id;
    void activate() const { if (!_id) _id= std::make_shared<internal_data>(*this);}

    public:
    matmul_lazy( A const & a_, B const & b_):a(a_),b(b_){ 
     if (a.dim1() != b.dim0()) TRIQS_RUNTIME_ERROR<< "Matrix product : dimension mismatch in A*B "<< a<<" "<< b; 
    }

    domain_type domain() const { return mini_vector<size_t,2>(a.dim0(), b.dim1());}
    //domain_type domain() const { return indexmaps::cuboid::domain_t<2>(mini_vector<size_t,2>(a.dim0(), b.dim1()));}
    size_t dim0() const { return a.dim0();} 
    size_t dim1() const { return b.dim1();} 

    template<typename K0, typename K1> value_type operator() (K0 const & k0, K1 const & k1) const { activate();  return _id->R(k0,k1); }

    // TO BE REMOVED because of the aliasing question
    // Optimized implementation of =, +=, -=
    template<typename LHS> 
    friend void triqs_arrays_assign_delegation (LHS & lhs, matmul_lazy const & rhs)  {
     static_assert((is_matrix_or_view<LHS>::value), "LHS is not a matrix");
     resize_or_check_if_view(lhs,make_shape(rhs.dim0(),rhs.dim1()));
     blas::gemm(1.0,rhs.a, rhs.b, 0.0, lhs);     
    }

    template<typename LHS> 
     friend void triqs_arrays_compound_assign_delegation (LHS & lhs, matmul_lazy const & rhs, mpl::char_<'A'>)  { rhs.assign_comp_impl(lhs,1.0);}
    template<typename LHS> 
     friend void triqs_arrays_compound_assign_delegation (LHS & lhs, matmul_lazy const & rhs, mpl::char_<'S'>)  { rhs.assign_comp_impl(lhs,-1.0);}

    private:   
    template<typename LHS> void assign_comp_impl (LHS & lhs, double S) const { 
     static_assert((is_matrix_or_view<LHS>::value), "LHS is not a matrix");
     if (lhs.dim0() != dim0()) 
      TRIQS_RUNTIME_ERROR<< "Matmul : +=/-= operator : first dimension mismatch in A*B "<< lhs.dim0()<<" vs "<< dim0(); 
     if (lhs.dim1() != dim1()) 
      TRIQS_RUNTIME_ERROR<< "Matmul : +=/-= operator : first dimension mismatch in A*B "<< lhs.dim1()<<" vs "<< dim1(); 
     blas::gemm(S,a, b, 1.0, lhs);     
    }

    friend std::ostream & operator<<(std::ostream & out, matmul_lazy<A,B> const & x){return out<<x.a<<" * "<<x.b;}

 };// class matmul_lazy

}}//namespace triqs::arrays
#endif
