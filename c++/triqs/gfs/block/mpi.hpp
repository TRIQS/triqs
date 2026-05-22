// Copyright (c) 2020-2021 Simons Foundation
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
// Authors: Michel Ferrero, Olivier Parcollet, Nils Wentzell

/**
 * @file
 * @brief Provides MPI routines for block Green's function objects.
 */

#pragma once

#include "./block_gf.hpp"
#include "../../utility/exceptions.hpp"

#include <mpi/mpi.hpp>

#include <cstddef>
#include <numeric>
#include <string>
#include <type_traits>

namespace triqs::gfs {

  namespace detail {

    // Check the shape of block GFs across all processes.
    template <typename G> bool have_mpi_equal_shape(const G &bg, const mpi::communicator &comm) {
      if constexpr (G::arity == 1) {
        return mpi::all_equal(bg.size(), comm);
      } else {
        return mpi::all_equal(bg.size1(), comm) && mpi::all_equal(bg.size2(), comm);
      }
    }

    // Hash the block names for comparison.
    template <typename G> std::size_t hash_names(G const &bg) {
      auto bin_op = [](std::size_t acc, auto const &name) { return acc + std::hash<std::string>{}(name); };
      if constexpr (G::arity == 1) {
        return std::accumulate(bg.block_names().begin(), bg.block_names().end(), std::size_t{0}, bin_op);
      } else {
        uint64_t hash = 0;
        for (auto const &vec : bg.block_names()) { hash += std::accumulate(vec.begin(), vec.end(), std::size_t{0}, bin_op); }
        return hash;
      }
    }

  } // namespace detail

  /**
   * @brief Implementation of an MPI broadcast for triqs::gfs::block_gf and triqs::gfs::block_gf_view types.
   *
   * @details It simply broadcasts the vector (of vectors) of GF objects. Furthermore,
   * - for non-view block GFs, it broadcasts the vector (of vectors) of block names and
   * - for views, it expects the block names to be the same on all processes.
   *
   * @tparam G Block GF type.
   * @param bg Block GF (view) to be broadcasted from/into.
   * @param c `mpi::communicator` object.
   * @param root Rank of the root process.
   */
  template <typename G>
    requires(BlockGreenFunction_v<G>)
  void mpi_broadcast(G &&bg, mpi::communicator c, int root) { // NOLINT (temporary views are allowed)
    constexpr bool is_view = std::decay_t<G>::is_view;

    // broadcast block names
    if constexpr (!is_view) {
      // for non-view block GFs, we broadcast the block names directly into the block GF
      mpi::broadcast(bg._block_names, c, root);
    } else {
      // for views, we keep the block names in the block GF but check that they are the same as on the root process
      auto names = bg.block_names();
      mpi::broadcast(names, c, root);
      EXPECTS(bg.block_names() == names);
    }

    // broadcast data
    mpi::broadcast(bg.data(), c, root);
  }

  /**
   * @brief Implementation of an MPI reduce for triqs::gfs::block_gf and triqs::gfs::block_gf_view types that reduces
   * directly into an existing block GF object.
   *
   * @details The function reduces input block GFs (views) from all processes in the given communicator and makes the
   * result available on the root process (`all == false`) or on all processes (`all == true`).
   *
   * It throws an exception if the input block GFs on all processes and the output block GF views on receiving processes
   * do not have the same shape. Furthermore, the block names of the input block GFs on all processes and of output
   * block GF views on receiving processes are expected to be the same.
   *
   * The content of the output block GF depends on the MPI rank and whether it receives the data or not:
   * - On receiving ranks, it contains the reduced GF objects obtained by calling `mpi::reduce_into` directly on the
   * vector (of vectors) of GFs and the same block names as the input block GF (the block names are assigned for
   * non-views but for views, it is expected that they already have the correct block names).
   * - On non-receiving ranks, the output block GF is ignored and left unchanged.
   *
   * @tparam G1 Block GF type.
   * @tparam G2 Block GF type.
   * @param bg_in Block GF (view) to be reduced.
   * @param bg_out Block GF (view) to be reduced into.
   * @param comm `mpi::communicator` object.
   * @param root Rank of the root process.
   * @param all Should all processes receive the result of the reduction.
   * @param op MPI reduction operation.
   */
  template <typename G1, typename G2>
    requires(BlockGreenFunction_v<G1> and BlockGreenFunction_v<G2>)
  void mpi_reduce_into(G1 const &bg_in, G2 &&bg_out, mpi::communicator c, int root, // NOLINT (temporary views are allowed here)
                       bool all, MPI_Op op) {
    constexpr bool is_view = std::decay_t<G2>::is_view;

    // check the shape and block names of the input block GFs
    EXPECTS(mpi::all_equal(detail::hash_names(bg_in), c));
    if (not detail::have_mpi_equal_shape(bg_in, c))
      TRIQS_RUNTIME_ERROR << "Error in triqs::gfs::mpi_reduce_into: Shapes of input block GFs must be equal";

    // assign (check) the block names of the output block GF (view) on receiving ranks
    if ((c.rank() == root || all)) {
      if constexpr (is_view) {
        EXPECTS(bg_in.block_names() == bg_out.block_names());
      } else {
        bg_out._block_names = bg_in.block_names();
      }
    }

    // reduce the vector (of vectors) of GF objects
    mpi::reduce_into(bg_in.data(), bg_out.data(), c, root, all, op);
  }

  /**
   * @brief Implementation of an MPI reduce for triqs::gfs::block_gf and triqs::gfs::block_gf_view types.
   *
   * @details The function reduces input block GFs (views) from all processes in the given communicator and makes the
   * result available on the root process (`all == false`) or on all processes (`all == true`).
   *
   * It calls triqs::gfs::mpi_reduce_into with the input block GF and a default constructed block GF object.
   *
   * The content of the returned block GF depends on the MPI rank and whether it receives the data or not:
   * - On receiving ranks, it contains the reduced GF objects and the same block names as the input block GF.
   * - On non-receiving ranks, a default constructed block GF is returned.
   *
   * @tparam G Block GF type.
   * @param bg Block GF (view) to be reduced (into).
   * @param comm `mpi::communicator` object.
   * @param root Rank of the root process.
   * @param all Should all processes receive the result of the reduction.
   * @param op MPI reduction operation.
   * @return A triqs::gfs::block_gf object with the reduced data.
   */
  template <typename G>
    requires(BlockGreenFunction_v<G>)
  auto mpi_reduce(G const &bg, mpi::communicator c = {}, int root = 0, bool all = false, MPI_Op op = MPI_SUM) {
    auto res = typename G::regular_type{};
    mpi_reduce_into(bg, res, c, root, all, op);
    return res;
  }

} // namespace triqs::gfs
