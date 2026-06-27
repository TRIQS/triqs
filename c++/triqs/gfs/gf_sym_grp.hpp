// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2023 Simons Foundation
// Copyright (c) 2023 Hugo U.R. Strand
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
// Authors: Dominik Kiese, Nils Wentzell

/**
 * @file
 * @brief Provides the sym_grp class and concepts to detect, exploit and enforce symmetries of Green's functions.
 */

#pragma once

#include "./gf/gf.hpp"

#include <nda/sym_grp.hpp>

#include <array>
#include <cstddef>
#include <functional>
#include <tuple>
#include <utility>
#include <vector>

namespace triqs::gfs {

  /**
   * @addtogroup triqs-gfs-symmetry
   * @{
   */

  /**
   * @brief Symmetry concept for scalar-valued Green's functions.
   *
   * @details A ScalarGfSymmetry accepts a mesh index and returns a new mesh index together with the residual
   * nda::operation to be applied to the data.
   *
   * @tparam F Anything callable with a `mesh_index_t`.
   * @tparam G Anything modeling the gf concept (with `target_rank == 0`).
   * @tparam mesh_index_t Mesh index type for `G`.
   */
  template <typename F, typename G, typename mesh_index_t = typename G::mesh_t::index_t>
  concept ScalarGfSymmetry = is_gf_v<G> and //
     requires(F f, mesh_index_t const &mesh_index) {
       requires(G::target_rank == 0);
       { f(mesh_index) } -> std::same_as<std::tuple<mesh_index_t, nda::operation>>;
     };

  /**
   * @brief Symmetry concept for tensor-valued Green's functions.
   *
   * @details A TensorGfSymmetry accepts a mesh index and a target index and returns a new mesh index, a new
   * target index and the residual nda::operation to be applied to the data.
   *
   * @tparam F Anything callable with a `mesh_index_t` and a `target_index_t`.
   * @tparam G Anything modeling the gf concept (with `target_rank > 0`).
   * @tparam mesh_index_t Mesh index type for `G`.
   * @tparam target_index_t Target index type for `G`.
   */
  template <typename F, typename G, typename mesh_index_t = typename G::mesh_t::index_t,
            typename target_index_t = std::array<long, static_cast<std::size_t>(G::target_rank)>>
  concept TensorGfSymmetry = is_gf_v<G> and //
     requires(F f, mesh_index_t const &mesh_index, target_index_t const &target_index) {
       requires(G::target_rank > 0);
       { f(mesh_index, target_index) } -> std::same_as<std::tuple<mesh_index_t, target_index_t, nda::operation>>;
     };

  /**
   * @brief Initializer-function concept for scalar-valued Green's functions.
   *
   * @details A ScalarGfInitFunc accepts a mesh index and returns the (scalar) value type of the Green's function.
   *
   * @tparam F Anything callable with a `mesh_index_t`.
   * @tparam G Anything modeling the gf concept (with `target_rank == 0`).
   * @tparam mesh_index_t Mesh index type for `G`.
   */
  template <typename F, typename G, typename mesh_index_t = typename G::mesh_t::index_t>
  concept ScalarGfInitFunc = is_gf_v<G> and //
     requires(F f, mesh_index_t const &mesh_index) {
       requires(G::target_rank == 0);
       { f(mesh_index) } -> std::same_as<typename G::scalar_t>;
     };

  /**
   * @brief Initializer-function concept for tensor-valued Green's functions.
   *
   * @details A TensorGfInitFunc accepts a mesh index and a target index and returns the value type of the
   * Green's function.
   *
   * @tparam F Anything callable with a `mesh_index_t` and a `target_index_t`.
   * @tparam G Anything modeling the gf concept (with `target_rank > 0`).
   * @tparam mesh_index_t Mesh index type for `G`.
   * @tparam target_index_t Target index type for `G`.
   */
  template <typename F, typename G, typename mesh_index_t = typename G::mesh_t::index_t,
            typename target_index_t = std::array<long, static_cast<std::size_t>(G::target_rank)>>
  concept TensorGfInitFunc = is_gf_v<G> and //
     requires(F f, mesh_index_t const &mesh_index, target_index_t const &target_index) {
       requires(G::target_rank > 0);
       { f(mesh_index, target_index) } -> std::same_as<typename G::scalar_t>;
     };

  // make tuple from array
  template <typename A> auto to_tuple(A const &arr) {
    constexpr auto fetch = [](auto const &...xs) { return std::tuple{xs...}; };
    return std::apply(fetch, arr);
  }

  // make tuple from array using index_sequence
  template <std::size_t R, typename A> auto to_tuple(A const &arr) {
    return [&]<std::size_t... Is>(std::index_sequence<Is...>) { return std::tuple{arr[Is]...}; }(std::make_index_sequence<R>{});
  }

  // make array from tuple
  template <typename tpl_t> auto to_array(tpl_t const &tpl) {
    constexpr auto fetch = [](auto const &...xs) { return std::array{xs...}; };
    return std::apply(fetch, tpl);
  }

  /**
   * @brief Symmetry group of a Green's function.
   *
   * @details A sym_grp deduces the symmetry classes of the data of a Green's function from a list of symmetries
   * (each modeling ScalarGfSymmetry or TensorGfSymmetry) and exposes methods to reduce the function to its
   * representative data, reconstruct it, initialize it from an init function and symmetrize it. Internally it
   * delegates to an nda::sym_grp operating on the underlying data array.
   *
   * @tparam F Anything modeling either ScalarGfSymmetry or TensorGfSymmetry with `G`.
   * @tparam G Anything modeling the gf concept.
   */
  template <typename F, typename G>
    requires(is_gf_v<G> && (ScalarGfSymmetry<F, G> || TensorGfSymmetry<F, G>))
  class sym_grp {

    private:
    // data aliases
    using data_t           = typename G::data_t;
    using value_t          = typename G::scalar_t;
    using data_index_t     = std::array<long, static_cast<std::size_t>(nda::get_rank<data_t>)>;
    using data_sym_func_t  = std::function<std::tuple<data_index_t, nda::operation>(data_index_t const &)>;
    using data_init_func_t = std::function<value_t(data_index_t const &)>;

    // mesh aliases
    using mesh_index_t              = typename G::mesh_t::index_t;
    static constexpr auto mesh_rank = n_variables<typename G::mesh_t>;

    // target aliases
    static constexpr size_t target_rank = G::target_rank;
    using target_index_t                = std::array<long, static_cast<std::size_t>(target_rank)>;

    // members
    nda::sym_grp<data_sym_func_t, data_t> data_sym_grp; // symmetry group instance for the data array

    // convert from gf to nda symmetry
    data_sym_func_t to_data_symmetry(F const &f, G const &g) const {

      auto fp = [f, m = g.mesh()](data_index_t const &x) -> std::tuple<data_index_t, nda::operation> {
        // init new data index and residual operation
        data_index_t xp;

        if constexpr (target_rank == 0) { // scalar valued gfs

          if constexpr (mesh_rank == 1) {
            auto [new_mesh_index, op] = f(m.to_index(x[0]));
            xp[0]                     = m.to_data_index(new_mesh_index);
            return {xp, op};

          } else { // product mesh
            auto [new_mesh_index, op] = f(m.to_index(to_tuple(x)));
            xp                        = to_array(m.to_data_index(new_mesh_index));
            return {xp, op};
          }

        } else { // tensor valued gfs

          // convert data index to target index
          target_index_t target_index;
          for (auto i : range(target_rank)) target_index[i] = x[i + mesh_rank];

          if constexpr (mesh_rank == 1) {
            // evaluate symmetry
            auto [new_mesh_index, new_target_index, op] = f(m.to_index(x[0]), target_index);

            // convert mesh index + target index back to data index
            xp[0] = m.to_data_index(new_mesh_index);
            for (auto i : range(target_rank)) xp[i + mesh_rank] = new_target_index[i];

            return {xp, op};

          } else { // product mesh
            // evaluate symmetry
            auto [new_mesh_index, new_target_index, op] = f(m.to_index(to_tuple<mesh_rank>(x)), target_index);

            // convert mesh index + target index back to data index
            auto new_mesh_arr = to_array(m.to_data_index(new_mesh_index));
            for (auto i : range(mesh_rank)) xp[i] = new_mesh_arr[i];
            for (auto i : range(target_rank)) xp[i + mesh_rank] = new_target_index[i];

            return {xp, op};
          }
        }
      };

      return fp;
    };

    // convert from list of gf symmetries to list of nda symmetries
    std::vector<data_sym_func_t> to_data_symmetry_list(G const &g, std::vector<F> const &sym_list) const {
      std::vector<data_sym_func_t> data_sym_list;
      for (auto f : sym_list) data_sym_list.push_back(to_data_symmetry(f, g));
      return data_sym_list;
    }

    // convert from gf to nda init function
    template <typename H> data_init_func_t to_data_init_func(G const &g, H const &h) const {

      auto hp = [h, m = g.mesh()](data_index_t const &x) {
        if constexpr (target_rank == 0) { // scalar valued gfs

          if constexpr (mesh_rank == 1) {
            return h(m.to_index(x[0]));

          } else { // product mesh
            return h(m.to_index(to_tuple(x)));
          }

        } else { // tensor valued gfs

          target_index_t target_index;
          for (auto i : range(target_rank)) target_index[i] = x[i + mesh_rank];

          if constexpr (mesh_rank == 1) {
            return h(m.to_index(x[0]), target_index);

          } else { // product mesh
            return h(m.to_index(to_tuple<mesh_rank>(x)), target_index);
          }
        }
      };

      return hp;
    }

    public:
    /**
     * @brief Accessor for the symmetry group of the data array.
     *
     * @return The underlying nda::sym_grp instance operating on the data array.
     */
    [[nodiscard]] nda::sym_grp<data_sym_func_t, data_t> const &get_data_sym_grp() const { return data_sym_grp; }

    /**
     * @brief Accessor for the number of symmetry classes.
     *
     * @return The number of deduced symmetry classes.
     */
    [[nodiscard]] long num_classes() const { return data_sym_grp.num_classes(); }

    /**
     * @brief Reduce a Green's function to its representative data using the symmetries.
     *
     * @param g A Green's function.
     * @return A vector of data values for the representative element of each symmetry class.
     */
    [[nodiscard]] std::vector<value_t> get_representative_data(G const &g) const { return data_sym_grp.get_representative_data(g.data()); }

    /**
     * @brief Initialize a Green's function from its representative data using the symmetries.
     *
     * @tparam V The type of the data container (vector or vector view).
     * @param g The Green's function to fill in place.
     * @param vec The data values for the representative element of each symmetry class.
     */
    template <typename V> void init_from_representative_data(G &g, V const &vec) const { data_sym_grp.init_from_representative_data(g.data(), vec); }

    /**
     * @brief Default constructor.
     */
    sym_grp() = default;

    /**
     * @brief Construct the symmetry group of a Green's function from a list of symmetries.
     *
     * @param g A Green's function.
     * @param sym_list A list of symmetries, each modeling one of the gf symmetry concepts.
     * @param max_length Maximum recursion depth for out-of-bounds projection (default \f$ 0 \f$).
     */
    sym_grp(G const &g, std::vector<F> const &sym_list, long const max_length = 0)
       : data_sym_grp{g.data(), to_data_symmetry_list(g, sym_list), max_length} {};

    /**
     * @brief Initialize a Green's function using an init function.
     *
     * @details Iterates over all symmetry classes and propagates the result of evaluating the init function on
     * each representative element.
     *
     * @tparam H Anything modeling either ScalarGfInitFunc or TensorGfInitFunc with `G`.
     * @param g The Green's function to fill in place.
     * @param h The init function to be used.
     * @param parallel If true, evaluate the init function with OpenMP in parallel (default false).
     */
    template <typename H>
    void init(G &g, H const &h, bool parallel = false) const
      requires(ScalarGfInitFunc<H, G> || TensorGfInitFunc<H, G>)
    {
      data_sym_grp.init(g.data(), to_data_init_func(g, h), parallel);
    }

    /**
     * @brief Symmetrize a Green's function in place.
     *
     * @details Enforces the symmetries on `g` and reports the maximum symmetry violation encountered and the
     * mesh and target index at which it occurs.
     *
     * @param g The Green's function to symmetrize in place.
     * @return A tuple of the maximum symmetry violation and the corresponding mesh and target index.
     */
    std::tuple<double, mesh_index_t, target_index_t> symmetrize(G &g) const {
      auto const &[max_diff, max_index] = data_sym_grp.symmetrize(g.data());
      auto const m                      = g.mesh();

      if constexpr (target_rank == 0) { // scalar valued gfs

        if constexpr (mesh_rank == 1) {
          return {max_diff, m.to_index(max_index[0]), {}};

        } else { // product mesh
          return {max_diff, m.to_index(to_tuple(max_index)), {}};
        }

      } else { // tensor valued gfs

        // convert data index to target index
        target_index_t target_index;
        for (auto i : range(target_rank)) target_index[i] = max_index[i + mesh_rank];

        if constexpr (mesh_rank == 1) {
          return {max_diff, m.to_index(max_index[0]), target_index};

        } else { // product mesh
          return {max_diff, m.to_index(to_tuple<mesh_rank>(max_index)), target_index};
        }
      }
    }
  };

  /** @} */

} // namespace triqs::gfs
