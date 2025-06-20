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

/**
 * @file
 * @brief Provides generic evaluation of functions defined on meshes.
 */

#pragma once

#include "./concepts.hpp"
#include "./prod.hpp"
#include "./utils.hpp"

#include <nda/nda.hpp>

#include <tuple>
#include <utility>

namespace triqs::mesh {

  /**
   * @addtogroup triqs-meshes-utils
   * @{
   */

  namespace detail {

    // Create a new tuple by removing the first element of a given tuple.
    template <typename T>
      requires(std::tuple_size_v<T> >= 1)
    auto pop_front_tuple(T const &tup) {
      return [&]<size_t... Is>(std::index_sequence<Is...>) {
        return std::tie(std::get<Is + 1>(tup)...);
      }(std::make_index_sequence<std::tuple_size_v<T> - 1>{});
    }
  } // namespace detail

  /**
   * @brief Evaluate a function \f$ f \f$ at the index of a mesh point of the given mesh.
   * 
   * @details It simply forwards the index to the function object \f$ f \f$.
   *
   * @tparam M triqs::mesh::Mesh type.
   * @param f Callable object \f$ f \f$ representing the function to be evaluated.
   * @param n %Mesh point index \f$ n \f$.
   * @return Result of the function call.
   */
  template <Mesh M> FORCEINLINE auto evaluate(M const &, auto const &f, typename M::index_t const &n) { return f(n); }

  /**
   * @brief Evaluate a function \f$ f \f$ at a triqs::mesh::closest_mesh_point_t object.
   * 
   * @details It simply forwards the given object to the function object \f$ f \f$.
   *
   * @tparam T Value type of triqs::mesh::closest_mesh_point_t.
   * @param f Callable object \f$ f \f$ representing the function to be evaluated.
   * @param cmp triqs::mesh::closest_mesh_point_t object.
   * @return Result of the function call.
   */
  template <typename T> FORCEINLINE auto evaluate(Mesh auto const &, auto const &f, mesh::closest_mesh_point_t<T> const &cmp) { return f(cmp); }

  /**
   * @brief Evaluate a function \f$ f \f$ for `nda::range::all`.
   * 
   * @details It simply calls the function object \f$ f \f$ with `nda::range::all`.
   *
   * @param f Callable object \f$ f \f$ representing the function to be evaluated.
   * @return Result of the function call.
   */
  FORCEINLINE auto evaluate(Mesh auto const &, auto const &f, nda::range::all_t) { return f(nda::range::all_t{}); }

  /**
   * @brief Evaluate a function \f$ f \f$ at a specific mesh point of the given mesh.
   *
   * @details If the given mesh satisfies the triqs::mesh::MeshWithValues concept, it calls `evaluate` with the mesh, 
   * the function object \f$ f \f$ and the value of the mesh point. Otherwise, it calls \f$ f \f$ with the mesh point 
   * itself.
   *
   * @tparam M triqs::mesh::Mesh type.
   * @param m %Mesh object.
   * @param f Callable object \f$ f \f$ representing the function to be evaluated.
   * @param mp %Mesh point of the mesh.
   * @return Result of the evaluation.
   */
  template <Mesh M> FORCEINLINE auto evaluate(M const &m, auto const &f, typename M::mesh_point_t const &mp) {
    if constexpr (MeshWithValues<M>) {
      return evaluate(m, f, mp.value());
    } else {
      return f(mp);
    }
  }

  /**
   * @brief Evaluate a multivariate function \f$ f \f$ defined on the given domains (meshes) at the given arguments.
   *
   * @details The function is evaluated by currying, i.e. it is evaluated step by step for each domain.
   *
   * Suppose that we want to evaluate the function \f$ f : \mathrm{D}_1 \times \dots \times \mathrm{D}_n \to \mathrm{R}
   * \f$ at the points \f$ x_1 \in \mathrm{D}_1, \dots, x_n \in \mathrm{D}_n \f$. Then currying works by creating a
   * sequence of functions each taking one argument:
   * \f[
   *   \mathrm{curry}(f) : \mathrm{D}_n \to ( \mathrm{D_{n-1}} \to ( \dots \to ( \mathrm{D}_1 \to \mathrm{R} ) ) ) \; .
   * \f]
   * Here, \f$ \mathrm{curry}(f) \f$ is a function that
   * - takes an argument \f$ x_n \in \mathrm{D}_n \f$ and returns a new function \f$ f_{x_n} \f$ that
   * - takes an argument \f$ x_{n-1} \in \mathrm{D}_{n-1} \f$ and returns a new function \f$ f_{x_n, x_{n-1}} \f$ that
   * - . . .
   * - takes an argument \f$ x_{2} \in \mathrm{D}_2 \f$ and returns a new function \f$ f_{x_n, x_{n-1}, \dots, x_2} \f$
   * that
   * - takes an argument \f$ x_1 \in \mathrm{D}_1 \f$ and returns the final result \f$ f(x_1, \dots, x_n) \f$.
   * 
   * This is used to evaluate @ref triqs-gfs defined on product meshes.
   * 
   * @note The intermediate functions objects \f$ f_{x_n, \dots} \f$ are hidden by nested `evaluate` calls.
   *
   * @code
   * #include <triqs/mesh.hpp>
   * #include <iostream>
   *
   * // Dummy domain.
   * struct domain {};
   *
   * // Overload evaluate for a single dummy domain.
   * auto evaluate(domain d, auto f, double x) { return f(x); }
   *
   * int main() {
   *   // function to evaluate by currying
   *   auto f = [](double x, double y, double z) { return x + y + z; };
   *
   *   // calculate f(1, 2, 3) = 6 using triqs::mesh::evaluate
   *   std::cout << triqs::mesh::evaluate(std::make_tuple(domain{}, domain{}, domain{}), f, 1, 2, 3) << std::endl;
   * }
   * @endcode
   *
   * Output:
   *
   * ```
   * 6
   * ```
   *
   * @tparam Ds Domain (%Mesh) types.
   * @tparam X1 First argument type.
   * @tparam Xs Remaining argument types.
   * @param tup Tuple of the domains (meshes) \f$ \mathrm{D}_1, \dots, \mathrm{D}_n \f$.
   * @param f Callable object \f$ f \f$ representing the function to be evaluated.
   * @param x1 First argument \f$ x_1 \in \mathrm{D}_1 \f$.
   * @param xs Remaining arguments \f$ x_2 \in \mathrm{D}_2, \dots, x_n \in \mathrm{D}_n \f$.
   * @return Result of the evaluation \f$ f(x_1, \dots, x_n) \f$.
   */
  template <typename... Ds, typename X1, typename... Xs>
  FORCEINLINE auto evaluate(std::tuple<Ds...> const &tup, auto const &f, X1 const &x1, Xs const &...xs) {
    auto const &d1 = std::get<0>(tup);
    if constexpr (sizeof...(Ds) > 1) {
      return evaluate(
         detail::pop_front_tuple(tup),
         [f, &x1, &d1](auto const &...ys) __attribute__((always_inline)) {
           return evaluate(d1, [f, &ys...](auto const &y1) __attribute__((always_inline)) { return f(y1, ys...); }, x1);
         },
         xs...);
    } else {
      return evaluate(d1, f, x1);
    }
  }

  /**
   * @brief Evaluate a multivariate function \f$ f \f$ defined on the given product mesh at the given arguments.
   *
   * @details It simply forwards the arguments and the components of the product mesh to 
   * @ref triqs::mesh::evaluate(std::tuple<Ds...> const &tup, auto const &f, X1 const &x1, Xs const &...xs) 
   * "triqs::mesh::evaluate".
   *
   * @tparam Ms triqs::mesh::Mesh types of the product mesh.
   * @tparam Xs Argument types.
   * @param m Product mesh.
   * @param f Callable object \f$ f \f$ representing the function to be evaluated.
   * @param xs Arguments \f$ x_1, \dots, x_n \f$.
   * @return Result of the evaluation \f$ f(x_1, \dots, x_n) \f$.
   */
  template <Mesh... Ms, typename... Xs> FORCEINLINE auto evaluate(mesh::prod<Ms...> const &m, auto const &f, Xs const &...xs) {
    return evaluate(m.components(), f, xs...);
  }

  /** @} */

} // namespace triqs::mesh
