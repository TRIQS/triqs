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
//#include <triqs/utility/std_array.hpp>

namespace triqs::gfs {

  namespace details {

    using nda::range;

    //------------------------------------
    //  multivar_eval

    // we need to form w_i w_j g[x_i, x_j],  0< i <= N1, 0 < j <= N2
    // we take a compile time sequence I in [0, ..., N1 N2]
    // and use i = I%N1, j = I/N1 to obtain all values of (i,j)
    // same in higher dimension
    // for w_i w_j w_k g[x_i, x_j, x_k],  0< i <= N1, 0 < j <= N2, 0 < k <= N3
    // we use i = I%N1, j = (I% N1N2) /N1, k = (I%N1N2N3)/N1N2 to obtain all values of (i,j, k) (for k the % is useless...)

    template <size_t... Is, typename G, typename A1> FORCEINLINE auto _multivar_eval_impl(std::index_sequence<Is...>, G const &g, A1 const &a1) {
      return ((a1[Is].second * slice_or_access(g, a1[Is].first)) + ...);
    }

    template <size_t... Is, typename G, typename A1, typename A2>
    FORCEINLINE auto _multivar_eval_impl(std::index_sequence<Is...>, G const &g, A1 const &a1, A2 const &a2) {
      constexpr int N1 = std::tuple_size<A1>::value;
      return ((a1[Is % N1].second * a2[Is / N1].second * slice_or_access(g, a1[Is % N1].first, a2[Is / N1].first)) + ...);
    }

    template <size_t... Is, typename G, typename A1, typename A2, typename A3>
    FORCEINLINE auto _multivar_eval_impl(std::index_sequence<Is...>, G const &g, A1 const &a1, A2 const &a2, A3 const &a3) {
      constexpr int N1  = std::tuple_size<A1>::value;
      constexpr int N12 = N1 * std::tuple_size<A2>::value;
      return ((a1[Is % N1].second * a2[(Is % N12) / N1].second * a3[Is / N12].second * //
               slice_or_access(g, a1[Is % N1].first, a2[(Is % N12) / N1].first, a3[Is / N12].first))
              + ...);
    }

    template <size_t... Is, typename G, typename A1, typename A2, typename A3, typename A4>
    FORCEINLINE auto _multivar_eval_impl(std::index_sequence<Is...>, G const &g, A1 const &a1, A2 const &a2, A3 const &a3, A4 const &a4) {
      constexpr int N1   = std::tuple_size<A1>::value;
      constexpr int N12  = N1 * std::tuple_size<A2>::value;
      constexpr int N123 = N12 * std::tuple_size<A3>::value;
      return ((a1[Is % N1].second * a2[(Is % N12) / N1].second * a3[(Is % N123) / N12].second * a4[Is / N123].second * //
               slice_or_access(g, a1[Is % N1].first, a2[(Is % N12) / N1].first, a3[(Is % N123) / N12].first, a4[Is / N123].first))
              + ...);
    }

    // multivar_eval
    // g : a gf, gf_view, ...
    // InterPolDataType : return types of get_interpolation_data : std::array<std::pair<long | all_t, double | one_t >> (  [ [ wi, xi] ] ) 
    template <typename G, typename... InterPolDataType> FORCEINLINE auto multivar_eval(G const &g, InterPolDataType const &... a) {
      return details::_multivar_eval_impl(std::make_index_sequence<(std::tuple_size<InterPolDataType>::value * ...)>{}, g, a...);
    }

    //------------------------------------------------------------------------------------

    template <typename M, typename Arg> auto _filter_the_all(M const &m, Arg const &arg) { return get_interpolation_data(m, arg); }
    template <typename M> std::array<std::pair<range::all_t, mesh::one_t>, 1> _filter_the_all(M const &m, mesh::all_t) { return {}; }

    // FIXME20 : use a lambda
    template <typename G, size_t... Is, typename... Args> auto evaluate_impl(std::index_sequence<Is...>, G const &g, Args const &... args) {
      return multivar_eval(g, _filter_the_all(std::get<Is>(g.mesh().components()), args)...);
    }
  } // namespace details

  // ------------------- evaluate --------------------------------

  /** 
   * Make the evaluation of a function by linear interpolation on the mesh
   * @tparam M The mesh type
   * @param m The mesh
   * @param f The function 
   */
  template <typename G, typename... Args> auto evaluate(G const &g, Args &&... args) requires(is_gf_v<G>) {

    if constexpr (not mesh::is_product_v<typename G::mesh_t>) {
      auto id = get_interpolation_data(g.mesh(), std::forward<Args>(args)...);
      return details::multivar_eval(g, id);
    } else { // special case for the product mesh
      return details::evaluate_impl(std::index_sequence_for<Args...>{}, g, std::forward<Args>(args)...);
    }
  }

} // namespace triqs::gfs
