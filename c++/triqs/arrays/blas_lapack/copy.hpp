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

#include "f77/cxx_interface.hpp"
#include "tools.hpp"
#include <triqs/arrays/impl/traits.hpp>
#include <triqs/utility/exceptions.hpp>

namespace triqs::arrays::blas {

  template <typename VTX, typename VTY> struct use_blas_copy {
    // I do not use cache here...
    static constexpr bool are_both_value_view = is_amv_value_or_view_class<VTX>::value && is_amv_value_or_view_class<VTY>::value;
    static constexpr bool value =
       are_both_value_view && is_blas_lapack_type<typename VTX::value_type>::value && have_same_value_type<VTX, VTY>::value;
  };

  /**
  * Blas 1: copy
  */
  template <typename VTX, typename VTY> typename std::enable_if<use_blas_copy<VTX, VTY>::value>::type copy(VTX const &X, VTY &Y) {
    static_assert(is_amv_value_or_view_class<VTX>::value, "blas1 bindings only take vector and vector_view");
    static_assert(is_amv_value_or_view_class<VTY>::value, "blas1 bindings only take vector and vector_view");
    //if (( X.size() != Y.size()) ) Y.resize(X.size());
    //const_qcache<VTX> Cx(X); // mettre la condition a la main
    f77::copy(X.size(), X.data_start(), X.stride(), Y.data_start(), Y.stride());
    //f77::copy(X.size(), Cx().data_start(), Cx().stride(), Y.data_start(), Y.stride());
  }

  // generic version
  template <typename VTX, typename VTY> typename std::enable_if<!use_blas_copy<VTX, VTY>::value>::type copy(VTX const &X, VTY &Y) {
    if (X.size() != Y.size()) TRIQS_RUNTIME_ERROR << "blas copy generic : dimension mismatch " << X.size() << Y.size();
    const size_t N = X.size();
    for (size_t i = 0; i < N; ++i) Y(i) = X(i);
  }

} // namespace triqs::arrays::blas
