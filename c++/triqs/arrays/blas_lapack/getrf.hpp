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

namespace triqs::arrays::lapack {

  using namespace blas_lapack_tools;

  /**
  * Calls getrf on a matrix or view
  * Takes care of making temporary copies if necessary
  */
  template <typename MT>
  typename std::enable_if<is_blas_lapack_type<typename MT::value_type>::value, int>::type getrf(MT &A, arrays::vector<int> &ipiv,
                                                                                                bool assert_fortran_order = false) {
    if (assert_fortran_order && A.memory_layout_is_c()) TRIQS_RUNTIME_ERROR << "matrix passed to getrf is not in Fortran order";
    reflexive_qcache<MT> Ca(A);
    auto dm = std::min(first_dim(Ca()), second_dim(Ca()));
    if (ipiv.size() < dm) ipiv.resize(dm);
    int info;
    f77::getrf(get_n_rows(Ca()), get_n_cols(Ca()), Ca().data_start(), get_ld(Ca()), ipiv.data_start(), info);
    return info;
  }

} // namespace triqs::arrays::lapack
