/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012  by M. Ferrero, O. Parcollet
 * Copyright (C) 2018-2019  by Simons Foundation
 *               author : O. Parcollet
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
#include <array>
#include "./mesh_tools.hpp"
#include "./multivar_eval.hpp"

// FIXME : MOVE THIS IN std::array util
namespace std {
  template <typename T, size_t R> std::ostream &operator<<(std::ostream &sout, std::array<T, R> const &a) {
    sout << '(';
    for (int u = 0; u < R; ++u) sout << (u ? ", " : "") << a[u];
    return sout << ')';
  }

} // namespace std

// Techniques for linear interpolation

namespace triqs::mesh {

   // ------------------- evaluate --------------------------------

  namespace details {
    // FIXME20 : use a lambda
    template <typename F, size_t... Is, typename... Args> auto evaluate_impl(std::index_sequence<Is...>, F const &f, Args &&... args) {
      return multivar_eval(f, std::get<Is>(f.mesh().components()).get_interpolation_data(std::forward<Args>(args))...);
    }
  } // namespace details

  // DOC ? Internal only ?
  template <typename Mesh, typename F, typename... Args> auto evaluate(Mesh const &m, F const &f, Args &&... args) {
    if constexpr (not is_product_v<Mesh>) {
      auto id = m.get_interpolation_data(std::forward<Args...>(args...));
      if constexpr (id.n_pts == 1) {
        return id.w[0] * f[id.idx[0]];
      } else if constexpr (id.n_pts == 2) {
        return id.w[0] * f[id.idx[0]] + id.w[1] * f[id.idx[1]];
      } else {
        return multivar_eval(f, id); // FIXME : should be the only case ...
      }

    } else { // special case for the product mesh
      return details::evaluate_impl(std::index_sequence_for<Args...>{}, f, std::forward<Args>(args)...);
    }
  }

} // namespace triqs::mesh
