/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012 by M. Ferrero, O. Parcollet
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

#include <itertools/itertools.hpp>
#include <nda/nda.hpp>
#include <triqs/utility/tuple_tools.hpp>

#include <fmt/core.h>

#include "./concepts.hpp"

namespace triqs::mesh {

  using dcomplex = std::complex<double>;

  using nda::array;
  using nda::array_view;
  using nda::ellipsis;
  using nda::eye;
  using nda::matrix;
  using nda::matrix_const_view;
  using nda::matrix_view;
  using nda::range;

  //-------------------------------------------------------------------------

  template <typename... T> uint64_t hash(T &&...ts) { return (std::hash<std::decay_t<T>>()(ts) + ...); }

  //------------------------------------------------------------------------

  inline long positive_modulo(long r, long d) {
    long res = r % d;
    return (res >= 0 ? res : res + d);
  }

  //------------------------------------------------------------------------

  template <Mesh M>  static constexpr bool is_product = false;
  template <Mesh Ms> static constexpr int n_variables = 1;

  /// A place holder for : or *all*
  using all_t = nda::range::all_t;

  /** The statistics : Boson or Fermion */
  enum statistic_enum { Boson = 0, Fermion = 1 };

  // 1 for Boson, -1 for Fermion
  inline int sign(statistic_enum s) { return (s == Boson ? 1 : -1); }

  /// Boson*Fermion -> Fermion, others -> Boson
  inline statistic_enum operator*(statistic_enum i, statistic_enum j) { return (i == j ? Boson : Fermion); }

  //------------------------------ closest_mesh_pt -------------------------

  template <typename T> struct closest_mesh_point_t {
    T value;
  };

  // Returns a lazy structure containing x that will be used by the [] operator of gf e.g.
  template <typename... T> auto closest_mesh_pt(T &&...x) {
    if constexpr (sizeof...(T) == 1)
      return closest_mesh_point_t<std::decay_t<T>...>{std::forward<T>(x)...};
    else
      return std::tuple{closest_mesh_point_t<std::decay_t<T>>{std::forward<T>(x)}...};
  }

  // 1 in a completely neutral type for the compiler, which can not optimize 1.0 * x a priori.
  struct one_t {};
  template <typename T> auto operator*(one_t, T &&x) { return std::forward<T>(x); }
  template <typename T> auto operator*(T &&x, one_t) { return std::forward<T>(x); }
  inline one_t operator*(one_t, one_t) { return {}; } // avoid ambiguity

  template <typename M> // do NOT constrain here .. 
  auto make_mesh_range_from_subscript(M const * m, long l) {
    return itertools::transform(itertools::range(l), [m](long i){ return (*m)[i];});
	// return std::ranges::views::iota(0l, l) | std::ranges::views::transform([&m](long i){ return m[i];});
  }

} // namespace triqs::mesh
