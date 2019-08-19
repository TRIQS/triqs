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
#include "./comma.hpp"

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
    template <int I0, typename G, typename A0> FORCEINLINE auto __add(G const &g, A0 const &a0) {
#define TRIQS_TERM_EVAL a0[I0].second *g[a0[I0].first]
      if constexpr (I0 < std::tuple_size<A0>::value - 1) {
        return TRIQS_TERM_EVAL + __add<I0 + 1>(g, a0);
      } else
        return TRIQS_TERM_EVAL;
#undef TRIQS_TERM_EVAL
    }

    template <int I0, int I1, typename G, typename A0, typename A1> FORCEINLINE auto __add(G const &g, A0 const &a0, A1 const &a1) {
#define TRIQS_TERM_EVAL a0[I0].second *a1[I1].second *g[make_tuple_com(a0[I0].first, a1[I1].first)]
      if constexpr (I0 < std::tuple_size<A0>::value - 1) {
        return TRIQS_TERM_EVAL + __add<I0 + 1, I1>(g, a0, a1);
      } else { // I0==0
        if constexpr (I1 < std::tuple_size<A1>::value - 1)
          return TRIQS_TERM_EVAL + __add<0, I1 + 1>(g, a0, a1);
        else
          return TRIQS_TERM_EVAL;
#undef TRIQS_TERM_EVAL
      }
    }

    template <int I0, int I1, int I2, typename G, typename A0, typename A1, typename A2>
    FORCEINLINE auto __add(G const &g, A0 const &a0, A1 const &a1, A2 const &a2) {
#define TRIQS_TERM_EVAL a0[I0].second *a1[I1].second *a1[I2].second *g[make_tuple_com(a0[I0].first, a1[I1].first, a2[I2].first)]
      if constexpr (I0 < std::tuple_size<A0>::value - 1) {
        return TRIQS_TERM_EVAL + __add<I0 + 1, I1, I2>(g, a0, a1, a2);
      } else {
        if constexpr (I1 < std::tuple_size<A1>::value - 1) {
          return TRIQS_TERM_EVAL + __add<0, I1 + 1, I2>(g, a0, a1, a2);
        } else {
          if constexpr (I2 < std::tuple_size<A2>::value - 1)
            return TRIQS_TERM_EVAL + __add<0, 0, I2 + 1>(g, a0, a1, a2);
          else
            return TRIQS_TERM_EVAL;
#undef TRIQS_TERM_EVAL
        }
      }
    }

    template <int I0, int I1, int I2, int I3, typename G, typename A0, typename A1, typename A2, typename A3>
    FORCEINLINE auto __add(G const &g, A0 const &a0, A1 const &a1, A2 const &a2, A3 const &a3) {
#define TRIQS_TERM_EVAL                                                                                                                              \
  a0[I0].second *a1[I1].second *a2[I2].second *a3[I3].second *g[make_tuple_com(a0[I0].first, a1[I1].first, a2[I2].first, a3[I3].first)]
      if constexpr (I0 < std::tuple_size<A0>::value - 1) {
        return TRIQS_TERM_EVAL + __add<I0 + 1, I1, I2, I3>(g, a0, a1, a2, a3);
      } else {
        if constexpr (I1 < std::tuple_size<A1>::value - 1) {
          return TRIQS_TERM_EVAL + __add<0, I1 + 1, I2, I3>(g, a0, a1, a2, a3);
        } else {
          if constexpr (I2 < std::tuple_size<A2>::value - 1) {
            return TRIQS_TERM_EVAL + __add<0, 0, I2 + 1, I3>(g, a0, a1, a2, a3);
          } else {
            if constexpr (I3 < std::tuple_size<A3>::value - 1)
              return TRIQS_TERM_EVAL + __add<0, 0, 0, I3 + 1>(g, a0, a1, a2, a3);
            else
              return TRIQS_TERM_EVAL;
#undef TRIQS_TERM_EVAL
          }
        }
      }
    }

    // multivar_eval
    template <size_t... Is, typename G, typename... InterPolDataType>
    FORCEINLINE auto _multivar_eval_impl(std::index_sequence<Is...>, G const &g, InterPolDataType const &... a) {
      return __add<0 * Is...>(g, a...);
    }

    //
    template <typename G, typename... InterPolDataType> FORCEINLINE auto multivar_eval(G const &g, InterPolDataType const &... a) {
      return details::_multivar_eval_impl(std::index_sequence_for<InterPolDataType...>{}, g, a...);
    }

    // FIXME20 : use a lambda
    template <typename F, size_t... Is, typename... Args> auto evaluate_impl(std::index_sequence<Is...>, F const &f, Args &&... args) {
      return multivar_eval(f, std::get<Is>(f.mesh().components()).get_interpolation_data(std::forward<Args>(args))...);
    }
  } // namespace details

  // ------------------- evaluate --------------------------------
  // DOC ? Internal only ?
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

