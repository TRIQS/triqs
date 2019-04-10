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
#include "./mpi.hpp"
#include <vector>

namespace mpi {

  // ---------------- broadcast ---------------------

  template <typename T> void mpi_broadcast(std::vector<T> &v, communicator c = {}, int root = 0) {
    size_t s = v.size();
    broadcast(s, c, root);
    if (c.rank() != root) v.resize(s);
    if constexpr (is_basic<T>::value) {
      if (s != 0) MPI_Bcast(v.data(), v.size(), datatype<T>(), root, c.get());
    } else {
      for (auto &x : v) broadcast(x, c, root);
    }
  }
  // ---------------- reduce in place  ---------------------

  template <typename T> void mpi_reduce_in_place(std::vector<T> &a, communicator c = {}, int root = 0, bool all = false, MPI_Op op = MPI_SUM) {
    if (a.size() == 0) return; // mpi behaviour not checked in that case.
    if constexpr (is_basic<T>::value) {
      if (!all)
        MPI_Reduce((c.rank() == root ? MPI_IN_PLACE : a.data()), a.data(), a.size(), datatype<T>(), op, root, c.get());
      else
        MPI_Allreduce(MPI_IN_PLACE, a.data(), a.size(), datatype<T>(), op, c.get());
    } else {
      for (auto &x : a) mpi_reduce_in_place(a, c, root, all);
    }
  }

  // ---------------- reduce   ---------------------

  namespace detail {
    template <typename T, typename Enable = void> struct _regular { using type = T; };
    template <typename T> struct _regular<T, std::void_t<typename T::regular_type>> { using type = typename T::regular_type; };
  } // namespace detail
  template <typename T> using regular_t = typename detail::_regular<std::decay_t<T>>::type;

  template <typename T>
  std::vector<regular_t<T>> mpi_reduce(std::vector<T> const &a, communicator c = {}, int root = 0, bool all = false, MPI_Op op = MPI_SUM) {
    size_t s = a.size();
    if (s == 0) return {}; // nothing to do, and MPI does not like size 0
    if constexpr (is_basic<T>::value) {
      static_assert(std::is_same_v<regular_t<T>, T>, "Internal error");
      std::vector<T> r(s);
      if (!all)
        MPI_Reduce((void *)a.data(), r.data(), s, datatype<T>(), op, root, c.get());
      else
        MPI_Allreduce((void *)a.data(), r.data(), s, datatype<T>(), op, c.get());
      return r;
    } else {
      std::vector<regular_t<T>> r;
      r.reserve(s);
      for (size_t i = 0; i < s; ++i) r.push_back(reduce(a[i], c, root, all, op));
      return r;
    }
  }

  // ---------------- scatter  ---------------------

  // FIXME : not checked for 0 size ?
  template <typename T> std::vector<T> mpi_scatter(std::vector<T> const &a, communicator c = {}, int root = 0) {

    if constexpr (is_basic<T>::value) {
      auto slow_size  = a.size();
      auto sendcounts = std::vector<int>(c.size());
      auto displs     = std::vector<int>(c.size() + 1, 0);
      int recvcount   = slice_length(slow_size, c.size(), c.rank());
      std::vector<T> b(recvcount);

      for (int r = 0; r < c.size(); ++r) {
        sendcounts[r] = slice_length(slow_size, c.size(), r);
        displs[r + 1] = sendcounts[r] + displs[r];
      }

      MPI_Scatterv((void *)a.data(), &sendcounts[0], &displs[0], datatype<T>(), (void *)b.data(), recvcount, datatype<T>(), root, c.get());
      return b;
    }

    // FIXME SHOULD WE KEEP THIS ? It is not a clear API. Probably a static assert would be better
    else {
      int s = a.size();
      std::vector<T> lhs;
      lhs.reserve(s);
      for (auto i = 0; i < s; ++i) lhs.push_back(mpi_scatter(a[i], c, root));
      return lhs;
    }
  }
  // ---------------- gather  ---------------------

  template <typename T> std::vector<T> mpi_gather(std::vector<T> const &a, communicator c = {}, int root = 0, bool all = false) {

    if constexpr (is_basic<T>::value) {
      long size = mpi_reduce(a.size(), c, root, all);
      std::vector<T> b((all || (c.rank() == root) ? size : 0));

      auto recvcounts = std::vector<int>(c.size());
      auto displs     = std::vector<int>(c.size() + 1, 0);
      int sendcount   = a.size();
      auto mpi_ty     = datatype<int>();
      if (!all)
        MPI_Gather(&sendcount, 1, mpi_ty, &recvcounts[0], 1, mpi_ty, root, c.get());
      else
        MPI_Allgather(&sendcount, 1, mpi_ty, &recvcounts[0], 1, mpi_ty, c.get());

      for (int r = 0; r < c.size(); ++r) displs[r + 1] = recvcounts[r] + displs[r];

      if (!all)
        MPI_Gatherv((void *)a.data(), sendcount, datatype<T>(), (void *)b.data(), &recvcounts[0], &displs[0], datatype<T>(), root, c.get());
      else
        MPI_Allgatherv((void *)a.data(), sendcount, datatype<T>(), (void *)b.data(), &recvcounts[0], &displs[0], datatype<T>(), c.get());

      return b;
    }

    // FIXME SHOULD WE KEEP THIS ? It is not a clear API. Probably a static assert would be better
    else {
      int s = a.size();
      std::vector<T> lhs;
      lhs.reserve(s);
      for (auto i = 0; i < s; ++i) lhs.push_back(mpi_gather(a[i], c, root, all));
      return lhs;
    }
  }
} // namespace mpi
