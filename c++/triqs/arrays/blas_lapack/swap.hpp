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
#include <triqs/utility/exceptions.hpp>

namespace triqs::arrays::blas {
  /**
  * Blas 1: swap
  */
  template <typename VTX, typename VTY>
  typename std::enable_if<is_blas_lapack_type<typename VTX::value_type>::value && have_same_value_type<VTX, VTY>::value>::type swap(VTX &X, VTY &Y) {
    static_assert(is_amv_value_or_view_class<VTX>::value, "blas1 bindings only take vector and vector_view");
    static_assert(is_amv_value_or_view_class<VTY>::value, "blas1 bindings only take vector and vector_view");
    if ((X.size() != Y.size())) TRIQS_RUNTIME_ERROR << "Dimension mismatch in dot : X : " << X().shape() << " and Y : " << Y().shape();
    f77::swap(X.size(), X.data_start(), X.stride(), Y.data_start(), Y.stride());
  } // namespace triqs::arrays::blastemplate<typenameVTX,typenameVTY>typenamestd::enable_if<is_blas_lapack_type<typenameVTX::value_type>::value&&have_same_value_type<VTX,VTY>::value>::typeswap(VTX&X,VTY&Y)

} // namespace triqs::arrays::blas
