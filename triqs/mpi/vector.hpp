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

 // ---------------- broadcast ---------------------

 template <typename T> void mpi_broadcast(std::vector<T> &a, communicator c={}, int root=0) {
  mpi_broadcast(a, c, root, is_basic<T>{});
 }

 template <typename T> void mpi_broadcast(std::vector<T> &a, communicator c, int root, std::true_type) {
  size_t s = a.size();
  mpi_broadcast(s, c, root);
  if (c.rank() != root) a.resize(s);
  MPI_Bcast(a.data(), a.size(), mpi_datatype<T>(), root, c.get());
 }

 template <typename T> void mpi_broadcast(std::vector<T> &v, communicator c, int root, std::false_type) {
  size_t s = mpi_broadcast(v.size());
  if (c.rank() != root) v.resize(s);
  for (auto &x : v) mpi_broadcast(x, c, root);
 }

 // ---------------- reduce in place  ---------------------

 template <typename T> void mpi_reduce_in_place(std::vector<T> &a, communicator c={}, int root=0) {
  mpi_reduce_in_place(a, c, root, is_basic<T>{});
 }

 template <typename T> void mpi_reduce_in_place(std::vector<T> &a, communicator c, int root, bool all, std::true_type) {
  if (!all)
   MPI_Reduce((c.rank() == root ? MPI_IN_PLACE : a.data()), a.data(), a.size(), mpi_datatype<T>(), MPI_SUM, root, c.get());
  else
   MPI_Allreduce(MPI_IN_PLACE, a.data(), a.size(), mpi_datatype<T>(), MPI_SUM, c.get());
 }

 template <typename T> void mpi_reduce_in_place(std::vector<T> &a, communicator c, int root, bool all, std::false_type) {
  for (auto &x : a) mpi_reduce_in_place(a, c, root, all);
 }

 // ---------------- reduce   ---------------------

 template <typename T> std::vector<T> mpi_reduce(std::vector<T> const &a, communicator c={}, int root=0, bool all=false) {
  return mpi_reduce(a, c, root, all, is_basic<T>{});
 }

 template <typename T> std::vector<T> mpi_reduce(std::vector<T> const &a, communicator c, int root, bool all, std::true_type) {
  std::vector<T> b(a.size());
  if (!all)
   MPI_Reduce(a.data(), b.data(), a.size(), mpi_datatype<T>(), MPI_SUM, root, c.get());
  else
   MPI_Allreduce(a.data(), b.data(), a.size(), mpi_datatype<T>(), MPI_SUM, root, c.get());
  return b;
 }

 template <typename T> std::vector<T> mpi_reduce(std::vector<T> const &a, communicator c, int root, bool all, std::false_type) {
  int s = a.size();
  std::vector<T> lhs;
  lhs.reserve(s);
  for (auto i = 0; i < s; ++i) lhs.push_back(mpi_reduce(a[i], c, root, all));
  return lhs;
 }

 // ---------------- scatter  ---------------------

 template <typename T> std::vector<T> mpi_scatter(std::vector<T> const &a, communicator c={}, int root=0) {
  return mpi_scatter(a, c, root, is_basic<T>{});
 }

 template <typename T> std::vector<T> mpi_scatter(std::vector<T> const &a, communicator c, int root, std::true_type) {
  auto slow_size = a.size();
  auto sendcounts = std::vector<int>(c.size());
  auto displs = std::vector<int>(c.size() + 1, 0);
  int recvcount = slice_length(slow_size - 1, c.size(), c.rank());
  std::vector<T> b(recvcount);

  for (int r = 0; r < c.size(); ++r) {
   sendcounts[r] = slice_length(slow_size - 1, c.size(), r);
   displs[r + 1] = sendcounts[r] + displs[r];
  }

  MPI_Scatterv((void *)a.data(), &sendcounts[0], &displs[0], mpi_datatype<T>(), (void *)b.data(), recvcount, mpi_datatype<T>(),
               root, c.get());
  return b;
 }

 template <typename T> std::vector<T> mpi_scatter(std::vector<T> const &a, communicator c, int root, std::false_type) {
  int s = a.size();
  std::vector<T> lhs;
  lhs.reserve(s);
  for (auto i = 0; i < s; ++i) lhs.push_back(mpi_scatter(a[i], c, root));
  return lhs;
 }

 // ---------------- gather  ---------------------

 template <typename T> std::vector<T> mpi_gather(std::vector<T> const &a, communicator c = {}, int root = 0, bool all = false) {
  return mpi_gather(a, c, root, all, is_basic<T>{});
 }

 template <typename T> std::vector<T> mpi_gather(std::vector<T> const &a, communicator c, int root, bool all, std::true_type) {
  long size = mpi_reduce(a.size(), c, root, all);
  std::vector<T> b((all || (c.rank() == root) ? size : 0));

  auto recvcounts = std::vector<int>(c.size());
  auto displs = std::vector<int>(c.size() + 1, 0);
  int sendcount = a.size();
  auto mpi_ty = mpi::mpi_datatype<int>();
  if (!all)
   MPI_Gather(&sendcount, 1, mpi_ty, &recvcounts[0], 1, mpi_ty, root, c.get());
  else
   MPI_Allgather(&sendcount, 1, mpi_ty, &recvcounts[0], 1, mpi_ty, c.get());

  for (int r = 0; r < c.size(); ++r) displs[r + 1] = recvcounts[r] + displs[r];

  if (!all)
   MPI_Gatherv((void *)a.data(), sendcount, mpi_datatype<T>(), (void *)b.data(), &recvcounts[0], &displs[0], mpi_datatype<T>(),
               root, c.get());
  else
   MPI_Allgatherv((void *)a.data(), sendcount, mpi_datatype<T>(), (void *)b.data(), &recvcounts[0], &displs[0], mpi_datatype<T>(),
                  c.get());

  return b;
 }

 template <typename T> std::vector<T> mpi_gather(std::vector<T> const &a, communicator c, int root, bool all, std::false_type) {
  int s = a.size();
  std::vector<T> lhs;
  lhs.reserve(s);
  for (auto i = 0; i < s; ++i) lhs.push_back(mpi_gather(a[i], c, root, all));
  return lhs;
 }

}
} // namespace

