/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2013 by O. Parcollet
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
#ifndef TRIQS_ARRAYS_MATRIX_STACK_VIEW_H
#define TRIQS_ARRAYS_MATRIX_STACK_VIEW_H
#include "./array.hpp"
#include "./matrix.hpp"
#include "./matrix_tensor_proxy.hpp"
#include <triqs/arrays/linalg/det_and_inverse.hpp>

namespace triqs { namespace arrays {

 template<typename T> class matrix_stack_view { 
   array_view<T,3>  a;
  public:
   typedef array_view<T,3> array_view_t;

   matrix_stack_view( typename array_view_t::view_type const & a_):a(a_) {}
   matrix_stack_view( array_view_t && a_):a(std::move(a_)) {}

#ifdef TRIQS_WITH_PYTHON_SUPPORT
   /// Build from a numpy.array (X is a borrowed reference) : throws if X is not a numpy.array
   explicit matrix_stack_view (PyObject * X):a(typename array_view_t::view_type (X)){}
#endif

   //matrix_view_proxy      <array_view_t,0> operator()(size_t i)       { return matrix_view_proxy       <array_view_t,0>(a,i);}
   //const_matrix_view_proxy<array_view_t,0> operator()(size_t i) const { return const_matrix_view_proxy <array_view_t,0>(a,i);}
   auto operator()(long i) DECL_AND_RETURN(make_matrix_proxy(this->a, i));
   auto operator()(long i) const DECL_AND_RETURN(make_const_matrix_proxy(this->a, i));

   matrix_view<T> view(size_t i) const { return a(i,range(),range());}
   
   size_t size() const { return a.shape(0);}
   // BE CAREFUL to the shift : it is 1, 2, not 0,1, because of the stack !
   friend size_t first_dim  (matrix_stack_view const & m) { return second_dim(m.a);}
   friend size_t second_dim (matrix_stack_view const & m) { return third_dim(m.a);}

   matrix_stack_view & operator +=(matrix_stack_view const & arg) { a += arg.a; return *this; }
   matrix_stack_view & operator -=(matrix_stack_view const & arg) { a -= arg.a; return *this; }

   template<typename RHS>   
    typename std::enable_if<RHS::rank ==2, matrix_stack_view &>::type 
    operator +=(RHS const & arg) { for (size_t i=0; i<size(); ++i) view(i) +=arg; return *this; }

   template<typename RHS>   
    typename std::enable_if<RHS::rank ==2, matrix_stack_view &>::type 
    operator -=(RHS const & arg) { for (size_t i=0; i<size(); ++i) view(i) -=arg; return *this; }
  
   template<typename RHS> matrix_stack_view & operator *=(RHS const & arg) { a*= arg; return *this; }
   template<typename RHS> matrix_stack_view & operator /=(RHS const & arg) { a/= arg; return *this; }

   void invert() {for (size_t i=0; i<size(); ++i) { auto v = view(i); v = inverse(v);} }

   friend matrix_stack_view matmul_L_R ( matrix_view<T> const & L, matrix_stack_view const & M, matrix_view<T> const & R) {
    matrix_stack_view res (typename array_view_t::regular_type (M.size(), first_dim(L), second_dim(R)));
    for (size_t i=0; i<M.size(); ++i)  { res.view(i) = L * M.view(i) * R; }
    return res;
   }

   void onsite_matmul_L_R ( matrix_view<T> const & L, matrix_stack_view const & M, matrix_view<T> const & R) {
    if ((first_dim(*this) != first_dim(L)) || (second_dim(*this) != second_dim(R)) || (second_dim(L) != first_dim(R)))
      TRIQS_RUNTIME_ERROR << "dimensions do not match!";
    for (size_t i=0; i<M.size(); ++i)  { view(i) = L * M.view(i) * R; }
   }
 };

}}
#endif

