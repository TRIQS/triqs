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

#pragma once

namespace triqs::gfs {

  /**
    * Initiate (lazy) MPI Bcast
    *
    * When the returned object is used at the RHS of operator = or in a constructor of a gf,
    * the MPI Bcast operation is performed.
    *
    * @group MPI
    * @param g The Green function
    * @param c The MPI communicator (default is world)
    * @param root The root of the broadcast communication in the MPI sense.
    * @return Returns a lazy object describing the object and the MPI operation to be performed.
    *
    */
  // mako ${mpidoc("Bcast")}
  template <typename V, typename T, int Arity> void mpi_broadcast(block_gf<V, T, Arity> &g, mpi::communicator c = {}, int root = 0) {
    // Shall we bcast mesh ?
    mpi::broadcast(g.data(), c, root);
  }

  /**
    * Initiate (lazy) MPI Bcast
    *
    * When the returned object is used at the RHS of operator = or in a constructor of a gf,
    * the MPI Bcast operation is performed.
    *
    * @group MPI
    * @param g The Green function
    * @param c The MPI communicator (default is world)
    * @param root The root of the broadcast communication in the MPI sense.
    * @return Returns a lazy object describing the object and the MPI operation to be performed.
    *
    */
  // mako ${mpidoc("Bcast")}
  template <typename V, typename T, int Arity, bool IsConst>
  void mpi_broadcast(block_gf_view<V, T, Arity, IsConst> &g, mpi::communicator c = {}, int root = 0) {
    // Shall we bcast mesh ?
    mpi::broadcast(g.data(), c, root);
  }

  /**
    * Initiate (lazy) MPI Reduce
    *
    * When the returned object is used at the RHS of operator = or in a constructor of a gf,
    * the MPI Reduce operation is performed.
    *
    * @group MPI
    * @param g The Green function
    * @param c The MPI communicator (default is world)
    * @param root The root of the broadcast communication in the MPI sense.
    * @return Returns a lazy object describing the object and the MPI operation to be performed.
    */
  template <typename V, typename T, int Arity>
  mpi::lazy<mpi::tag::reduce, block_gf_const_view<V, T, Arity>> mpi_reduce(block_gf<V, T, Arity> const &a, mpi::communicator c = {}, int root = 0,
                                                                           bool all = false, MPI_Op op = MPI_SUM) {
    return {a(), c, root, all, op};
  }

  /**
    * Initiate (lazy) MPI Reduce
    *
    * When the returned object is used at the RHS of operator = or in a constructor of a gf,
    * the MPI Reduce operation is performed.
    *
    * @group MPI
    * @param g The Green function
    * @param c The MPI communicator (default is world)
    * @param root The root of the broadcast communication in the MPI sense.
    * @return Returns a lazy object describing the object and the MPI operation to be performed.
    */
  template <typename V, typename T, int Arity, bool IsConst>
  mpi::lazy<mpi::tag::reduce, block_gf_const_view<V, T, Arity>> mpi_reduce(block_gf_view<V, T, Arity, IsConst> const &a, mpi::communicator c = {},
                                                                           int root = 0, bool all = false, MPI_Op op = MPI_SUM) {
    return {a, c, root, all, op};
  }

} // namespace triqs::gfs
