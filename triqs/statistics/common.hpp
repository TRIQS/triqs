/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011-2017 by O. Parcollet, T. Ayral.
 * Copyright (C) 2018 by Simons Foundation
 *   author : O. Parcollet, H. Strand, P. Dumitrescu
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
#include <triqs/utility/itertools.hpp>
#include <triqs/mpi/vector.hpp>
#include <triqs/h5/h5_serialize.hpp>
#include <type_traits>
#include <vector>
#include "./stacked_array.hpp"
#include "./stacked_gf.hpp"

namespace triqs::stat {

  template <typename T, typename Enable = void> struct _vec_t { using type = std::vector<T>; };

  template <typename A> struct _vec_t<A, std::enable_if_t<is_amv_value_or_view_class<A>::value>> {
    using type = stacked_array<typename A::value_type, A::rank>;
  };

  template <typename A> struct _vec_t<A, std::enable_if_t<gfs::is_gf<A>::value>> { using type = gfs::stacked_gf<A>; };

  template <typename T> using vec_t = typename _vec_t<T>::type;

  template <typename T> class accumulator;

} // namespace triqs::stat


