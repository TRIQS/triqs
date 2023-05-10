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

namespace triqs::mesh {

  // We introduce a function
  // evaluate(m, f, x)
  // that evaluate any function f on a mesh at point x
  // m : a mesh
  // f : a lambda  m-> anything
  // The goal is to write the evaluation on the cartesian product of meshes generically.
  // i.e. on a tuple of meshes (m1, m...)
  // We currify the function, and call evaluation for each step of the curry
  // We denote a lambda here as  x -> f(x) for a light notation.
  // Then if f in any function with N arguments
  //
  //  evaluate( (m1, m...),  (y1, y...) -> f(y1, y...),    (x1, x...))
  // =  evaluate( (m...),  (y...) -> evaluate(m1, y1-> f(y1, y...), x1),  (x...))

  // in other words, I consider the function as yn -> ... -> y2 -> y1 -> f(y1, y2, yn)
  // an apply evaluate at each step.
  //
  // Example :
  // e.g.  3 meshes  m1, m2, m3
  // evaluate(m1, f, x1) = sum w1(x1) f(z1(x1)) // write z1 simply later
  // evaluate(m2, f, x2) = sum w2 f(z2)         // from now on, I omit x deps of z, and "sum" for simplicity
  // evaluate(m3, f, x3) = sum w3 f(z3)
  // then (I also add the explicit capture to be closer to C++ implementation below)
  //
  // evaluate( (m1,m2), g, (x1, x2)) =  evaluate( (m2), y2 -> evaluate(m1, [y2](y1) -> g[y1, y2], x1) ,  (x2) )
  //                                    // now apply the evaluate
  //                                 =  evaluate( (m2), y2 -> w1 g[z1, y2],  (x2) )
  //                                 =  w2 w1 g[z1, z2]
  //
  // evaluate( (m1,m2,m3), g, (x1, x2, x3)) = evaluate( (m2, m3), (y2, y3)                  -> evaluate(m1, [y2, y3](y1) -> g(y1, y2, y3), x1)     , (x2, x3) )
  //                                        = evaluate( (m3), (y3) -> evaluate(m2, [y3](y2) -> evaluate(m1, [y2, y3](y1) -> g(y1, y2, y3), x1), x2), (x3) )
  //
  //                                        = w3                  evaluate(m2, [z3](y2) -> evaluate(m1, [y2, z3](y1) -> g(y1, y2, z3), x1), x2 )
  //                                        = w3 w2                                        evaluate(m1, [z2, z3](y1) -> g(y1, z2, z3), x1)
  //                                        = w3 w2 w1                                                                  g(z1, z2, z3))
  //

  namespace detail {

    template <typename Tu> auto pop_front_tuple(Tu const &tu) {
      return [&]<size_t... Is>(std::index_sequence<Is...>) { return std::tie(std::get<Is + 1>(tu)...); }
      (std::make_index_sequence<std::tuple_size_v<Tu> - 1>{});
    }
  } // namespace detail

  template <Mesh M> FORCEINLINE decltype(auto) evaluate(M const &m, auto const &f, typename M::idx_t const &idx) { return f(idx); }

  template <Mesh M> FORCEINLINE decltype(auto) evaluate(M const &m, auto const &f, typename M::mesh_point_t const &mp) {
    if constexpr (MeshWithValues<M>) {
      return evaluate(m, f, mp.value());
    } else {
      return f(mp);
    }
  }

  template <typename T> FORCEINLINE decltype(auto) evaluate(Mesh auto const &m, auto const &f, mesh::closest_mesh_point_t<T> const &cmp) { return f(cmp); }

  FORCEINLINE decltype(auto) evaluate(Mesh auto const &m, auto const &f, all_t) { return f(all_t{}); }

  // Implementation for tuple.
  // We use the recursive formula above, except with size of the tuple is 1, where we simply call other evaluate overloads.
  // NB : do not forward here. Arguments are always taken by const & and it makes the
  // overloads clearer.
  // Do not restrict M here, as this function can be used (in brzone) with simpler objects which do not have the full Mesh concept.
  // FIXME : Return auto or decltype(auto) ? If latter, add it to the lambda too...
  //
  template <typename... M, typename X1, typename... X>
  FORCEINLINE decltype(auto) evaluate(std::tuple<M...> const &mesh_tuple, auto const &f, X1 const &x1, X const &...x) {
    auto const &m1 = std::get<0>(mesh_tuple);
    if constexpr (sizeof...(M) > 1)
      return evaluate(
         detail::pop_front_tuple(mesh_tuple), [ f, &x1, &m1 ](auto const &...y) __attribute__((always_inline)) {
           return evaluate(
              m1, [f, &y...](auto const &y1) { return f(y1, y...); }, x1);
         },
         x...);
    else
      return evaluate(m1, f, x1);
  }

  // Cartesian product mesh
  template <Mesh... M, typename... X> FORCEINLINE decltype(auto) evaluate(mesh::prod<M...> const &m, auto const &f, X const &...x) {
    return evaluate(m.components(), f, x...);
  }

} // namespace triqs::mesh
