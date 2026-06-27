// Copyright (c) 2016-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2016-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2023 Simons Foundation
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
// Authors: Michel Ferrero, Olivier Parcollet, Nils Wentzell

/**
 * @file
 * @brief Provides the target types that fix the value stored at each mesh point of a Green's function.
 */

#pragma once

#include "../../arrays.hpp"

#include <nda/stdutil/complex.hpp>

#include <type_traits>

namespace triqs::gfs {

  /**
   * @addtogroup triqs-gfs-targets
   * @{
   */

  // Forward declarations.
  template <int R> struct tensor_valued;
  struct matrix_valued;
  struct scalar_valued;

  /**
   * @brief Target type for a real tensor-valued Green's function.
   *
   * @details Real-valued counterpart of triqs::gfs::tensor_valued, i.e. the value stored at each mesh point is an nda 
   * array of rank \f$ R \f$ with `double` value type.
   *
   * @tparam R Rank of the tensor (must be strictly positive).
   */
  template <int R> struct tensor_real_valued {
    static_assert(R > 0, "tensor_real_valued : R must be > 0");

    /// Rank of the target, i.e. the number of indices of the value stored at each mesh point.
    static constexpr int rank = R;

    /// Whether the target is real valued.
    static constexpr int is_real = true;

    /// Whether the target is matrix valued.
    static constexpr bool is_matrix = false;

    /// Type of the scalar value, i.e. the value type of the stored nda array.
    using scalar_t = double;

    /// Associated real-valued target type.
    using real_t = tensor_real_valued;

    /// Associated complex-valued target type.
    using complex_t = tensor_valued<R>;
  };

  /**
   * @brief Target type for a complex tensor-valued Green's function.
   *
   * @details The value stored at each mesh point is a complex tensor of rank \f$ R \f$, i.e. an nda array of rank \f$ 
   * R \f$ with `std::complex<double>` value type.
   *
   * @tparam R Rank of the tensor (must be strictly positive).
   */
  template <int R> struct tensor_valued {
    static_assert(R > 0, "tensor_valued : R must be > 0");

    /// Rank of the target, i.e. the number of indices of the value stored at each mesh point.
    static constexpr int rank = R;

    /// Whether the target is real valued.
    static constexpr int is_real = false;

    /// Whether the target is matrix valued.
    static constexpr bool is_matrix = false;

    /// Type of the scalar value, i.e. the value type of the stored nda array.
    using scalar_t = dcomplex;

    /// Associated real-valued target type.
    using real_t = tensor_real_valued<R>;

    /// Associated complex-valued target type.
    using complex_t = tensor_valued;
  };

  /**
   * @brief Target type for a real matrix-valued Green's function.
   *
   * @details Real-valued counterpart of triqs::gfs::matrix_valued, i.e. the value stored at each mesh point is a real
   * \f$ n \times m \f$ matrix.
   */
  struct matrix_real_valued {
    /// Rank of the target, i.e. the number of indices of the value stored at each mesh point.
    static constexpr int rank = 2;

    /// Whether the target is real valued.
    static constexpr int is_real = true;

    /// Whether the target is matrix valued.
    static constexpr bool is_matrix = true;

    /// Type of the scalar value, i.e. the value type of the stored nda array.
    using scalar_t = double;

    /// Associated real-valued target type.
    using real_t = matrix_real_valued;

    /// Associated complex-valued target type.
    using complex_t = matrix_valued;
  };

  /**
   * @brief Target type for a complex matrix-valued Green's function.
   *
   * @details The value stored at each mesh point is a complex \f$ n \times m \f$ matrix, i.e. an nda array of rank \f$ 
   * 2 \f$ with `std::complex<double>` value type.
   */
  struct matrix_valued {
    /// Rank of the target, i.e. the number of indices of the value stored at each mesh point.
    static constexpr int rank = 2;

    /// Whether the target is real valued.
    static constexpr int is_real = false;

    /// Whether the target is matrix valued.
    static constexpr bool is_matrix = true;

    /// Type of the scalar value, i.e. the value type of the stored nda array.
    using scalar_t = dcomplex;

    /// Associated real-valued target type.
    using real_t = matrix_real_valued;

    /// Associated complex-valued target type.
    using complex_t = matrix_valued;
  };

  /**
   * @brief Target type for a real scalar-valued Green's function.
   *
   * @details Real-valued counterpart of triqs::gfs::scalar_valued, i.e. the value stored at each mesh point is a single
   * `double` scalar.
   */
  struct scalar_real_valued {
    /// Rank of the target, i.e. the number of indices of the value stored at each mesh point.
    static constexpr int rank = 0;

    /// Whether the target is real valued.
    static constexpr int is_real = true;

    /// Whether the target is matrix valued.
    static constexpr bool is_matrix = false;

    /// Type of the scalar value, i.e. the value type of the stored nda array.
    using scalar_t = double;

    /// Associated real-valued target type.
    using real_t = scalar_real_valued;

    /// Associated complex-valued target type.
    using complex_t = scalar_valued;
  };

  /**
   * @brief Target type for a complex scalar-valued Green's function.
   *
   * @details The value stored at each mesh point is a single `std::complex<double>` scalar.
   */
  struct scalar_valued {
    /// Rank of the target, i.e. the number of indices of the value stored at each mesh point.
    static constexpr int rank = 0;

    /// Whether the target is real valued.
    static constexpr int is_real = false;

    /// Whether the target is matrix valued.
    static constexpr bool is_matrix = false;

    /// Type of the scalar value, i.e. the value type of the stored nda array.
    using scalar_t = dcomplex;

    /// Associated real-valued target type.
    using real_t = scalar_real_valued;

    /// Associated complex-valued target type.
    using complex_t = scalar_valued;
  };

  /**
   * @brief Trait to invert the relation (scalar type, rank) \f$ \to \f$ target type.
   *
   * @details The nested `type` alias gives the target type whose scalar type is `T` and whose rank is \f$ R \f$.
   *
   * @tparam T Scalar type (`double` or `std::complex<double>`).
   * @tparam R Rank of the target.
   */
  template <typename T, int R> struct _target_from_type_rank;

  // Specialization mapping a complex scalar type and rank \f$ R \f$ to triqs::gfs::tensor_valued.
  template <int R> struct _target_from_type_rank<dcomplex, R> {
    using type = tensor_valued<R>;
  };

  // Specialization mapping a complex scalar type and rank 2 to triqs::gfs::matrix_valued.
  template <> struct _target_from_type_rank<dcomplex, 2> {
    using type = matrix_valued;
  };

  // Specialization mapping a complex scalar type and rank 0 to triqs::gfs::scalar_valued.
  template <> struct _target_from_type_rank<dcomplex, 0> {
    using type = scalar_valued;
  };

  // Specialization mapping a real scalar type and rank \f$ R \f$ to triqs::gfs::tensor_real_valued.
  template <int R> struct _target_from_type_rank<double, R> {
    using type = tensor_real_valued<R>;
  };

  // Specialization mapping a real scalar type and rank 2 to triqs::gfs::matrix_real_valued.
  template <> struct _target_from_type_rank<double, 2> {
    using type = matrix_real_valued;
  };

  // Specialization mapping a real scalar type and rank 0 to triqs::gfs::scalar_real_valued.
  template <> struct _target_from_type_rank<double, 0> {
    using type = scalar_real_valued;
  };

  /**
   * @brief Deduce the target type of a Green's function from its data array type and arity.
   *
   * @details Given a data array `A` of rank \f$ R_A \f$ and a Green's function with \f$ N \f$ variables (the arity),
   * the target rank is \f$ R_A - N \f$ and the scalar type is the value type of the array. The resulting target type
   * is obtained from triqs::gfs::_target_from_type_rank.
   *
   * @tparam A Data array type.
   * @tparam nvar Arity of the Green's function, i.e. the number of mesh dimensions.
   */
  template <typename A, int nvar>
  using target_from_array =
     typename _target_from_type_rank<typename std::decay_t<typename std::decay_t<A>::value_type>, std::decay_t<A>::rank - nvar>::type;

  /** @} */

} // namespace triqs::gfs
