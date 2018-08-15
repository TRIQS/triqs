/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011-2017 by O. Parcollet, T. Ayral.
 * Copyright (C) 2018 by Simons Foundation
 *   author : O. Parcollet
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
#include <triqs/utility/itertools.hpp>
#include <type_traits>
#include <vector>
#include "./common.hpp"

namespace triqs::stat::accumulators {

  template <typename T> T _make_T(T sum, long count) {
    sum /= count;
    return std::move(sum);
  }

  /**
   * Simple sum of T;
   *
   * T : any regular type
   */
  template <typename T> class average {

    T _sum;
    long _count = 0; // number of elements in sums

    template <typename F> void h5_serialize(F &&f) {
      f("sum", _sum);
      f("count", _count, h5::as_attribute);
    }
    friend class h5::access;

    public:
    static std::string hdf5_scheme() { return "accumulator<average>"; }

    average() = default;

    average(T zero) : _sum{std::move(zero)} {}

    /// add a new u in the binned series
    template <typename U> void operator<<(U const &u) {
      static_assert(std::is_constructible_v<T, U>, "Expected something I can construct from");
      _sum += u;
      _count++;
    }

    friend T reduce(average const &x) { return _make_T<T>(x._sum, x._count); }

    friend T mpi_reduce(average const &x, mpi::communicator c) { return _make_T(T{mpi_reduce(x._sum, c)}, mpi_reduce(x._count, c)); }

    friend T mpi_all_reduce(average const &x, mpi::communicator c) { return _make_T<T>(T{mpi_all_reduce(x._sum, c)}, mpi_all_reduce(x._count, c)); }
  };
} // namespace triqs::stat::accumulators
