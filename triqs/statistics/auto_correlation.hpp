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
#include "./common.hpp"
namespace triqs::stat {

  template <typename T> vec_t<T> _make_vecT(vec_t<T> sum, vec_t<T> const &sum2, std::vector<long> const &count) {
    vec_t<T> r(sum); 
    for (int n = 0; n < sum.size(); ++n) {
      if (count[n] ==0) break;
      sum[n] /= count[n];
      r[n] = sum2[n] / count[n] - sum[n] * sum[n];
    }
    return r;
  }

  /**
   *
   * Analyze the auto-correlation time using 2^n bins technique
   *
   * T : any regular type
   *
   * Given a series of T, it computes the mean and variances of bin of size 2^n.
   *
   */
  // FIXME : add conjugate ?

  // Given any vector, compute the estimagte
  template <typename V> auto tau_estimates(V const &v, long n) { return 0.5 * (v[n] / v[0] * (1ul << n) - 1); }
} // namespace triqs::stat
namespace triqs::stat::accumulators {
  /**
   *
   * Analyze the auto-correlation time using 2^n bins technique
   *
   * T : any regular type
   *
   * Given a series of T, it computes the mean and variances of bin of size 2^n.
   *
   */
  // FIXME : add conjugate ?
  template <typename T> class auto_correlation {

    vec_t<T> _sum, _sum2;        // sum xi, sum xi^2
    vec_t<T> acc;                // accumulator
    std::vector<long> sum_count; // number of elements in sums
    std::vector<long> acc_count; // number of elements in acc

    template <typename F> void h5_serialize(F &&f) {
      f("_sum", _sum), f("_sum2", _sum2), f("acc", acc), f("sum_count", sum_count), f("acc_count", acc_count);
    }
    friend class h5::access;

    // The accumulator is full. Store its value and increase counters.
    void store(int n) {
      acc[n] /= (1ul << n); // bin_size = 2^n
      _sum[n] += acc[n];
      _sum2[n] += acc[n] * acc[n];
      sum_count[n]++;
      acc_count[n] = 0;
      acc[n]       = 0; // FIXME reinit acc. Actually, it should be acc() for gf ... Use a trait ? Generalize ??
    }

    public:
    static std::string hdf5_scheme() { return "auto_correlation"; }

    auto_correlation() = default;

    /**
     * Precondition : n_bins > 0
     */
    auto_correlation(T const &zero, int n_bins)
       : _sum(n_bins, zero), _sum2(n_bins, zero), acc(n_bins, zero), sum_count(n_bins, 0), acc_count(n_bins, 0) {}

    /**
     */
    auto_correlation(int n_bins) : auto_correlation(T{}, n_bins) {}

    /**
     * add a new u in the binned series
     */
    template <typename U> void operator<<(U const &u) {
      static_assert(std::is_constructible_v<T, U>, "Expected something I can construct from");

      // TODO optimization : We do not need this copy : treat the first bin separately
      acc[0] = u;

      /// Advance the counters.
      // go up in n as long as the acc_count becomes full and add the acc in the
      // then go down, and store the acc
      int n = 1;
      for (; n < acc.size(); ++n) {
        acc[n] += acc[n - 1];
        if (++(acc_count[n]) < 2) break;
      }
      --n;
      for (; n >= 0; n--) store(n);
    }

    // Make a new zero 
    T zero() const {
      T r = _sum[0];
      r   = 0;
      return r;
    }

    friend vec_t<T> reduce(auto_correlation const &x) { return _make_vecT<T>(x._sum, x._sum2, x.sum_count); }

    friend vec_t<T> mpi_reduce(auto_correlation const &x, mpi::communicator c) {
      return _make_vecT<T>(mpi_reduce( x._sum,c), mpi_reduce( x._sum2,c), mpi_reduce(x.sum_count, c));
    }

    friend vec_t<T> mpi_all_reduce(auto_correlation const &x, mpi::communicator c) {
      return _make_vecT<T>(mpi_all_reduce( x._sum,c), mpi_all_reduce( x._sum2,c), mpi_all_reduce(x.sum_count, c));
    }
  };

} // namespace triqs::stat::accumulators
