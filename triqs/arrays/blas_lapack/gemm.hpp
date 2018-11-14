/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012-2017 by O. Parcollet
 * Copyright (C) 2018 by Simons Foundation
 *   author : O. Parcollet, P. Dumitrescu, N. Wentzell
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

#pragma once

#include <complex>
#include "f77/cxx_interface.hpp"
#include "tools.hpp"
#include "qcache.hpp"

namespace triqs::arrays::blas {

  using namespace blas_lapack_tools;

  template <typename MT1, typename MT2, typename MTOut> struct use_blas_gemm {
    static_assert(is_amv_value_or_view_class<MTOut>::value, "output of matrix product must be a matrix or matrix_view");
    //static constexpr bool are_both_value_view = is_amv_value_or_view_class<MT1>::value && is_amv_value_or_view_class<MT2>::value;
    //static constexpr bool value = are_both_value_view && is_blas_lapack_type<typename MT1::value_type>::value && have_same_value_type< MT1, MT2, MTOut>::value;
    static constexpr bool value = is_blas_lapack_type<typename MT1::value_type>::value && have_same_value_type<MT1, MT2, MTOut>::value;
    // if inverse_lazy e.g. it is ok, we will use a cache anyway....
  };

  /**
  * Calls gemm on a matrix or view
  * Takes care of making temporary copies if necessary
  */
  template <typename MT1, typename MT2, typename MTOut>
  typename std::enable_if<use_blas_gemm<MT1, MT2, MTOut>::value>::type gemm(typename MT1::value_type alpha, MT1 const &A, MT2 const &B,
                                                                            typename MT1::value_type beta, MTOut &C) {
    //std::cerr  << "gemm: blas call "<< std::endl ;
    // first resize if necessary and possible
    resize_or_check_if_view(C, make_shape(first_dim(A), second_dim(B)));

    // now we use qcache instead of the matrix to make a copy if necessary ...
    // not optimal : if stride == 1, N ---> use LDA parameters
    // change the condition in the qcache construction....
    reflexive_qcache<MTOut> Cc(C);

    if (C.memory_layout_is_c()) {
      // then tC = tB tA !
      const_qcache<MT1> Cb(A); // note the inversion  A <-> B
      const_qcache<MT2> Ca(B); // note the inversion  A <-> B
      if (!(first_dim(Ca()) == second_dim(Cb())))
        TRIQS_RUNTIME_ERROR << "Dimension mismatch in gemm : A : " << get_shape(Ca()) << " while B : " << get_shape(Cb());
      char trans_a = get_trans(Ca(), true);
      char trans_b = get_trans(Cb(), true);
      int m        = (trans_a == 'N' ? get_n_rows(Ca()) : get_n_cols(Ca()));
      int n        = (trans_b == 'N' ? get_n_cols(Cb()) : get_n_rows(Cb()));
      int k        = (trans_a == 'N' ? get_n_cols(Ca()) : get_n_rows(Ca()));
      //std::cerr<< " about to call GEMM"<< std::endl ;
      //std::cerr<< "A = "<< get_shape(Ca())<< Ca()<< std::endl;
      //std::cerr<< "B = "<< get_shape(Cb())<< Cb()<< std::endl;
      //std::cerr<< "C c" << get_shape(Cc()) << Cc().indexmap().strides() << std::endl;
      //std::cerr<<Ca().memory_layout_is_c() <<Ca().memory_layout_is_fortran()<<std::endl;
      //std::cerr<< get_n_rows(Ca())<<get_n_cols(Cb())<<get_n_cols(Ca()) << std::endl ;
      f77::gemm(trans_a, trans_b, m, n, k, alpha, Ca().data_start(), get_ld(Ca()), Cb().data_start(), get_ld(Cb()), beta, Cc().data_start(),
                get_ld(Cc()));
      //std::cerr << " gemm ok "<< std::endl ;
    } else {
      const_qcache<MT1> Ca(A);
      const_qcache<MT2> Cb(B);
      if (!(second_dim(Ca()) == first_dim(Cb())))
        TRIQS_RUNTIME_ERROR << "Dimension mismatch in gemm : A : " << get_shape(Ca()) << " while B : " << get_shape(Cb());
      char trans_a = get_trans(Ca(), false);
      char trans_b = get_trans(Cb(), false);
      int m        = (trans_a == 'N' ? get_n_rows(Ca()) : get_n_cols(Ca()));
      int n        = (trans_b == 'N' ? get_n_cols(Cb()) : get_n_rows(Cb()));
      int k        = (trans_a == 'N' ? get_n_cols(Ca()) : get_n_rows(Ca()));
      f77::gemm(trans_a, trans_b, m, n, k, alpha, Ca().data_start(), get_ld(Ca()), Cb().data_start(), get_ld(Cb()), beta, Cc().data_start(),
                get_ld(Cc()));
    }
  }

  // make the generic version for non lapack types or more complex types
  // largely suboptimal
  template <typename MT1, typename MT2, typename MTOut>
  void gemm_generic(typename MT1::value_type alpha, MT1 const &A, MT2 const &B, typename MT1::value_type beta, MTOut &C) {
    //std::cerr  << "gemm: generic call "<< std::endl ;
    // first resize if necessary and possible
    resize_or_check_if_view(C, make_shape(first_dim(A), second_dim(B)));
    if (second_dim(A) != first_dim(B)) TRIQS_RUNTIME_ERROR << "gemm generic : dimension mismatch " << get_shape(A) << get_shape(B);
    C() = 0;
    for (int i = 0; i < first_dim(A); ++i)
      for (int k = 0; k < second_dim(A); ++k)
        for (int j = 0; j < second_dim(B); ++j) C(i, j) += A(i, k) * B(k, j);
  }

  // generic version for non lapack
  template <typename MT1, typename MT2, typename MTOut>
  typename std::enable_if<!use_blas_gemm<MT1, MT2, MTOut>::value>::type gemm(typename MT1::value_type alpha, MT1 const &A, MT2 const &B,
                                                                             typename MT1::value_type beta, MTOut &C) {
    gemm_generic(alpha, A, B, beta, C);
  }

  // to allow gemm (alpha, a, b, beta, M(..., ...)) i.e. a temporary view, which is not matched by previos templates
  // which require an lvalue. This is the only version which takes an && as last argument
  // indeed, in the routine, c is a *lvalue*, since it has a name, and hence we call *other* overload of the function
  template <typename A, typename MT1, typename MT2, typename B, typename V, typename To, bool W>
  void gemm(A alpha, MT1 const &a, MT2 const &b, B beta, matrix_view<V, To, W> &&c) {
    gemm(alpha, a, b, beta, c);
  }

} // namespace triqs::arrays::blas