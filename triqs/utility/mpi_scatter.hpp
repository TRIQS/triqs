/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2019 The Simons Foundation
 *   author : N. Wentzell
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
#include "./itertools.hpp"
#include "./../mpi/base.hpp"

namespace triqs::utility {

  /**
   * A forward iterator for a range scattered over multiple mpi threads
   */
  template <typename Iter, typename Value = typename std::iterator_traits<Iter>::value_type>
  struct mpi_scatter_iter : triqs::utility::iterator_facade<mpi_scatter_iter<Iter>, Value> {

    Iter it;
    triqs::mpi::communicator comm;

    mpi_scatter_iter(Iter it, triqs::mpi::communicator comm) : it(it), comm(comm) {}

    void increment() { std::advance(it, comm.size()); }
    bool equal(mpi_scatter_iter const &other) const { return (it == other.it); }
    decltype(auto) dereference() const { return *it; }
  };

  namespace details {

    template <typename T> struct mpi_scattered {
      T x;
      triqs::mpi::communicator comm;

      using iterator       = mpi_scatter_iter<std::decay_t<decltype(std::begin(x))>>;
      using const_iterator = mpi_scatter_iter<std::decay_t<decltype(std::cbegin(x))>>;

      auto begin() { return iterator{std::next(std::begin(x), comm.rank()), comm}; }
      auto cbegin() const { return const_iterator{std::next(std::cbegin(x), comm.rank()), comm}; }
      auto begin() const { return cbegin(); }

      auto end() {
        auto total_size = std::distance(std::cbegin(x), std::cend(x));
        if (comm.rank() >= total_size) return begin();
        auto rank_size = 1 + (total_size - comm.rank() - 1) / comm.size();
        return std::next(begin(), rank_size);
      }
      auto cend() const {
        auto total_size = std::distance(std::cbegin(x), std::cend(x));
        if (comm.rank() >= total_size) return cbegin();
        auto rank_size = 1 + (total_size - comm.rank() - 1) / comm.size();
        return std::next(cbegin(), rank_size);
      }
      auto end() const { return cend(); }
    };
  } // namespace details

  /**
   * Function to scatters a range uniformly over all MPI threads.
   *
   * @tparam T The type of the range
   *
   * @param range The range to scatter
   * @param comm The mpi communicator
   */
  template <typename T> auto mpi_scatter(T &&range, triqs::mpi::communicator comm = {}) {
    return details::mpi_scattered<T>{std::forward<T>(range), comm};
  }
} // namespace triqs::mpi
