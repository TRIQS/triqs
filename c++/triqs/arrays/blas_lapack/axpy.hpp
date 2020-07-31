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

#include <triqs/arrays/impl/traits.hpp>
#include "f77/cxx_interface.hpp"
#include "tools.hpp"

namespace triqs::arrays::blas {
  /**
  * Blas 1: axpy
  */
  template <typename VTX, typename VTY>
  std::enable_if_t<is_blas_lapack_type<typename VTX::value_type>::value && have_same_value_type<VTX, VTY>::value>
  axpy(typename VTX::value_type const &alpha, VTX const &X, VTY &Y) {
    static_assert(is_amv_value_or_view_class<VTX>::value, "blas1 bindings only take vector and vector_view");
    static_assert(is_amv_value_or_view_class<VTY>::value, "blas1 bindings only take vector and vector_view");
    //if (( X.size() != Y.size()) ) Y.resize(X.size());
    //const_qcache<VTX> Cx(X); // mettre la condition a la main
    //f77::copy(X.size(), Cx().data_start(), Cx().stride(), Y.data_start(), Y.stride());
    f77::axpy(X.size(), alpha, X.data_start(), X.stride(), Y.data_start(), Y.stride());
  }

  /**
  * Non blas equivalent
  */
  template <typename VTX, typename VTY>
  std::enable_if_t<!(is_blas_lapack_type<typename VTX::value_type>::value && have_same_value_type<VTX, VTY>::value)>
  axpy(typename VTX::value_type const &alpha, VTX const &X, VTY &Y) {
    for (size_t i = 0; i < X.size(); ++i) Y(i) = Y(i) + alpha * X(i);
  }

} // namespace triqs::arrays::blas
