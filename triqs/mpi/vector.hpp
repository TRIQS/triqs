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

namespace triqs {
namespace mpi {

 // When value_type is a basic type, we can directly call the C API
 template <typename T> struct mpi_impl_std_vector_basic {

  static MPI_Datatype D() { return mpi_datatype<T>::invoke(); }

  // -----------
  static void reduce_in_place(communicator c, std::vector<T> &a, int root) {
   MPI_Reduce((c.rank() == root ? MPI_IN_PLACE : a.data()), a.data(), a.size(), D(), MPI_SUM, root, c.get());
  }

  static void allreduce_in_place(communicator c, std::vector<T> &a, int root) {
   MPI_Allreduce(MPI_IN_PLACE, a.data(), a.size(), D(), MPI_SUM, root, c.get());
  }

  // -----------
  static void broadcast(communicator c, std::vector<T> &a, int root) { MPI_Bcast(a.data(), a.size(), D(), root, c.get()); }

  // -----------
  static std::vector<T> invoke(tag::reduce, communicator c, T const &a, int root) {
   std::vector<T> b(a.size());
   MPI_Reduce(a.data(), b.data(), a.size(), D(), MPI_SUM, root, c.get());
   return b;
  }

  // -----------
  static std::vector<T> invoke(tag::allreduce, communicator c, std::vector<T> const &a, int root) {
   std::vector<T> b(a.size());
   MPI_Allreduce(a.data(), b.data(), a.size(), D(), MPI_SUM, root, c.get());
   return b;
  }

  // -----------
  static std::vector<T> invoke(tag::scatter, communicator c, std::vector<T> const &a, int root) {

   auto slow_size = a.size();
   auto sendcounts = std::vector<int>(c.size());
   auto displs = std::vector<int>(c.size() + 1, 0);
   int recvcount = slice_length(slow_size - 1, c, c.rank());
   std::vector<T> b(recvcount);

   for (int r = 0; r < c.size(); ++r) {
    sendcounts[r] = slice_length(slow_size - 1, c, r);
    displs[r + 1] = sendcounts[r] + displs[r];
   }

   MPI_Scatterv((void *)a.data(), &sendcounts[0], &displs[0], D(), (void *)b.data(), recvcount, D(), root, c.get());
   return b;
  }

  // -----------
  static std::vector<T> invoke(tag::gather, communicator c, std::vector<T> const &a, int root) {
   long size = reduce(a.size(), c, root);
   std::vector<T> b(size);

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

  static std::vector<T> invoke(tag::allgather, communicator c, std::vector<T> const &a, int root) {
   long size = reduce(a.size(), c, root);
   std::vector<T> b(size);

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

 template <typename T>
 struct mpi_impl<std::vector<T>, std14::enable_if_t<std::is_arithmetic<T>::value ||
                                                    triqs::is_complex<T>::value>> : mpi_impl_std_vector_basic<T> {};

 // vector <T> for T non basic
}
} // namespace

