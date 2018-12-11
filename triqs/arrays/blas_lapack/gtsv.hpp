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
#include <triqs/utility/c14.hpp>
#include "f77/cxx_interface.hpp"
#include "tools.hpp"
#include "qcache.hpp"

namespace triqs::arrays::lapack {

  using namespace blas_lapack_tools;

  // RHS as a matrix/matrix_view
  template <typename MT, typename VT>
  std::enable_if_t<is_blas_lapack_type<typename MT::value_type>::value && is_blas_lapack_type<typename VT::value_type>::value
                   && have_same_value_type<MT, VT>::value>
  gtsv(VT &DL, VT &D, VT &DU, MT &B) {
    reflexive_qcache<VT> Cdl(DL);
    reflexive_qcache<VT> Cd(D);
    reflexive_qcache<VT> Cdu(DU);
    reflexive_qcache<MT> Cb(B);

    if (Cb().memory_layout_is_c()) TRIQS_RUNTIME_ERROR << "RHS matrix passed to gtsv is not in Fortran order";

    int N = first_dim(Cd());
    if (first_dim(Cdl()) != N - 1)
      TRIQS_RUNTIME_ERROR << "gtsv : dimension mismatch between sub-diagonal and diagonal vectors, " << first_dim(Cdl()) << " vs " << N;
    if (first_dim(Cdu()) != N - 1)
      TRIQS_RUNTIME_ERROR << "gtsv : dimension mismatch between super-diagonal and diagonal vectors, " << first_dim(Cdu()) << " vs " << N;
    if (first_dim(Cb()) != N)
      TRIQS_RUNTIME_ERROR << "gtsv : dimension mismatch between diagonal vector and RHS matrix, " << N << " vs " << first_dim(Cb());

    int info;
    f77::gtsv(N, second_dim(Cb()), Cdl().data_start(), Cd().data_start(), Cdu().data_start(), Cb().data_start(), N, info);
    if (info) TRIQS_RUNTIME_ERROR << "Error in gtsv : info = " << info;
  }

  // RHS as a vector/vector_view
  template <typename VT> std::enable_if_t<is_blas_lapack_type<typename VT::value_type>::value> gtsv(VT &DL, VT &D, VT &DU, VT &B) {
    reflexive_qcache<VT> Cdl(DL);
    reflexive_qcache<VT> Cd(D);
    reflexive_qcache<VT> Cdu(DU);
    reflexive_qcache<VT> Cb(B);

    int N = first_dim(Cd());
    if (first_dim(Cdl()) != N - 1)
      TRIQS_RUNTIME_ERROR << "gtsv : dimension mismatch between sub-diagonal and diagonal vectors, " << first_dim(Cdl()) << " vs " << N;
    if (first_dim(Cdu()) != N - 1)
      TRIQS_RUNTIME_ERROR << "gtsv : dimension mismatch between super-diagonal and diagonal vectors, " << first_dim(Cdu()) << " vs " << N;
    if (first_dim(Cb()) != N)
      TRIQS_RUNTIME_ERROR << "gtsv : dimension mismatch between diagonal vector and RHS vector, " << N << " vs " << first_dim(Cb());

    int info;
    f77::gtsv(N, 1, Cdl().data_start(), Cd().data_start(), Cdu().data_start(), Cb().data_start(), N, info);
    if (info) TRIQS_RUNTIME_ERROR << "Error in gtsv : info = " << info;
  }

} // namespace triqs::arrays::lapack
