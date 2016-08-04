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
#ifndef TRIQS_ARRAYS_LINALG_DET_INV_H
#define TRIQS_ARRAYS_LINALG_DET_INV_H
#include "../impl/common.hpp"
#include "../matrix.hpp"
#include "../blas_lapack/getrf.hpp"
#include "../blas_lapack/getri.hpp"

namespace triqs {
namespace arrays {

 /// Error which occurs during the matrix inversion
 class matrix_inverse_exception : public triqs::runtime_error {};

 /**
  * Lazy result of inverse(M) where M can be :
  *  * a matrix, a matrix_view
  *  * any matrix expression
  * The object is lazy, it does not do any computation.
  * It can be copied at no cost
  * It keeps view of the object A if it a matrix, a copy if it is a formal expression.
  */
 template <typename A> struct inverse_lazy;

 template <class A>
 std14::enable_if_t<ImmutableMatrix<std14::remove_reference_t<A>>::value,
                    inverse_lazy<typename utility::remove_rvalue_ref<A>::type>>
 inverse(A &&a) {
  return {std::forward<A>(a)};
 }

 //template <class A>
 //std14::enable_if_t<ImmutableArray<std14::remove_reference_t<A>>::value> inverse(A &&a) = delete; // arrays can not be inverted.

 // ----------------- implementation -----------------------------------------

 // worker takes a contiguous view and compute the det and inverse in two steps.
 // it is separated in case of multiple use (no reallocation of ipvi, etc...)
 // A can be a matrix, a matrix_view
 template <typename A> class det_and_inverse_worker {
  typedef typename A::value_type value_type;
  typedef matrix_view<value_type> V_type;
  A a;
  int dim;
  triqs::arrays::vector<int> ipiv;
  int step, info;
  value_type _det;

  public:
  det_and_inverse_worker(A a_) : a(std::move(a_)), dim(first_dim(a)), ipiv(dim), step(0) {
   if (first_dim(a) != second_dim(a))
    TRIQS_RUNTIME_ERROR << "Inverse/Det error:non-square matrix. Dimensions are :(" << first_dim(a) << "," << second_dim(a)
                        << ")\n  ";
   if (!(has_contiguous_data(a))) TRIQS_RUNTIME_ERROR << "det_and_inverse_worker only takes a contiguous view";
  }

  value_type det() {
   V_type W = fortran_view(a);
   _step1(W);
   _compute_det(W);
   return _det;
  }

  A const &inverse() {
   if (step < 2) {
    V_type W = fortran_view(a);
    _step1(W);
    _step2(W);
   }
   return a;
  }

  private:

  // no need of special traversal
  template <typename MT> V_type fortran_view(MT const &x) {
   if (x.indexmap().memory_layout_is_c())
    return x.transpose();
   else
    return x;
  }

  void _step1(V_type &W) {
   if (step > 0) return;
   step = 1;
   info = lapack::getrf(W, ipiv);
   if (info < 0) throw matrix_inverse_exception() << "Inverse/Det error : failure of getrf lapack routine ";
  }

  void _compute_det(V_type const &W) {
   if (step > 1) return;
   _det = 1;
   for (size_t i = 0; i < dim; i++) _det *= W(i, i);
   bool flip = false; // compute the sign of the permutation
   for (size_t i = 0; i < dim; i++) {
    if (ipiv(i) != int(i) + 1) flip = !(flip);
   }
   _det = (flip ? -_det : _det);
  }

  void _step2(V_type &W) {
   assert(step == 1); // if (step==1) return;
   step = 2;
   _compute_det(W);
   info = lapack::getri(W, ipiv);
   if (info != 0) throw matrix_inverse_exception() << "Inverse/Det error : matrix is not invertible";
  }
 };

 //-----------------------------------------------------------

 template <typename A> struct inverse_lazy : TRIQS_CONCEPT_TAG_NAME(ImmutableMatrix) {
  typedef typename std::remove_reference<A>::type A_t;
  typedef typename std::remove_const<typename A_t::value_type>::type value_type;
  typedef typename A_t::domain_type domain_type;
  typedef matrix<value_type> M_type;
  typedef matrix_view<value_type> M_view_type;

  template <typename AA> inverse_lazy(AA &&a_) : a(std::forward<AA>(a_)), M{}, computed{false} {
   if (first_dim(a) != second_dim(a))
    TRIQS_RUNTIME_ERROR << "Inverse : matrix is not square but of size " << first_dim(a) << " x " << second_dim(a);
  }

  domain_type domain() const { return a.domain(); }
  A const & input() const { return a;}

  template <typename K0, typename K1> value_type const &operator()(K0 const &k0, K1 const &k1) const {
   activate();
   return M(k0, k1);
  }

  M_type const &operator()() const {
   activate();
   return M;
  }

  friend std::ostream &operator<<(std::ostream &out, inverse_lazy const &x) { return out << "inverse(" << x.a << ")"; }

  private:
  A a;
  mutable M_type M;
  mutable bool computed;

  void activate() const {
   if (computed) return;
   M = a;
   auto worker  = det_and_inverse_worker<M_view_type> {M};
   worker.inverse();
   computed = true; 
  }
 };

 // Optimized implementation of =
 // if M = inverse(M) with the SAME object, then we do not need to copy the data
 template <typename A, typename MT>
  ENABLE_IF(is_matrix_or_view<typename std::remove_reference<A>::type>)
  triqs_arrays_assign_delegation(MT &lhs, inverse_lazy<A> const &rhs) {
   static_assert(is_matrix_or_view<MT>::value, "Can only assign an inverse matrix to a matrix or a matrix_view");
   bool M_eq_inverse_M = ((lhs.indexmap().get_memory_layout() == rhs.input().indexmap().get_memory_layout()) &&
     (lhs.data_start() == rhs.input().data_start()) && (has_contiguous_data(lhs)));
   if (!M_eq_inverse_M) {
    lhs = rhs();
   } else {
    blas_lapack_tools::reflexive_qcache<MT> C(lhs);
    // a reflexive cache will use a temporary "regrouping" copy if and only if needed
    det_and_inverse_worker<typename MT::view_type> W(C()); // the worker to make the inversion of the lhs...
    W.inverse();                                           // worker is working ...
   }
  }

 //------------------- det   ----------------------------------------

 template <typename A> typename std::remove_reference<A>::type::value_type determinant(A &&a) {
  // makes a temporary copy of A if A is a const &
  // If a is a matrix &&, it is moved into the worker.
  auto worker = det_and_inverse_worker<matrix<typename std::remove_reference<A>::type::value_type>>(std::forward<A>(a));
  return worker.det();
 }
}

namespace clef {
 TRIQS_CLEF_MAKE_FNT_LAZY(determinant);
}

} // namespace triqs::arrays
#endif
