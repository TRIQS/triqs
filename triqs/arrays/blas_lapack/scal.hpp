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
