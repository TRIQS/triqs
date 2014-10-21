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
  /*static void all_reduce_in_place(communicator c, G &g, int root) {
   triqs::mpi::all_reduce_in_place(c, g.data(), root);
   triqs::mpi::all_reduce_in_place(c, g.singularity(), root);
  }
*/

  //---------
  static void broadcast(communicator c, G &g, int root) {
   triqs::mpi::broadcast(c, g.data(), root);
   triqs::mpi::broadcast(c, g.singularity(), root);
  }

  //---------
  template <typename Tag> static mpi_lazy<Tag, G> invoke(Tag, communicator c, G const &g, int root) {
   return {g, root, c};
  }

  //---- reduce ----
  static G &complete_operation(G &target, mpi_lazy<tag::reduce, G> laz) {
   target._data = mpi::reduce(laz.ref.data(), laz.c, laz.root);
   target._singularity = mpi::reduce(laz.ref.singularity(), laz.c, laz.root);
   return target;
  }

  //---- all_reduce ----
  static G &complete_operation(G &target, mpi_lazy<tag::all_reduce, G> laz) {
   target._data = mpi::all_reduce(laz.ref.data(), laz.c, laz.root);
   target._singularity = mpi::all_reduce(laz.ref.singularity(), laz.c, laz.root);
   return target;
  }

  //---- scatter ----
  static G &complete_operation(G &target, mpi_lazy<tag::scatter, G> laz) {
   target._mesh = mpi_scatter(laz.ref.mesh(), laz.c, laz.root); 
   target._data = mpi::scatter(laz.ref.data(), laz.c, laz.root); // HERE ADD OPTION FOR CHUNCK
   target._singularity = laz.ref.singularity();
   //mpi::broadcast(target._singularity, laz.c, laz.root);
   return target;
  }
 
  //---- gather ----
  static G &complete_operation(G &target, mpi_lazy<tag::gather, G> laz) {
   target._mesh = mpi_gather(laz.ref.mesh(), laz.c, laz.root); 
   target._data = mpi::gather(laz.ref.data(), laz.c, laz.root); // HERE ADD OPTION FOR CHUNCK
   // do nothing for singularity
   return target;
  }

  //---- allgather ----
  static G &complete_operation(G &target, mpi_lazy<tag::allgather, G> laz) {
   target._data = mpi::allgather(laz.ref.data(), laz.c, laz.root); // HERE ADD OPTION FOR CHUNCK
   // do nothing for singularity
   return target;
  }

 };

} // mpi namespace 
} // namespace triqs
