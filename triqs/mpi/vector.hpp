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
#include <vector>

namespace triqs {
namespace mpi {

 // When value_type is a basic type, we can directly call the C API
 template <typename T> struct mpi_impl_std_vector_basic {

  using V = std::vector<T>;

  static MPI_Datatype D() { return mpi_datatype<T>::invoke(); }

  // -----------
  static void reduce_in_place(communicator c, V &a, int root) {
   MPI_Reduce((c.rank() == root ? MPI_IN_PLACE : a.data()), a.data(), a.size(), D(), MPI_SUM, root, c.get());
  }

  static void all_reduce_in_place(communicator c, V &a, int root) {
   MPI_Allreduce(MPI_IN_PLACE, a.data(), a.size(), D(), MPI_SUM, root, c.get());
  }

  // -----------
  static void broadcast(communicator c, V &a, int root) {
   size_t s = a.size();
   MPI_Bcast(&s, 1, mpi_datatype<size_t>::invoke(), root, c.get());
   if (c.rank() != root) a.resize(s);
   MPI_Bcast(a.data(), a.size(), D(), root, c.get());
  }

  // ------------
  template <typename Tag> static void invoke2(V &lhs, Tag, communicator c, V const &a, int root) {
   lhs = invoke(Tag(), c, a, root);
  }

  // -----------
  static V invoke(tag::reduce, communicator c, V const &a, int root) {
   V b(a.size());
   MPI_Reduce(a.data(), b.data(), a.size(), D(), MPI_SUM, root, c.get());
   return b;
  }

  // -----------
  static V invoke(tag::all_reduce, communicator c, V const &a, int root) {
   V b(a.size());
   MPI_Allreduce(a.data(), b.data(), a.size(), D(), MPI_SUM, root, c.get());
   return b;
  }

  // -----------
  static V invoke(tag::scatter, communicator c, V const &a, int root) {

   auto slow_size = a.size();
   auto sendcounts = std::vector<int>(c.size());
   auto displs = std::vector<int>(c.size() + 1, 0);
   int recvcount = slice_length(slow_size - 1, c.size(), c.rank());
   V b(recvcount);

   for (int r = 0; r < c.size(); ++r) {
    sendcounts[r] = slice_length(slow_size - 1, c.size(), r);
    displs[r + 1] = sendcounts[r] + displs[r];
   }

   MPI_Scatterv((void *)a.data(), &sendcounts[0], &displs[0], D(), (void *)b.data(), recvcount, D(), root, c.get());
   return b;
  }

  // -----------
  static V invoke(tag::gather, communicator c, V const &a, int root) {
   long size = reduce(a.size(), c, root);
   V b(size);

   auto recvcounts = std::vector<int>(c.size());
   auto displs = std::vector<int>(c.size() + 1, 0);
   int sendcount = a.size();
   auto mpi_ty = mpi::mpi_datatype<int>::invoke();
   MPI_Gather(&sendcount, 1, mpi_ty, &recvcounts[0], 1, mpi_ty, root, c.get());
   for (int r = 0; r < c.size(); ++r) displs[r + 1] = recvcounts[r] + displs[r];

   MPI_Gatherv((void *)a.data(), sendcount, D(), (void *)b.data(), &recvcounts[0], &displs[0], D(), root, c.get());
   return b;
  }

  // -----------

  static V invoke(tag::allgather, communicator c, V const &a, int root) {
   long size = all_reduce(a.size(), c, root);
   V b(size);

   auto recvcounts = std::vector<int>(c.size());
   auto displs = std::vector<int>(c.size() + 1, 0);
   int sendcount = a.size();
   auto mpi_ty = mpi::mpi_datatype<int>::invoke();
   MPI_Allgather(&sendcount, 1, mpi_ty, &recvcounts[0], 1, mpi_ty, c.get());
   for (int r = 0; r < c.size(); ++r) displs[r + 1] = recvcounts[r] + displs[r];

   MPI_Allgatherv((void *)a.data(), sendcount, D(), (void *)b.data(), &recvcounts[0], &displs[0], D(), c.get());
   return b;
  }
 };

 //-------------------------------------------------------------------------
 // vector generic
 //-------------------------------------------------------------------------
 template <typename T> struct mpi_impl_std_vector_generic {

  using V = std::vector<T>;

  static void reduce_in_place(communicator c, V &v, int root) {
   for (auto &x : v) mpi::reduce_in_place(c, x, root);
  }

  static void all_reduce_in_place(communicator c, V &v, int root) {
   for (auto &x : v) mpi::all_reduce_in_place(c, x, root);
  }

  static void broadcast(communicator c, V &v, int root) {
   for (auto &x : v) mpi::broadcast(c, x, root);
  }

  template <typename Tag> static mpi_lazy<Tag, V> invoke(Tag, communicator c, V const &g, int root) {
   return {g, root, c};
  }

  template <typename Tag> static void invoke2(V &lhs, Tag, communicator c, V const &a, int root) {
   int s = a.size();
   lhs.resize(s);
   for (auto i = 0; i < s; ++i) mpi::_invoke2(lhs[i], Tag(), c, a[i], root);
  }

  template <typename Tag> static void complete_operation(V &lhs, mpi_lazy<Tag, V> laz) {
   invoke2(lhs, Tag(), laz.c, laz.ref, laz.root);
  }
 };

 template <typename T>
 struct mpi_impl<std::vector<T>, std14::enable_if_t<std::is_arithmetic<T>::value ||
                                                    triqs::is_complex<T>::value>> : mpi_impl_std_vector_basic<T> {};

 template <typename T>
 struct mpi_impl<std::vector<T>, std14::enable_if_t<!(std::is_arithmetic<T>::value ||
                                                      triqs::is_complex<T>::value)>> : mpi_impl_std_vector_generic<T> {};
}
} // namespace

