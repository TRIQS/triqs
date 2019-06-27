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
#include <functional>
#include <numeric>
#include <type_traits>
#include <triqs/arrays.hpp>
#include <triqs/arrays/math_functions.hpp>
#include <mpi/vector.hpp>

namespace triqs::stat {

  using triqs::arrays::conj_r;

  namespace details {

    // Represent the jacknifed series.
    // @tparam V A vector like object, with size() and []
    //
    template <typename V> struct jackknifed_t {
      V const &original_series;
      using T = std::decay_t<decltype(original_series[0])>;
      T sum;
      long n_tot;

      // On each node independently
      jackknifed_t(V const &bins) : original_series(bins) {
        // ENSURE(bins.size()>0)
        n_tot = original_series.size();
        // compute the sum of B into a new T.
        sum = original_series[0];
        for (long i = 1; i < original_series.size(); ++i) sum += original_series[i];
      }

      // Averages are taken over the whole communicator
      jackknifed_t(V const &bins, mpi::communicator c) : jackknifed_t(bins) {
        // first sum on each node (delegated constructor) and then reduce
        n_tot = mpi::all_reduce(n_tot, c);
        sum   = mpi::all_reduce(sum, c);
      }

      // average of the series (over the communicator given at construction)
      auto average() const { return sum / n_tot; }

      // Call. NB : if T is an array, returns an expression template to retard the evaluation
      auto operator[](long i) const { return (sum - original_series[i]) / (n_tot - 1); }
    };

    //----------------------------------------------------------------

    // implementation of jacknife.
    // mpi iif the c pointer is not null. If null the computation is on this node only.
    template <typename F, typename... Jacknifed> auto jackknife_impl(mpi::communicator *c, F &&f, Jacknifed const &... ja) {

      // N is the size of the series, it should be all equal and !=0
      std::array<long, sizeof...(Jacknifed)> dims{long(ja.original_series.size())...};
      long N = dims[0];
      if (N == 0) TRIQS_RUNTIME_ERROR << "No data !";
      if (not((ja.original_series.size() == N) and ...)) TRIQS_RUNTIME_ERROR << " Jackknife : size mismatch";

      using std::real;
      using std::sqrt;
      using triqs::arrays::abs2;

      // WARNING NB : In this function, we must be very careful never to store an expression template using f.
      // f is provided by the user. It will be provided with ja[i] which
      // might be an expression template itself (hence a temporary).
      // The user is likely be careless with rvalue references
      // e.g. f = [](auto const & x, auto const y){ return x/y;};
      // in such cases, the ja[0] will be dangling unless we use the expression template
      // containing f immediately.
      // Cf below, M and Q computation.
      // Check by ASAN sanitizer on tests
      auto M = make_regular(f(ja[0]...));

      for (long i = 1; i < N; ++i) {
        M += (f(ja[i]...) - M) / (i + 1); // cf NB
      }

      // Now the reduction over the nodes.
      long Ntot = N;
      if (c) { // MPI version
        Ntot = mpi::reduce(N, *c);

        M *= double(N) / Ntot; // remove the 1/N
        M = mpi::all_reduce(M, *c);
      }

      // Compute the variance
      auto Q = make_regular(real(M));
      Q      = 0;
      for (long i = 0; i < N; ++i) {
        Q += abs2(f(ja[i]...) - M); // Cf NB
      }

      // reduce Q
      if (c) Q = mpi::all_reduce(Q, *c);

      auto f_on_averages = make_regular(f(ja.average()...));
      auto std_err       = make_regular(std::sqrt((Ntot - 1) / double(Ntot)) * sqrt(Q));
      auto average       = make_regular(N * (f_on_averages - M) + M);
      return std::make_tuple(average, std_err, M, f_on_averages);
    }

  } // namespace details

  // ------------------------------------------------------------------------------

  /// Directly pass data-series in vector like objects
  /// @head Calculate the value and error of a general function $f$ of the average of sampled observables $f\left(\langle \mathbf{a} \rangle\right)$, using jackknife resampling.
  /// @tparam F return type of function :param:`f` which acts on data
  /// @tparam A vector-like object, defining size() and []
  /// @param a one or multiple series with data: $\mathbf{a} = \{a_1, a_2, a_3, \ldots\}$
  ///   Pre-condition: if more than one series is passed, the series have to be equal in size
  /// @param f a function which acts on the $i^\mathrm{th}$ elements of the series in :param:`a`:
  /// $$\left(a_1[i], a_2[i],a_3[i],\ldots\right) \to f\left(a_1[i],a_2[i],a_3[i],\ldots\right)$$
  /// @return std::tuple with four statistical estimators $\left(f_\mathrm{J}^{*}, \Delta{f}_\mathrm{J}, f_\mathrm{J}, f_\mathrm{direct}\right)$, defined below.
  ///
  /// Jackknife resampling takes $N$ data points $\mathbf{a}[i]$ and creates $N$ samples ("jackknifed data"), which we denote $\tilde{\mathbf{a}}[i]$. We calculate three statistical estimators for $f\left(\langle \mathbf{a} \rangle\right)$:
  ///   * The function $f$ applied to observed mean of the data
  ///     $$f_\mathrm{direct} = f\left(\bar{\mathbf{a}}\right),\quad \bar{\mathbf{a}} = \frac{1}{N}\sum_{i=0}^{N}\mathbf{a}[i]$$
  ///   * The jacknife estimate defined as
  ///     $$f_\mathrm{J} = \frac{1}{N}\sum_{i=0}^N f(\tilde{\mathbf{a}}[i])$$
  ///   * The jacknife estimate, with bias correction to remove $O(1/N)$ effects
  ///     $$f_\mathrm{J}^{*} = N f_\mathrm{direct} - (N - 1) f_\mathrm{J}$$
  /// Additionally, an estimate in the errror of $f\left(\langle \mathbf{a} \rangle\right)$ is given by the jacknife as
  ///     $$\Delta{f}_J = \sqrt{N-1} \cdot \sigma_f$$
  /// where $\sigma_f$ is the standard deviation of $\left\{f(\tilde{\mathbf{a}}[0]), f(\tilde{\mathbf{a}}[1]), \ldots, f(\tilde{\mathbf{a}}[N])\right\}$.
  /// @brief Calculate mean and error of derived data using jackknife resampling
  template <typename F, typename... A> auto jackknife(F &&f, A const &... a) {
    static_assert(not std::is_same_v<std::decay_t<F>, mpi::communicator>,
                  "I see that you pass a mpi:::communicator, you probably want to use jackknife_mpi");
    return details::jackknife_impl(nullptr, std::forward<F>(f), details::jackknifed_t{a}...);
  }

  /// Pass :ref:`accumulators <triqs__stat__accumulator>`, where the jacknife acts on the :ref:`linear binned data <accumulator_linear_bins>`
  /// @tparam T type of data stored in the accumulators
  template <typename F, typename... T> auto jackknife(F &&f, accumulator<T> const &... a) {
    static_assert(not std::is_same_v<std::decay_t<F>, mpi::communicator>,
                  "I see that you pass a mpi:::communicator, you probably want to use jackknife_mpi");
    return jackknife(std::forward<F>(f), a.linear_bins()...);
  }

  // ------------------------------------------------------------------------------

  /// Directly pass data-series in vector like objects
  /// @head Calculate the value and error of a general function $f$ of the average of sampled observables $f\left(\langle \mathbf{a} \rangle\right)$, using jackknife resampling.
  /// @tail The calculation is performed over the nodes; the answers which are then reduced (not all-reduced) to the node 0.
  /// @tparam F return type of function :param:`f` which acts on data
  /// @tparam A vector-like object, defining size() and []
  /// @param c TRIQS MPI communicator
  /// @param a one or multiple series with data: $\mathbf{a} = \{a_1, a_2, a_3, \ldots\}$
  ///   Pre-condition: if more than one series is passed, the series have to be equal in size
  /// @param f a function which acts on the $i^\mathrm{th}$ elements of the series in :param:`a`:
  /// $$\left(a_1[i], a_2[i],a_3[i],\ldots\right) \to f\left(a_1[i],a_2[i],a_3[i],\ldots\right)$$
  /// @return std::tuple with four statistical estimators $\left(f_\mathrm{J}^{*}, \Delta{f}_\mathrm{J}, f_\mathrm{J}, f_\mathrm{direct}\right)$, defined below. The MPI reduction occurs *only* to node 0.
  ///
  /// Jackknife resampling takes $N$ data points $\mathbf{a}[i]$ and creates $N$ samples ("jackknifed data"), which we denote $\tilde{\mathbf{a}}[i]$. We calculate three statistical estimators for $f\left(\langle \mathbf{a} \rangle\right)$:
  ///   * The function $f$ applied to observed mean of the data
  ///     $$f_\mathrm{direct} = f\left(\bar{\mathbf{a}}\right),\quad \bar{\mathbf{a}} = \frac{1}{N}\sum_{i=0}^{N}\mathbf{a}[i]$$
  ///   * The jacknife estimate defined as
  ///     $$f_\mathrm{J} = \frac{1}{N}\sum_{i=0}^N f(\tilde{\mathbf{a}}[i])$$
  ///   * The jacknife estimate, with bias correction to remove $O(1/N)$ effects
  ///     $$f_\mathrm{J}^{*} = N f_\mathrm{direct} - (N - 1) f_\mathrm{J}$$
  /// Additionally, an estimate in the errror of $f\left(\langle \mathbf{a} \rangle\right)$ is given by the jacknife as
  ///     $$\Delta{f}_J = \sqrt{N-1} \cdot \sigma_f$$
  /// where $\sigma_f$ is the standard deviation of $\left\{f(\tilde{\mathbf{a}}[0]), f(\tilde{\mathbf{a}}[1]), \ldots, f(\tilde{\mathbf{a}}[N])\right\}$.
  /// @brief Calculate mean and error of derived data using jackknife resampling (MPI Version)
  template <typename F, typename... A> auto jackknife_mpi(mpi::communicator c, F &&f, A const &... a) {
    return details::jackknife_impl(&c, std::forward<F>(f), details::jackknifed_t{a, c}...);
  }

  /// Pass :ref:`accumulators <triqs__stat__accumulator>`, where the jacknife acts on the :ref:`linear binned data <accumulator_linear_bins>`
  /// @tparam T type of data stored in the accumulators
  template <typename F, typename... T> auto jackknife_mpi(mpi::communicator c, F &&f, accumulator<T> const &... a) {
    return jackknife_mpi(c, std::forward<F>(f), a.linear_bins()...);
  }

} // namespace triqs::stat
