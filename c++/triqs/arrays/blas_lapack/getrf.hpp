// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018 Simons Foundation
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You may obtain a copy of the License at
//     https://www.gnu.org/licenses/gpl-3.0.txt
//
// Authors: Philipp D, Olivier Parcollet, Nils Wentzell

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
