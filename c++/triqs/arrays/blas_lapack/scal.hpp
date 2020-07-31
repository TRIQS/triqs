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
// Authors: Philipp D, Olivier Parcollet

#pragma once

#include <complex>
#include "f77/cxx_interface.hpp"
#include "tools.hpp"
#include <triqs/arrays/impl/traits.hpp>

namespace triqs::arrays::blas {

  /**
  * Blas1 : scal
  */
  template <typename VT>
  std::enable_if_t<is_blas_lapack_type<typename VT::value_type>::value && have_same_value_type<VT>::value> scal(typename VT::value_type const &alpha,
                                                                                                                VT &X) {
    static_assert(is_amv_value_or_view_class<VT>::value, "blas1 bindings only take vector and vector_view");
    f77::scal(X.size(), alpha, X.data_start(), X.stride());
  }

  /**
  * Non blas equivalent
  */
  template <typename VT>
  std::enable_if_t<!(is_blas_lapack_type<typename VT::value_type>::value && have_same_value_type<VT>::value)>
  scal(typename VT::value_type const &alpha, VT &X) {
    static_assert(is_amv_value_or_view_class<VT>::value, "blas1 bindings only take vector and vector_view");
    for (size_t i = 0; i < X.size(); ++i) X(i) = X(i) * alpha;
  }

} // namespace triqs::arrays::blas
