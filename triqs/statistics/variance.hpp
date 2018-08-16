/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011-2017 by O. Parcollet, T. Ayral.
 * Copyright (C) 2018 by Simons Foundation
 *   author : O. Parcollet, H. U.R. Strand, P. Dumitrescu
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
    variance(T zero) : _sum{std::move(zero)},
		       _sum2{_sum*_sum} {} // nb, _sum*_sum req. for fermi/bose stat

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

    private:
    inline static std::pair<T, T> _make_TT(T sum, T sum2, long count) {
      sum /= count;
      sum2 /= count;
      sum2 -= sum * sum;
      return {std::move(sum), std::move(sum2)};
    }

    public:
    friend std::pair<T, T> reduce(variance const &x) { return variance::_make_TT(x._sum, x._sum2, x._count); }

    friend std::pair<T, T> mpi_reduce(variance const &x, mpi::communicator c, int root = 0, bool all = false) {
      return _make_TT(T{mpi_reduce(x._sum, c, root, all)}, T{mpi_reduce(x._sum2, c, root, all)}, mpi_reduce(x._count, c));
    }
  };

} // namespace triqs::stat::accumulators
