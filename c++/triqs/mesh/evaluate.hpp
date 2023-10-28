// Copyright (c) 2022-2023 Simons Foundation
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
// Authors: Olivier Parcollet, Nils Wentzell

#pragma once
#include <array>
#include "./concepts.hpp"
//#include "../utility/macros.hpp"
#include "./prod.hpp"

// We write the evaluation on the cartesian product of meshes generically.
// i.e. on a tuple of meshes (m1, m...)
// We currify the function, and call evaluation for each step of the curry.
//
// We introduce a function
//           evaluate(m, f, x)
// that evaluates any function f on a mesh at point x of a mesh m
//
// We denote a lambda here as  x -> f(x) for a light notation.
// If f in any function with N arguments
//
//    evaluate( (m1, m...),  (y1, y...) -> f(y1, y...),    (x1, x...))
// =  evaluate( (m...),     (y...) -> evaluate(m1, y1-> f(y1, y...), x1),      (x...))

// In other words, we currify the function as :
//                  yn -> ... -> y2 -> y1 -> f(y1, y2, yn)
// an apply evaluate at each step.
//
// Example :
// e.g.  3 meshes  m1, m2, m3
// evaluate(m1, f, x1) = sum w1(x1) f(z1(x1)) // write z1 simply later
// evaluate(m2, f, x2) =  w2 f(z2)            // from now on, I omit x deps of z, and "sum" for simplicity
// evaluate(m3, f, x3) =  w3 f(z3)
// then (I also add the explicit capture to be closer to C++ implementation below)
//
// evaluate( (m1,m2), (y1, y2) -> g(y1, y2)  , (x1, x2))
//                                 = evaluate( (m2), y2 -> evaluate(m1, [y2](y1) -> g[y1, y2), x1) ,  (x2) )
//                                 = evaluate( (m2), y2 -> w1 g(z1, y2),  (x2) )
//                                 = w2 w1 g(z1, z2)
//
// evaluate( (m1,m2,m3), g, (x1, x2, x3)) = evaluate( (m2, m3), (y2, y3)                  -> evaluate(m1, [y2, y3](y1) -> g(y1, y2, y3), x1)     , (x2, x3) )
//                                        = evaluate( (m3), (y3) -> evaluate(m2, [y3](y2) -> evaluate(m1, [y2, y3](y1) -> g(y1, y2, y3), x1), x2), (x3) )
//
//                                        = w3                  evaluate(m2, [z3](y2) -> evaluate(m1, [y2, z3](y1) -> g(y1, y2, z3), x1), x2 )
//                                        = w3 w2                                        evaluate(m1, [z2, z3](y1) -> g(y1, z2, z3), x1)
//                                        = w3 w2 w1                                                                  g(z1, z2, z3))
//
namespace triqs::mesh { // NOLINT
  namespace detail {

    // Take a tuple and return a new tuple without the first element
    template <typename Tu> auto pop_front_tuple(Tu const &tu) {
      static constexpr auto S = std::tuple_size_v<Tu>;
      static_assert(S >= 1);
      return [&]<size_t... Is>(std::index_sequence<Is...>) { return std::tie(std::get<Is + 1>(tu)...); }(std::make_index_sequence<S - 1>{});
    }
  } // namespace detail

  // evaluate on a mesh index just pass through
  template <Mesh M> FORCEINLINE auto evaluate([[maybe_unused]] M const &m, auto const &f, typename M::index_t const &index) { return f(index); }

  // evaluate on a closest_mesh_point : pass through, like an index.
  template <typename T> FORCEINLINE auto evaluate([[maybe_unused]] Mesh auto const &m, auto const &f, mesh::closest_mesh_point_t<T> const &cmp) { return f(cmp); }

  // all_t : pass through, like a (range of) index
  FORCEINLINE auto evaluate([[maybe_unused]] Mesh auto const &m, auto const &f, all_t) { return f(all_t{}); }

  // evaluate on a mesh point. Use its value if its available, else pass through
  template <Mesh M> FORCEINLINE auto evaluate(M const &m, auto const &f, typename M::mesh_point_t const &mp) {
    if constexpr (MeshWithValues<M>) {
      return evaluate(m, f, mp.value());
    } else {
      return f(mp);
    }
  }

  // Implementation for tuple.
  // NB the X points are passed as a pack, not a tuple.
  // We use the recursive formula above, except with size of the tuple is 1, where we simply call other evaluate overloads.
  // NB : do not forward here. Arguments are always taken by const & and it makes the
  // overloads clearer.
  // Do not restrict M here, as this function can be used (in brzone) with simpler objects which do not have the full Mesh concept.
  //
  template <typename... M, typename X1, typename... X>
  FORCEINLINE auto evaluate(std::tuple<M...> const &mesh_tuple, auto const &f, X1 const &x1, X const &...x) {
    auto const &m1 = std::get<0>(mesh_tuple);
    if constexpr (sizeof...(M) > 1)
      return evaluate(
         detail::pop_front_tuple(mesh_tuple), [ f, &x1, &m1 ](auto const &...y) __attribute__((always_inline)) {
           return evaluate(
              m1, [ f, &y... ](auto const &y1) __attribute__((always_inline)) { return f(y1, y...); }, x1);
         },
         x...);
    else
      return evaluate(m1, f, x1);
  }

  // Cartesian product mesh is done as a tuple
  template <Mesh... M, typename... X> FORCEINLINE auto evaluate(mesh::prod<M...> const &m, auto const &f, X const &...x) {
    return evaluate(m.components(), f, x...);
  }

} // namespace triqs::mesh
