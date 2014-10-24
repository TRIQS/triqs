/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2014 by O. Parcollet
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
#include "./base.hpp"
#include "./arrays.hpp"
#include "./vector.hpp"

namespace triqs {
namespace mpi {

 //--------------------------------------------------------------------------------------------------------

 // When value_type is a basic type, we can directly call the C API
 template <typename G> struct mpi_impl_triqs_gfs {

  //---------
  static void reduce_in_place(communicator c, G &g, int root) {
   triqs::mpi::reduce_in_place(c, g.data(), root);
   triqs::mpi::reduce_in_place(c, g.singularity(), root);
  }

  //---------
  static void all_reduce_in_place(communicator c, G &g, int root) {
   triqs::mpi::all_reduce_in_place(c, g.data(), root);
   triqs::mpi::all_reduce_in_place(c, g.singularity(), root);
  }

  //---------
  static void broadcast(communicator c, G &g, int root) {
   // Shall we bcast mesh ?
   triqs::mpi::broadcast(c, g.data(), root);
   triqs::mpi::broadcast(c, g.singularity(), root);
  }

  //---------
  template <typename Tag> static mpi_lazy<Tag, G> invoke(Tag, communicator c, G const &g, int root) {
   return {g, root, c};
  }

  //---------
  template <typename Tag> static void complete_operation(G &lhs, mpi_lazy<Tag, G> laz) {
   invoke2(lhs, Tag(), laz.c, laz.ref, laz.root);
  }

  //---- reduce ----
  static void invoke2(G &lhs, tag::reduce, communicator c, G const &g, int root) {
   lhs._mesh = g._mesh;
   mpi::_invoke2(lhs._data, tag::reduce(), c, g.data(), root);
   mpi::_invoke2(lhs._singularity, tag::reduce(), c, g.singularity(), root);
  }

  //---- all_reduce ----
  static void invoke2(G &lhs, tag::all_reduce, communicator c, G const &g, int root) {
   lhs._mesh = g._mesh;
   mpi::_invoke2(lhs._data, tag::all_reduce(), c, g.data(), root);
   mpi::_invoke2(lhs._singularity, tag::all_reduce(), c, g.singularity(), root);
  }

  //---- scatter ----
  static void invoke2(G &lhs, tag::scatter, communicator c, G const &g, int root) {
   lhs._mesh = mpi_scatter(g.mesh(), c, root);
   mpi::_invoke2(lhs._data, tag::scatter(), c, g.data(), root);
   if (c.rank() == root) lhs._singularity = g.singularity();
   mpi::broadcast(lhs._singularity, c, root);
  }

  //---- gather ----
  static void invoke2(G &lhs, tag::gather, communicator c, G const &g, int root) {
   lhs._mesh = mpi_gather(g.mesh(), c, root);
   mpi::_invoke2(lhs._data, tag::gather(), c, g.data(), root);
   if (c.rank() == root) lhs._singularity = g.singularity();
  }

  //---- allgather ----
  static void invoke2(G &lhs, tag::allgather, communicator c, G const &g, int root) {
   lhs._mesh = mpi_gather(g.mesh(), c, root);
   mpi::_invoke2(lhs._data, tag::allgather(), c, g.data(), root);
   lhs._singularity = g.singularity();
  }
 };

 // ---------------------------------------------------------------------------------------
 //  Do nothing for nothing...
 // ---------------------------------------------------------------------------------------
 template <> struct mpi_impl<gfs::nothing> {
  template <typename Tag> static void invoke2(gfs::nothing &lhs, Tag, communicator c, gfs::nothing const &a, int root) {}
  template <typename Tag> static gfs::nothing invoke(Tag, communicator c, gfs::nothing const &a, int root) { return gfs::nothing(); }
  static void reduce_in_place(communicator c, gfs::nothing &a, int root) {}
  static void broadcast(communicator c, gfs::nothing &a, int root) {}
 };

} // mpi namespace
} // namespace triqs
