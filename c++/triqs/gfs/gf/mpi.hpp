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
 * @brief Provides MPI routines for Green's function objects.
 */

#pragma once

#include "./gf.hpp"

#include <mpi/mpi.hpp>
#include <nda/nda.hpp>

#include <type_traits>

namespace triqs::gfs {

  /**
   * @brief Implementation of an MPI broadcast for triqs::gfs::gf, triqs::gfs::gf_view or triqs::gfs::gf_const_view
   * types.
   *
   * @details It simply broadcasts the data of the GF, i.e. the nda array/view object. Furthermore,
   * - for non-view GFs, it also broadcasts the mesh and
   * - for views, it expects the mesh to be the same on all processes.
   *
   * @tparam G triqs::gfs::MemoryGf type.
   * @param g GF (view) to be broadcasted from/into.
   * @param c `mpi::communicator` object.
   * @param root Rank of the root process.
   */
  template <MemoryGf G> void mpi_broadcast(G &&g, mpi::communicator c, int root) { // NOLINT (temporary views are allowed)
    constexpr bool is_view = std::decay_t<G>::is_view;

    // broadcast mesh
    if constexpr (!is_view) {
      // for non-view GFs, we broadcast the mesh directly into the GF
      mpi::broadcast(g._mesh, c, root);
    } else {
      // for views, we keep the mesh in the GF but check that it is the same as the mesh on the root process
      auto m = g.mesh();
      mpi::broadcast(m, c, root);
      EXPECTS(m.mesh_hash() == g.mesh().mesh_hash());
    }

    // broadcast data
    mpi::broadcast(g.data(), c, root);
  }

  /**
   * @brief Implementation of an MPI reduce for triqs::gfs::gf, triqs::gfs::gf_view or triqs::gfs::gf_const_view types
   * that reduces directly into an existing GF object.
   *
   * @details The function reduces input GFs (views) from all processes in the given communicator and makes the result
   * available on the root process (`all == false`) or on all processes (`all == true`).
   *
   * Input GFs on all processes and output GF views on receiving processes are expected to have the same mesh.
   *
   * The content of the output GF depends on the MPI rank and whether it receives the data or not:
   * - On receiving ranks, it contains the reduced data obtained from calling `mpi::reduce_into` and the same mesh as
   * the input GF (the mesh is assigned for non-views but for views, it is expected that it already has the correct
   * mesh).
   * - On non-receiving ranks, the output GF is ignored and left unchanged.
   *
   * The reduction of the data is performed in place if the input and output GFs point to the same memory location.
   *
   * @tparam G1 triqs::gfs::MemoryGf type.
   * @tparam G2 triqs::gfs::MemoryGf type.
   * @param g_in GF (view) to be reduced.
   * @param g_out GF (view) to be reduced into.
   * @param comm `mpi::communicator` object.
   * @param root Rank of the root process.
   * @param all Should all processes receive the result of the reduction.
   * @param op MPI reduction operation.
   */
  template <MemoryGf G1, MemoryGf G2>
  void mpi_reduce_into(G1 const &g_in, G2 &&g_out, mpi::communicator c, // NOLINT (temporary views are allowed here)
                       int root, bool all, MPI_Op op) {
    constexpr bool is_view = std::decay_t<G2>::is_view;

    // check the meshes of the input GFs
    EXPECTS(mpi::all_equal(g_in.mesh().mesh_hash(), c));

    // assign (check) the mesh of the output GF (view) on receiving ranks
    if (c.rank() == root || all) {
      if constexpr (is_view) {
        EXPECTS(g_in.mesh().mesh_hash() == g_out.mesh().mesh_hash());
      } else {
        g_out._mesh = g_in.mesh();
      }
    }

    // reduce the data
    mpi::reduce_into(g_in.data(), g_out.data(), c, root, all, op);
  }

  /**
   * @brief Implementation of an MPI reduce for triqs::gfs::gf, triqs::gfs::gf_view or triqs::gfs::gf_const_view types.
   *
   * @details The function reduces input GF (views) from all processes in the given communicator and makes the result
   * available on the root process (`all == false`) or on all processes (`all == true`).
   *
   * It calls triqs::gfs::mpi_reduce_into with the input GF (view) and a default constructed GF object.
   *
   * The content of the returned GF depends on the MPI rank and whether it receives the data or not:
   * - On receiving ranks, it contains the reduced data and the same mesh as the input GF.
   * - On non-receiving ranks, a default constructed GF is returned.
   *
   * @tparam G triqs::gfs::MemoryGf type.
   * @param g GF (view) to be reduced.
   * @param comm `mpi::communicator` object.
   * @param root Rank of the root process.
   * @param all Should all processes receive the result of the reduction.
   * @param op MPI reduction operation.
   * @return A triqs::gfs::gf object with the reduced data.
   */
  template <MemoryGf G> auto mpi_reduce(G const &g, mpi::communicator c = {}, int root = 0, bool all = false, MPI_Op op = MPI_SUM) {
    auto res = typename G::regular_type{};
    mpi_reduce_into(g, res, c, root, all, op);
    return res;
  }

} // namespace triqs::gfs
