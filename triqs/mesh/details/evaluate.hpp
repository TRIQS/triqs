/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012-2018  by M. Ferrero, O. Parcollet
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

// ------------------------------------------
// FIXME : MOVE THIS IN std::array util
namespace std {
  template <typename T, size_t R> std::ostream &operator<<(std::ostream &sout, std::array<T, R> const &a) {
    sout << '(';
    for (int u = 0; u < R; ++u) sout << (u ? ", " : "") << a[u];
    return sout << ')';
  }

} // namespace std

//---------------------------------

namespace triqs::mesh {

  namespace details {
  
    // we need to form w_i w_j g[x_i, x_j],  0< i <= N1, 0 < j <= N2
    // we take a compile time sequence I in [0, ..., N1 N2]
    // and use i = I%N1, j = I/N1 to obtain all values of (i,j)
    // same in higher dimension
    // for w_i w_j w_k g[x_i, x_j, x_k],  0< i <= N1, 0 < j <= N2, 0 < k <= N3
    // we use i = I%N1, j = (I% N1N2) /N1, k = (I%N1N2N3)/N1N2 to obtain all values of (i,j, k) (for k the % is useless...)

    template <size_t... Is, typename F, typename A1>
    FORCEINLINE auto _multivar_eval_impl2(std::index_sequence<Is...>, F const & f, A1 const &a1) {
      return ((a1[Is].second * f(a1[Is].first)) + ...);
    }

    template <size_t... Is, typename F, typename A1, typename A2>
    FORCEINLINE auto _multivar_eval_impl2(std::index_sequence<Is...>, F const & f, A1 const &a1, A2 const &a2) {
      constexpr int N1 = std::tuple_size<A1>::value;
      return ((a1[Is % N1].second * a2[Is / N1].second * f(a1[Is % N1].first, a2[Is / N1].first)) + ...);
    }

    template <size_t... Is, typename F, typename A1, typename A2, typename A3>
    FORCEINLINE auto _multivar_eval_impl2(std::index_sequence<Is...>, F const & f, A1 const &a1, A2 const &a2, A3 const &a3) {
      constexpr int N1  = std::tuple_size<A1>::value;
      constexpr int N12 = N1 * std::tuple_size<A2>::value;
      return ((a1[Is % N1].second * a2[(Is % N12) / N1].second * a3[Is / N12].second * //
               f(a1[Is % N1].first, a2[(Is % N12) / N1].first, a3[Is / N12].first))
              + ...);
    }

    template <size_t... Is, typename F, typename A1, typename A2, typename A3, typename A4>
    FORCEINLINE auto _multivar_eval_impl2(std::index_sequence<Is...>, F const & f, A1 const &a1, A2 const &a2, A3 const &a3, A4 const &a4) {
      constexpr int N1   = std::tuple_size<A1>::value;
      constexpr int N12  = N1 * std::tuple_size<A2>::value;
      constexpr int N123 = N12 * std::tuple_size<A3>::value;
      return ((a1[Is % N1].second * a2[(Is % N12) / N1].second * a3[(Is % N123) / N12].second * a4[Is / N123].second * //
               f(a1[Is % N1].first, a2[(Is % N12) / N1].first, a3[(Is % N123) / N12].first, a4[Is / N123].first))
              + ...);
    }

    //
    template <typename F, typename... InterPolDataType> FORCEINLINE auto multivar_eval(F const & f, InterPolDataType const &... a) {
      return details::_multivar_eval_impl2(std::make_index_sequence<(std::tuple_size<InterPolDataType>::value * ...)>{}, f, a...);
    }

    // FIXME20 : use a lambda
    template <typename F, size_t... Is, typename... Args> auto evaluate_impl(std::index_sequence<Is...>, F const &f, Args &&... args) {
      return multivar_eval(f, std::get<Is>(f.mesh().components()).get_interpolation_data(std::forward<Args>(args))...);
    }
  } // namespace details

  // ------------------- evaluate --------------------------------
 
  /** 
   * Make the evaluation of a function by linear interpolation on the mesh
   * @tparam M The mesh type
   * @param m The mesh
   * @param f The function 
   */
  template <typename Mesh, typename F, typename... Args> auto evaluate(Mesh const &m, F const &f, Args &&... args) {

    if constexpr (not is_product_v<Mesh>) {

      auto id = m.get_interpolation_data(std::forward<Args...>(args...));
      if constexpr (id.size() == 1) {
        return id[0].second * f[id[0].first];
      } else if constexpr (id.size() == 2) {
        return id[0].second * f[id[0].first] + id[1].second * f[id[1].first];
      } else {
        return details::multivar_eval(f, id); // FIXME : should be the only case ...
      }

    } else { // special case for the product mesh
      return details::evaluate_impl(std::index_sequence_for<Args...>{}, f, std::forward<Args>(args)...);
    }
  }

} // namespace triqs::mesh

