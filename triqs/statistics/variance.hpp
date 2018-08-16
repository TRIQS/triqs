/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011-2017 by O. Parcollet, T. Ayral.
 * Copyright (C) 2018 by Simons Foundation
 *   author : O. Parcollet, H. Strand, P. Dumitrescu
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

  /**
   * value and error. A simple pair with names
   *
   */
  template <typename T> struct value_error { T value, error; };

  template <typename T> std::ostream &operator<<(std::ostream &out, value_error<T> const &ve) { return out << ve.value << " +/- " << ve.error; }

  template <typename T> std::pair<T, T> _make_TT(T sum, T sum2, long count) {
    sum /= count;
    sum2 /= count;
    sum2 -= sum * sum;
    return {std::move(sum), std::move(sum2)};
  }

  // ------- h5  ------

  template <typename T> void h5_write(h5::group g, std::string const &name, value_error<T> const &x) {
    h5_write(g, name, x.value);
    h5_write(g, name + ".error", x.error);
  }

  template <typename T> void h5_read(h5::group g, std::string const &name, value_error<T> &x) {
    h5_read(g, name, x.value);
    h5_read(g, name + ".error", x.error);
  }

  /**
   * Sum and variance
   *
   * T : any regular type
   */
  template <typename T> class variance {

    T _sum, _sum2;
    long _count = 0; // number of elements in sums

    template <typename F> void h5_serialize(F &&f) { f("sum", _sum), f("sum2", _sum2), f("count", _count); }
    friend class h5::access;

    public:
    static std::string hdf5_scheme() { return "accumulator<variance>"; }

    variance() = default;
    variance(T zero) : _sum{std::move(zero)}, _sum2{_sum} {}

    /// add a new u in the binned series
    template <typename U> void operator<<(U const &u) {
      static_assert(std::is_constructible_v<T, U>, "Expected something I can construct from");
      _sum += u;
      _sum2 += u * u;
      _count++;
    }

    // Make a new zero
    T zero() const {
      T r = _sum;
      r   = 0;
      return r;
    }

    friend std::pair<T, T> reduce(variance const &x) { return _make_TT<T>(x._sum, x._sum2, x._count); }

    friend std::pair<T, T> mpi_reduce(variance const &x, mpi::communicator c) {
      return _make_TT(T{mpi_reduce(x._sum, c)}, T{mpi_reduce(x._sum2, c)}, mpi_reduce(x._count, c));
    }

    friend std::pair<T, T> mpi_all_reduce(variance const &x, mpi::communicator c) {
      return _make_TT(T{mpi_all_reduce(x._sum, c)}, T{mpi_all_reduce(x._sum2, c)}, mpi_all_reduce(x._count, c));
    }
  };

} // namespace triqs::stat::accumulators
