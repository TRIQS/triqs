/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2016 by O. Parcollet
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
#include <triqs/arrays.hpp>
#include "triqs/utility/complex_ops.hpp"
#include <utility>

namespace triqs {
namespace gfs {

  template <int R> struct tensor_real_valued {
  static_assert(R > 0, "tensor_real_valued : R must be > 0");
  static constexpr int rank = R;
  static constexpr bool is_matrix = false;
  using scalar_t = double;
  using slice_t = arrays::array<scalar_t, rank>;
  // using value_type = std14::conditional_t<(R!=0), arrays::array<dcomplex, R>, double > ;
 };

 template <int R> struct tensor_valued {
  static_assert(R > 0, "tensor_valued : R must be > 0");
  static constexpr int rank = R;
  static constexpr bool is_matrix = false;
  // using value_type = std14::conditional_t<(R!=0), arrays::array<dcomplex, R>, dcomplex > ;
  using scalar_t = dcomplex;
  using slice_t = arrays::array<scalar_t, rank>;
  using real_t = tensor_real_valued<R>;
 };

 struct matrix_real_valued {
  static constexpr int rank = 2;
  static constexpr bool is_matrix = true;
  using scalar_t = double;
  using slice_t = arrays::matrix<scalar_t>;
  // using value_type = arrays::<double>;
 };

 struct matrix_valued {
  static constexpr int rank = 2;
  using scalar_t = dcomplex;
  using slice_t = arrays::matrix<scalar_t>;
  // using value_type = arrays::matrix<dcomplex>;
  static constexpr bool is_matrix = true;
  using real_t = matrix_real_valued;
 };

 struct scalar_real_valued {
  static constexpr int rank = 0;
  static constexpr bool is_matrix = false;
  using scalar_t = double;
  using slice_t = scalar_t;
 };

 struct scalar_valued {
  static constexpr int rank = 0;
  using scalar_t = dcomplex;
  using slice_t = scalar_t;
  // using value_type = dcomplex;
  static constexpr bool is_matrix = false;
  using real_t = scalar_real_valued;
 };

 template <typename T> struct tail_valued {
  static constexpr int rank = T::rank + 1;
  static constexpr bool is_matrix = false;
  using scalar_t = dcomplex;
  // using slice_t = scalar_t;
  // using value_type = typename T::value_type;
 };

 template <typename T> constexpr int is_tail_valued(T) { return 0; }
 template <typename T> constexpr int is_tail_valued(tail_valued<T>) { return 1; }

 /// invert the relation:  type, rank -> target.
 template <typename T, int R> struct _target_from_type_rank;
 template <int R> struct _target_from_type_rank<dcomplex, R> { using type = tensor_valued<R>; };
 template <> struct _target_from_type_rank<dcomplex, 2> { using type = matrix_valued; };
 template <> struct _target_from_type_rank<dcomplex, 0> { using type = scalar_valued; };
 template <int R> struct _target_from_type_rank<double, R> { using type = tensor_real_valued<R>; };
 template <> struct _target_from_type_rank<double, 2> { using type = matrix_real_valued; };
 template <> struct _target_from_type_rank<double, 0> { using type = scalar_real_valued; };

 // Given an array A, compute the target, with A::rank - nvar
 template <typename A, int nvar>
 using target_from_array =
     typename _target_from_type_rank<typename std14::decay_t<A>::value_type, std14::decay_t<A>::rank - nvar>::type;
}
}
